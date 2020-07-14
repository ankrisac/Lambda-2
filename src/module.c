#include "module.h"

bool M_Module_Pos_equal(const M_Module_Pos* const self, const M_Module_Pos* const other){
    return self == other || 
        (self->module == other->module
            && self->begin == other->begin 
            && self->end == other->end 
            && self->line == other->line);
}
void M_Module_loadfile(M_Module* const self, const M_Array* const path){
    M_Array_copy(&self->path, path);

    if(M_FileIO_loadfile(&self->path, &self->data) != M_STATUS_OK){
        printf("Failed to load file\n");
    }
}
void M_Module_clear(M_Module* const self){
    M_Array_clear(&self->path);
    M_Array_clear(&self->data);
}

void M_Module_print(M_Module* const self){
    printf("[");
    M_Array_print(&self->path);
    printf("]\n");
    M_Array_print(&self->data);
    printf("\n");
}

M_Module_Pos M_Module_getbegin(const M_Module* const self){
    return (M_Module_Pos){ .module = self, .begin = 0, .end = 0, .line = 1 }; 
}

M_Str M_Module_Pos_print(const M_Module_Pos mpos){
    //pos is not validated

    #define RESET "\033[0m"
    #define COLOR "\033[31m\033[1m\033[4m"

    M_Str outln;
    M_Str_from_cstr(&outln, "in \033[03m'");
    M_Str_join(&outln, &mpos.module->path);
    M_Str_join_cstr(&outln, RESET "':\n");

    size_t begin = mpos.begin;
    size_t end = mpos.end;
    size_t len = mpos.module->data.len;
    size_t line = mpos.line;

    const char* data = mpos.module->data.data_char;

    while(begin > 0){
        if(data[begin] == '\n'){
            begin++;
            break;
        }
        begin--;
    }

    while(end < len){
        if(data[end] == '\n'){
            break;
        }
        end++;
    }

    for(size_t i = mpos.begin; i < mpos.end; i++){
        if(data[i] == '\n'){
            line--;
        }
    }

    M_Str_join_int(&outln, line);
    M_Str_join_cstr(&outln, " | " COLOR);
    for(size_t i = begin; i < mpos.begin; i++){
        M_Str_push(&outln, data[i]);
    }

    for(size_t i = mpos.begin; i < mpos.end; i++){
        if(data[i] == '\n'){
            line++;
            M_Str_join_cstr(&outln, RESET "\n");
            M_Str_join_int(&outln, line);
            M_Str_join_cstr(&outln, " | " COLOR);
        }
        else{
            M_Str_push(&outln, data[i]);
        }
    }

    M_Str_join_cstr(&outln, RESET);
    for(size_t i = mpos.end; i < end; i++){
        M_Str_push(&outln, data[i]);
    }
    M_Str_push(&outln, '\n');

    M_Array_print(&outln);
    M_Array_clear(&outln);
    return outln;
}

char M_Module_Pos_peek(const M_Module_Pos* const pos){
    if(pos->end < pos->module->data.len){
        return pos->module->data.data_char[pos->end];
    }
    return '\0';
}
char M_Module_Pos_peekahead(const M_Module_Pos* const pos, size_t n){
    if(pos->end + n < pos->module->data.len){
        return pos->module->data.data_char[pos->end + n];
    }
    return '\0';
}
char M_Module_Pos_pop(M_Module_Pos* const pos){
    if(pos->end < pos->module->data.len){
        char val = pos->module->data.data_char[pos->end];
        if(val == '\n'){
            pos->line++;
        }

        pos->end++;
        return val;
    }
    return '\0';
}
bool M_Module_Pos_next(M_Module_Pos* const pos){
    if(pos->end < pos->module->data.len){
        char val = pos->module->data.data_char[pos->end];
        if(val == '\n'){
            pos->line++;
        }

        pos->end++;
        return true;
    }
    return false;
}
bool M_Module_Pos_prev(M_Module_Pos* const pos){
    if(pos->begin < pos->end){
        pos->end--;

        char val = pos->module->data.data_char[pos->end];
        if(val == '\n'){
            pos->line--;
        }

        return true;
    }
    return false;
}
void M_Module_Pos_advance(M_Module_Pos* const pos, const M_Module_Pos* const src){
    pos->end = src->end;
    pos->line = src->line;
}

