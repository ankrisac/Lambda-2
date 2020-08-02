#include "lint.h"

void M_JSLint_append(M_JSLint* const self, char chr){
    switch(chr){
        case '\n': 
            for(size_t i = 0; i < self->style_stack.len; i++){    
                M_Str_join_cstr(&self->output, "</div>");
            }

            M_Str_join_cstr(&self->output, "</div><br><div class='editor_line'>"); 
            
            for(size_t i = 0; i < self->style_stack.len; i++){
                M_Str_join_cstr(&self->output, "<div class='syntax_");
                M_Str_join(&self->output, &self->style_stack.data_arc[i]->v_array);  
                M_Str_join_cstr(&self->output, "'>");
            }
            break;
        case '&': M_Str_join_cstr(&self->output, "&amp;"); break;
        case '<': M_Str_join_cstr(&self->output, "&lt;"); break;
        case '>': M_Str_join_cstr(&self->output, "&gt;"); break;
        default: M_Str_push(&self->output, chr); break;
    }
}

void M_JSLint_openTag(M_JSLint* const self, const char* style){
    M_Str style_str;
    M_Str_from_cstr(&style_str, style); 
    __M_Array_push_Array(&self->style_stack, &style_str);
    M_Array_clear(&style_str);

    M_Str_join_cstr(&self->output, "<div class='syntax_");
    M_Str_join_cstr(&self->output, style);  
    M_Str_join_cstr(&self->output, "'>");
}
void M_JSLint_closeTag(M_JSLint* const self){    
    M_Str_join_cstr(&self->output, "</div>");
    M_Array_drop(&self->style_stack);
}
void M_JSLint_applystyle(M_JSLint* const self, const char* style, size_t* iter, const size_t end){
    M_JSLint_openTag(self, style);
    size_t i = *iter;
    for(; i < end; i++){
        M_JSLint_append(self, self->input.data_char[i]);
    }
    *iter = i;
    M_JSLint_closeTag(self);
}

