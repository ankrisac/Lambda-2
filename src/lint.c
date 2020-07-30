#include "lint.h"

void M_Lint_HTML_append(M_Str* const output, char chr){
    switch(chr){
        case '\n': M_Str_join_cstr(output, "<br><div class='syntax_newline'></div>"); break;
        case '&': M_Str_join_cstr(output, "&amp;"); break;
        case '<': M_Str_join_cstr(output, "&lt;"); break;
        case '>': M_Str_join_cstr(output, "&gt;"); break;
        default: M_Str_push(output, chr); break;
    }
}

void M_Lint_HTML_openTag(M_Str* const output, const char* style){
    M_Str_join_cstr(output, "<div class='syntax_");
    M_Str_join_cstr(output, style);  
    M_Str_join_cstr(output, "'>");
}
void M_Lint_HTML_closeTag(M_Str* const output){
    M_Str_join_cstr(output, "</div>");   
}
void M_Lint_HTML_applystyle(M_Str* const output, const M_Str* const input, const char* style, size_t* iter, const size_t end){
    M_Lint_HTML_openTag(output, style);
    size_t i = *iter;
    for(; i < end; i++){
        M_Lint_HTML_append(output, input->data_char[i]);
    }
    *iter = i;
    M_Lint_HTML_closeTag(output);
}

void M_Lint_HTML_Symbol(M_Str* const output, const M_Str* const input, size_t* iter, const M_Module_Pos elem_pos){
    M_Lint_HTML_applystyle(output, input, "symbol", iter, elem_pos.end);
}
void M_Lint_HTML_Operator(M_Str* const output, const M_Str* const input, size_t* iter, const M_Module_Pos elem_pos){
    M_Lint_HTML_applystyle(output, input, "operator", iter, elem_pos.end);
}
void M_Lint_HTML_RawString(const M_Object* const obj, M_Str* const output, const M_Str* const input, size_t* iter, const M_Module_Pos* elem_pos){
    size_t i = *iter;
    size_t end = elem_pos->end;

    char chr;
    M_Lint_HTML_openTag(output, "string_rawquote");
    for(; i < end; ++i){
        chr = input->data_char[i];
        M_Lint_HTML_append(output, chr);

        if(chr == '"'){
            i++;
            break;
        }
    }
    size_t len = i + obj->v_symbol->data.len;
    len = (len > end) ? end : len;

    M_Lint_HTML_openTag(output, "string_inner");
    for(;i < len; ++i){
        M_Lint_HTML_append(output, input->data_char[i]);
    }
    M_Lint_HTML_closeTag(output);

    for(; i < end; ++i){
        M_Lint_HTML_append(output, input->data_char[i]);
    }
    *iter = i;

    M_Lint_HTML_closeTag(output);
}
void M_Lint_HTML_String(const M_Object* const obj, M_Str* const output, const M_Str* const input, size_t* iter, const M_Module_Pos* elem_pos){
    size_t i = *iter;
    size_t end = elem_pos->end;

    char chr;
    M_Lint_HTML_openTag(output, "string_rawquote");
    for(; i < end; ++i){
        chr = input->data_char[i];
        M_Lint_HTML_append(output, chr);

        if(chr == '"'){
            i++;
            break;
        }
    }
    size_t len = i + obj->v_symbol->data.len;
    len = (len > end) ? end : len;

    chr = '\0';
    M_Lint_HTML_openTag(output, "string_inner");
    for(;i < len; ++i){
        chr = input->data_char[i];

        if(chr == '\\'){
            M_Lint_HTML_openTag(output, "string_escape");
            M_Lint_HTML_append(output, chr);
            
            len++;

            if(++i < len){
                chr = input->data_char[i];
                M_Lint_HTML_append(output, chr);
            }
            M_Lint_HTML_closeTag(output);
        }
        else{
            M_Lint_HTML_append(output, chr);
        }
    }
    M_Lint_HTML_closeTag(output);

    for(; i < end; ++i){
        M_Lint_HTML_append(output, input->data_char[i]);
    }
    *iter = i;

    M_Lint_HTML_closeTag(output);
}
void M_Lint_HTML_Var(const M_Object* const obj, M_Str* const str, const M_Str* const in, size_t* pos, const M_Module_Pos elem_pos){
    switch(in->data_char[*pos]){
        case 'r':
        case 'R':
            if(*pos + 1 < elem_pos.end){
                switch(in->data_char[*pos + 1]){
                    case '#':
                    case '"':
                        M_Lint_HTML_RawString(obj, str, in, pos, &elem_pos);
                        break;
                    default:
                        M_Lint_HTML_Symbol(str, in, pos, elem_pos);
                        break;
                }
            }
            else{
                M_Lint_HTML_Symbol(str, in, pos, elem_pos);
            }
            break;
        case '#':
        case '"':
            M_Lint_HTML_String(obj, str, in, pos, &elem_pos);
            break;
        M_CASE_OPERATOR
            M_Lint_HTML_Operator(str, in, pos, elem_pos);
            break;
        default:              
            M_Lint_HTML_Symbol(str, in, pos, elem_pos);
            break;
    }
}
void M_Lint_HTML_expr(const M_Expr* const tree, M_Str* const str, const M_Str* const in, size_t* pos){
    for(size_t i = 0; i < tree->len; i++){
        const M_Object elem = tree->data[i];
        const M_Module_Pos elem_pos = tree->pos[i];

        for(; *pos < elem_pos.begin; (*pos)++){
            M_Lint_HTML_append(str, in->data_char[*pos]);
        }

        if(elem_pos.begin >= elem_pos.end){
            continue;
        }

        switch(elem.type){
            case M_TYPE_ERROR:
                break;
            case M_TYPE_INT:
            case M_TYPE_FLOAT:
                M_Lint_HTML_applystyle(str, in, "number", pos, elem_pos.end);
                break;
            case M_TYPE_SYMBOL:
                M_Lint_HTML_Var(&elem, str, in, pos, elem_pos);
                break;
            case M_TYPE_EXPR:
                M_Lint_HTML_openTag(str, "expr");
                M_Lint_HTML_expr(&elem.arc->v_expr, str, in, pos);
                M_Lint_HTML_closeTag(str);
                break;
            default:
                M_PANIC_TYPE(elem.type, "")
                break;
        }
    }
}
M_Str M_Lint_HTML(const M_Object* const tree){
    M_Str str;
    M_Array_init(&str, M_TYPE_CHAR, 16);

    if(tree->type == M_TYPE_EXPR && tree->arc->v_expr.len > 0){
        const M_Expr expr = tree->arc->v_expr;
        size_t i = 0;
        M_Module_Pos pos = expr.pos[0];
        M_Array in = pos.module->data;

        M_Lint_HTML_expr(&expr, &str, &in, &i);

        for(; i < in.len; i++){
            M_Lint_HTML_append(&str, in.data_char[i]);
        }
    }
    return str;
}