#define CASE_SPACE          \
    case ' ':   case'\n':   \
    case '\t':  case '\r':

#define CASE_STRING         \
    case '"':   case '#':

#define CASE_SYNTAX         \
    case ';':               \
    case '(':   case ')':   \
    case '[':   case ']':   \
    case '{':   case '}':

#define CASE_NUM                                                \
    case '0':   case '1':   case '2':   case '3':   case '4':   \
    case '5':   case '6':   case '7':   case '8':   case '9':

#define CASE_OPERATOR                   \
    case '=':   case '<':   case '>':   \
    case '.':   case ':':   case '$':   \
    case '+':   case '-':   case '*':   \
    case '`':   case '@':               \
    case '~':   case '!':               \
    case '/':   case '|':   case '\\':  \
    case '%':   case '^':   case '&':   \
    

M_Float M_Module_parse_Frac(M_Module_Pos* const pos){
    M_Float frac = 0, exp = 1.0;

    do{
        char chr = M_Module_Pos_peek(pos);
        switch(chr){
            CASE_NUM{
                exp *= 0.1;
                frac += exp * (M_Float)(chr - '0');
                break;
            }
            case '_':
                break;
            default:
                return frac;
        }
    } while(M_Module_Pos_next(pos));

    return frac;
}
M_Object M_Module_parse_Number(M_Module_Pos* const pos){
    M_Object out = (M_Object){
        .type = M_TYPE_ERROR,
        .v_error = M_STATUS_PARSE_ERROR
    };

    M_Int integer = 0;
    char chr;

    switch((chr = M_Module_Pos_peek(pos))){
        CASE_NUM break;
        default:
            return out;
    }

    do{
        switch((chr = M_Module_Pos_peek(pos))){
            case '_':
                break;
            CASE_NUM { 
                integer = 10 * integer + (M_Int)(chr - '0');
                break;
            }
            case '.':
                M_Module_Pos_next(pos);
                switch((chr = M_Module_Pos_peek(pos))){
                    CASE_NUM
                        return (M_Object) { 
                            .type = M_TYPE_FLOAT, 
                            .v_float = (float)integer + M_Module_parse_Frac(pos) 
                        };
                    default:
                        M_Module_Pos_prev(pos);
                        return (M_Object){
                            .type = M_TYPE_INT,
                            .v_int = integer
                        };
                }
                break;
            default:
                return (M_Object){
                    .type = M_TYPE_INT,
                    .v_int = integer
                };
        }
    } while(M_Module_Pos_next(pos));
    return out;
}
M_Object M_Module_parse_Operator(M_Module_Pos* const pos, M_SymbolTable* const table){
    M_Object out = (M_Object){ 
        .type = M_TYPE_ERROR, 
        .v_error = M_STATUS_PARSE_ERROR 
    };
    M_Module_Pos iter = *pos;
    
    M_Str str;
    char chr;

    M_Array_init(&str, M_TYPE_CHAR, 8);
    do{
        switch((chr = M_Module_Pos_peek(&iter))){
            CASE_OPERATOR{
                M_Str_push(&str, chr);
                break;
            }
            case '\0':
                M_Array_clear(&str);    
                return out;
            default: {
                if(str.len > 0){
                    M_Object_set_symbol(&out, M_Symbol_new(table, &str));
                    M_Module_Pos_advance(pos, &iter);
                }                
                M_Array_clear(&str);    
                return out;
            }
        }
    } while(M_Module_Pos_next(&iter));

    M_Array_clear(&str);
    return out;
}
M_Object M_Module_parse_Symbol(M_Module_Pos* const pos, M_SymbolTable* const table){
    M_Object out = (M_Object){ 
        .type = M_TYPE_ERROR, 
        .v_error = M_STATUS_PARSE_ERROR
    };
    M_Module_Pos iter = *pos;
    
    M_Str str;
    char chr;

    M_Array_init(&str, M_TYPE_CHAR, 8);
    do{
        switch((chr = M_Module_Pos_peek(&iter))){ 
            CASE_SPACE 
            CASE_NUM 
            CASE_STRING
            CASE_SYNTAX 
            CASE_OPERATOR 
            case '\0': {
                if(str.len > 0){
                    M_Object_set_symbol(&out, M_Symbol_new(table, &str));
                    M_Module_Pos_advance(pos, &iter);
                }
                M_Array_clear(&str);
                return out;
            } 
            default:
                M_Str_push(&str, chr);
                break;
        }
    } while(M_Module_Pos_next(&iter));

    M_Array_clear(&str);
    return out;
}