void M_JSLint_color_Symbol(M_JSLint* const self, const M_Object* const obj, size_t* iter, const M_Module_Pos elem_pos){
    if(obj->v_symbol == M_Symbol_from_cstr(&self->symtable, "def")){
        M_JSLint_applystyle(self, "keyword", iter, elem_pos.end);
    }
    else{
        M_JSLint_applystyle(self, "symbol", iter, elem_pos.end);
    }
}
void M_JSLint_color_Operator(M_JSLint* const self, size_t* iter, const M_Module_Pos elem_pos){
    M_JSLint_applystyle(self, "operator", iter, elem_pos.end);
}
void M_JSLint_color_String(M_JSLint* const self, const M_Object* const obj, size_t* iter, const M_Module_Pos* elem_pos, bool raw){
    size_t i = *iter;
    size_t end = elem_pos->end;

    char chr;
    M_JSLint_openTag(self, "string_quote");
    for(; i < end; ++i){
        chr = self->input.data_char[i];
        M_JSLint_append(self, chr);

        if(chr == '"'){
            i++;
            break;
        }
    }
    size_t len = i + obj->v_symbol->data.len;
    len = (len > end) ? end : len;

    chr = '\0';
    M_JSLint_openTag(self, "string_inner");
    for(;i < len; ++i){
        chr = self->input.data_char[i];

        if(!raw && chr == '\\'){
            M_JSLint_openTag(self, "string_escape");
            M_JSLint_append(self, chr);
            
            len++;

            if(++i < len){
                chr = self->input.data_char[i];
                M_JSLint_append(self, chr);
            }
            M_JSLint_closeTag(self);
        }
        else{
            M_JSLint_append(self, chr);
        }
    }
    M_JSLint_closeTag(self);

    for(; i < end; ++i){
        M_JSLint_append(self, self->input.data_char[i]);
    }
    *iter = i;

    M_JSLint_closeTag(self);
}
void M_JSLint_color_Var(M_JSLint* const self, const M_Object* const obj, size_t* pos, const M_Module_Pos elem_pos){
    switch(self->input.data_char[*pos]){
        case 'r':
        case 'R':
            if(*pos + 1 < elem_pos.end){
                switch(self->input.data_char[*pos + 1]){
                    case '#':
                    case '"':
                        M_JSLint_color_String(self, obj, pos, &elem_pos, true);
                        break;
                    default:
                        M_JSLint_color_Symbol(self, obj, pos, elem_pos);
                        break;
                }
            }
            else{
                M_JSLint_color_Symbol(self, obj, pos, elem_pos);
            }
            break;
        case '#':
        case '"':       M_JSLint_color_String(self, obj, pos, &elem_pos, false);    break;
        M_CASE_OPERATOR M_JSLint_color_Operator(self, pos, elem_pos);               break;
        default:        M_JSLint_color_Symbol(self, obj, pos, elem_pos);                 break;
    }
}
void M_JSLint_color_Expr(M_JSLint* const self, const M_Expr* const tree, size_t* pos){
    for(size_t i = 0; i < tree->len; i++){
        const M_Object elem = tree->data[i];
        const M_Module_Pos elem_pos = tree->pos[i];

        for(; *pos < elem_pos.begin; (*pos)++){
            M_JSLint_append(self, self->input.data_char[*pos]);
        }

        if(elem_pos.begin >= elem_pos.end){
            continue;
        }

        switch(elem.type){
            case M_TYPE_ERROR:
                break;
            case M_TYPE_INT:
            case M_TYPE_FLOAT:
                M_JSLint_applystyle(self, "number", pos, elem_pos.end);
                break;
            case M_TYPE_SYMBOL:
                M_JSLint_color_Var(self, &elem, pos, elem_pos);
                break;
            case M_TYPE_EXPR:
                M_JSLint_openTag(self, "expr");
                M_JSLint_color_Expr(self, &elem.arc->v_expr, pos);
                M_JSLint_closeTag(self);
                break;
            default:
                M_PANIC_TYPE(elem.type, "")
                break;
        }
    }
}
M_Str M_JSLint_color(M_JSLint* const self){
    M_Str_from_cstr(&self->output, "<div class='editor_line'>");
    const M_Object tree = self->tree;

    if(tree.type == M_TYPE_EXPR && tree.arc->v_expr.len > 0){
        const M_Expr expr = tree.arc->v_expr;
        size_t i = 0;
        M_Module_Pos pos = expr.pos[0];
        self->input = pos.module->data;

        M_JSLint_color_Expr(self, &expr, &i);

        for(; i < self->input.len; i++){
            M_JSLint_append(self, self->input.data_char[i]);
        }
    }
    M_Str_join_cstr(&self->output, "</div>");
    return self->output;
}

void M_JSLint_init(M_JSLint* const self){
    M_SymbolTable_init(&self->symtable);
    M_ErrorStack_init(&self->err_stack);
    M_Object_set_none(&self->tree);
    M_Array_init(&self->style_stack, M_TYPE_ARRAY, 1);

    M_Array_init(&self->module.path, M_TYPE_CHAR, 1);
    M_Array_init(&self->module.data, M_TYPE_CHAR, 1);

    printf("=== Lambda-5 ===\n");
}
void M_JSLint_parse(M_JSLint* const self, M_Str input){
    M_ErrorStack_revert(&self->err_stack, 0);

    M_Array_clear(&self->module.path);
    M_Array_clear(&self->module.data);
    M_Str_from_cstr(&self->module.path, "program.ul");
    self->module.data = input;
    
    M_Module_Pos pos = M_Module_getbegin(&self->module);

    M_Object_clear(&self->tree);
    self->tree = M_Module_parse_File(&pos, &self->symtable, &self->err_stack);
}
M_Str M_JSLint_get_errors(M_JSLint* const self){
    return M_ErrorStack_toStr(&self->err_stack);
}
void M_JSLint_clear(M_JSLint* const self){
    M_SymbolTable_clear(&self->symtable);
    M_ErrorStack_clear(&self->err_stack);
    M_Object_clear(&self->tree);
    M_Module_clear(&self->module);

    M_Array_clear(&self->style_stack);
}