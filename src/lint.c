#include "lint.h"

void M_Str_append_HTML_char(M_Str* const str, char chr){
    switch(chr){
        case '\n': M_Str_join_cstr(str, "<br><div class='syntax_newline'></div>"); break;
        case '&': M_Str_join_cstr(str, "&amp;"); break;
        case '<': M_Str_join_cstr(str, "&lt;"); break;
        case '>': M_Str_join_cstr(str, "&gt;"); break;
        default: M_Str_push(str, chr); break;
    }
}
void M_Lint_recsyntax_HTML(const M_Expr* const tree, M_Str* const str, const M_Str* const in, size_t* pos){
    for(size_t i = 0; i < tree->len; i++){
        const M_Object elem = tree->data[i];
        const M_Module_Pos elem_pos = tree->pos[i];

        for(; *pos < elem_pos.begin; (*pos)++){
            M_Str_append_HTML_char(str, in->data_char[*pos]);
        }

        if(elem_pos.begin >= elem_pos.end){
            continue;
        }

        switch(elem.type){
            case M_TYPE_ERROR:
                M_Str_join_cstr(str, "<div class='syntax_error'>");
                for(;*pos < elem_pos.end; (*pos)++){
                    M_Str_append_HTML_char(str, in->data_char[*pos]);
                }
                M_Str_join_cstr(str, "</div>");
                break;
            case M_TYPE_INT:
            case M_TYPE_FLOAT:
                M_Str_join_cstr(str, "<div class='syntax_number'>");
                for(;*pos < elem_pos.end; (*pos)++){
                    M_Str_append_HTML_char(str, in->data_char[*pos]);
                }
                M_Str_join_cstr(str, "</div>");
                break;
            case M_TYPE_SYMBOL:
                switch(in->data_char[*pos]){
                    case '"':
                        M_Str_join_cstr(str, "<div class='syntax_string'>");
                        break;
                    M_CASE_OPERATOR
                        M_Str_join_cstr(str, "<div class='syntax_operator'>");
                        break;
                    default:              
                        M_Str_join_cstr(str, "<div class='syntax_symbol'>");
                        break;
                }

                for(;*pos < elem_pos.end; (*pos)++){       
                    M_Str_append_HTML_char(str, in->data_char[*pos]);
                }
                M_Str_join_cstr(str, "</div>");
                break;
            case M_TYPE_EXPR:
                M_Str_join_cstr(str, "<div class='syntax_expr'>");
                M_Lint_recsyntax_HTML(&elem.arc->v_expr, str, in, pos);
                M_Str_join_cstr(str, "</div>");
                break;
            default:
                M_PANIC_TYPE(elem.type, "")
                break;
        }
    }
}
M_Str M_Lint_syntax_HTML(const M_Object* const tree){
    M_Str str;
    M_Array_init(&str, M_TYPE_CHAR, 16);

    if(tree->type == M_TYPE_EXPR && tree->arc->v_expr.len > 0){
        const M_Expr expr = tree->arc->v_expr;
        size_t i = 0;
        M_Module_Pos pos = expr.pos[0];
        M_Array in = pos.module->data;

        M_Lint_recsyntax_HTML(&expr, &str, &in, &i);

        for(; i < in.len; i++){
            M_Str_append_HTML_char(&str, in.data_char[i]);
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
    return M_Lint_syntax_HTML(&self->tree);
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