M_Object M_Module_parse_String(M_Module_Pos* const pos, M_SymbolTable* const table){
    M_Object out = (M_Object){ 
        .type = M_TYPE_ERROR, 
        .v_error = M_STATUS_PARSE_ERROR 
    };
    
    bool raw = false;
    switch(M_Module_Pos_peek(pos)){
        case 'r':
        case 'R':
            M_Module_Pos_next(pos);
            raw = true;
            break;  
        default:
            break;
    }

    size_t escape_count = 0;
    while(M_Module_Pos_peek(pos) == '#'){
        M_Module_Pos_next(pos);
        escape_count++;
    }

    if(M_Module_Pos_peek(pos) != '"' || !M_Module_Pos_next(pos)){ 
        return out;
    }            

    M_Str str;
    char chr;

    M_Array_init(&str, M_TYPE_CHAR, 8);
    do {    
        switch((chr = M_Module_Pos_peek(pos))){
            case '"': { 
                M_Str_push(&str, chr);        
                size_t i;

                for(i = 0; i < escape_count && M_Module_Pos_next(pos); i++) {
                    M_Str_push(&str, (chr = M_Module_Pos_peek(pos)));
                    if(chr != '#') {
                        break;
                    }
                } 

                if(i == escape_count){ 
                    M_Array_dropn(&str, escape_count + 1);

                    M_Module_Pos_next(pos);

                    M_Expr expr;
                    M_Expr_init(&expr, 2);
                    M_Expr_push(&expr, 
                        &(M_Object){ 
                            .type = M_TYPE_SYMBOL,
                            .v_symbol = M_Symbol_from_cstr(table, "str")
                        },
                        pos
                    );
                    M_Expr_push(&expr, 
                        &(M_Object){ 
                            .type = M_TYPE_SYMBOL,
                            .v_symbol = M_Symbol_new(table, &str)
                        },
                        pos
                    );
                    M_Array_clear(&str);
                    
                    M_Object_set_expr(&out, expr);
                    return out;
                }
                break;
            }
            case '\\': 
                if(raw){
                    M_Str_push(&str, chr);
                }
                else{
                    if(!M_Module_Pos_next(pos)){ 
                        M_Array_clear(&str);
                        return out;
                    }

                    switch(chr = M_Module_Pos_peek(pos)){
                        case 't': M_Str_push(&str, '\t'); break;
                        case 'r': M_Str_push(&str, '\r'); break;
                        case 'n': M_Str_push(&str, '\n'); break;
                        case 'f': M_Str_push(&str, '\f'); break;
                        case 'b': M_Str_push(&str, '\b'); break;
                        default: M_Str_push(&str, chr); break;
                    }
                }
                break;
            default:
                M_Str_push(&str, chr);
                break;
        }
    } while(M_Module_Pos_next(pos));        
     
    M_Array_clear(&str);    
    return out;
}
M_Object M_Module_parse_Var(M_Module_Pos* const pos, M_SymbolTable* const table){
    switch(M_Module_Pos_peek(pos)){
        CASE_NUM {
            return M_Module_parse_Number(pos);
        }
        CASE_SPACE
        CASE_OPERATOR 
        CASE_SYNTAX {
            break;
        }
        case 'r':
        case 'R':
            switch(M_Module_Pos_peekahead(pos, 1)){
                case '#':
                case '"':{
                    return M_Module_parse_String(pos, table);
                }
                default:{
                    return M_Module_parse_Symbol(pos, table);
                }
            }
            break;
        case '#':
        case '"':{
            return M_Module_parse_String(pos, table);
        }
        default:{
            return M_Module_parse_Symbol(pos, table);
            break;
        }
    }
    return (M_Object){ 
        .type = M_TYPE_ERROR, 
        .v_error = M_STATUS_PARSE_ERROR 
    };
}