void M_Lint_init(M_Lint* const self){
    M_SymbolTable_init(&self->symtable);
    M_ErrorStack_init(&self->err_stack);
    M_Object_set_none(&self->tree);

    M_Array_init(&self->module.path, M_TYPE_CHAR, 1);
    M_Array_init(&self->module.data, M_TYPE_CHAR, 1);

    printf("=== Lambda-5 ===\n");
}
void M_Lint_parse(M_Lint* const self, M_Str input){
    M_ErrorStack_revert(&self->err_stack, 0);

    M_Array_clear(&self->module.path);
    M_Array_clear(&self->module.data);
    M_Str_from_cstr(&self->module.path, "program.ul");
    self->module.data = input;
    
    M_Module_Pos pos = M_Module_getbegin(&self->module);

    M_Object_clear(&self->tree);
    self->tree = M_Module_parse_File(&pos, &self->symtable, &self->err_stack);
}
M_Str M_Lint_color_HTML(M_Lint* const self){
    return M_Lint_HTML(&self->tree);
}
M_Str M_Lint_get_errors(M_Lint* const self){
    return M_ErrorStack_toStr(&self->err_stack);
}
void M_Lint_clear(M_Lint* const self){
    M_SymbolTable_clear(&self->symtable);
    M_ErrorStack_clear(&self->err_stack);
    M_Object_clear(&self->tree);
    M_Module_clear(&self->module);
}