void M_Module_parse_Space(M_Module_Pos* const pos){
    while(true){
        switch(M_Module_Pos_peek(pos)){
            CASE_SPACE {
                M_Module_Pos_next(pos);
                continue;
            }
            default:
                break;
        }
        break;
    }
}
M_Object M_Module_parse_FuncExpr(M_Module_Pos* const pos, M_SymbolTable* const table){
    M_Module_Pos iter = *pos;
    M_Object head = M_Module_parse_Var(&iter, table);
    
    if(head.type == M_TYPE_ERROR){
        return head;
    }
    
    M_Expr f_expr;
    M_Expr_init(&f_expr, 2);
    M_Expr_push(&f_expr, &head, &iter);

    M_Module_parse_Space(&iter);
    
    while(true){
        M_Module_parse_Space(&iter);
        iter.begin = iter.end;

        M_Object tail = M_Module_parse_Var(&iter, table);

        if(tail.type == M_TYPE_ERROR){
            break;
        }
        
        M_Expr_push(&f_expr, &tail, &iter);
    }


    M_Module_Pos_advance(pos, &iter);
    if(f_expr.len == 1){
        M_Expr_clear(&f_expr);
        return head;
    }

    M_Object f_obj;
    M_Object_set_expr(&f_obj, f_expr);
    return f_obj;
}
M_Object M_Module_parse_InfixExpr(M_Module_Pos* const pos, M_SymbolTable* const table){
    M_Module_Pos iter = *pos;
    M_Object out = (M_Object){
        .type = M_TYPE_ERROR,
        .v_error = M_STATUS_PARSE_ERROR
    };

    M_Expr expr;
    M_Expr_init(&expr, 2);
    M_Expr_push(&expr, 
        &(M_Object){
            .type = M_TYPE_SYMBOL,
            .v_symbol = M_Symbol_from_cstr(table, "infix")
        },
        &iter
    );

    M_Object atom;
    M_Object op;
    while(true){
        M_Module_parse_Space(&iter);

        atom = M_Module_parse_FuncExpr(&iter, table);
        if(atom.type == M_TYPE_ERROR){
            op = M_Module_parse_Operator(&iter, table);        
            if(op.type == M_TYPE_ERROR){
                break;
            }

            M_Expr_push(&expr, &op, &iter);    
        }

        M_Expr_push(&expr, &atom, &iter);            
    }

    M_Module_Pos_advance(pos, &iter);
    if(expr.len == 2){
        out = expr.data[1];
        M_Expr_clear(&expr);
        return out;
    }
    M_Object_set_expr(&out, expr);

    return out;
}

void M_ModuleList_init(M_ModuleList* const self){
    M_SymbolTable_init(&self->path_list);
    
    self->modules_len = 0;
    self->modules_reserve_len = 16;
    self->modules = M_malloc(self->modules_reserve_len, sizeof* self->modules);
}
void M_ModuleList_clear(M_ModuleList* const self){
    M_SymbolTable_clear(&self->path_list);
    
    free(self->modules);
}
