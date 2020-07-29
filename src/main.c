#define _GNU_SOURCE
#include "core.h"
#include "module.h"

void M_Str_append_HTML_char(M_Str* const str, char chr){
    switch(chr){
        case '\n': M_Str_join_cstr(str, "<br><div class='syntax_newline'></div>"); break;
        case '&': M_Str_join_cstr(str, "&amp;"); break;
        case '<': M_Str_join_cstr(str, "&lt;"); break;
        case '>': M_Str_join_cstr(str, "&gt;"); break;
        default: M_Str_push(str, chr); break;
    }
}
void M_syntax_coloring_recursive_HTML(const M_Expr* const tree, M_Str* const str, const M_Str* const in, size_t* pos){
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
                M_syntax_coloring_recursive_HTML(&elem.arc->v_expr, str, in, pos);
                M_Str_join_cstr(str, "</div>");
                break;
            default:
                M_PANIC_TYPE(elem.type, "")
                break;
        }
    }
}
char* M_syntax_coloring_HTML(const M_Object* const tree){
    M_Str str;
    M_Array_init(&str, M_TYPE_CHAR, 16);

    if(tree->type == M_TYPE_EXPR && tree->arc->v_expr.len > 0){
        const M_Expr expr = tree->arc->v_expr;
        size_t i = 0;
        M_Module_Pos pos = expr.pos[0];
        M_Array in = pos.module->data;

        M_syntax_coloring_recursive_HTML(&expr, &str, &in, &i);

        for(; i < in.len; i++){
            M_Str_append_HTML_char(&str, in.data_char[i]);
        }
    }
    M_Str_append_HTML_char(&str, '\0');
    return str.data_char;
}


typedef struct{
    M_ErrorStack err_stack;
    M_SymbolTable symtable;
    M_Object tree;
    M_Module module;
}M_Server;

M_Server core;

void M_Server_init(){
    M_SymbolTable_init(&core.symtable);
    M_ErrorStack_init(&core.err_stack);
    M_Object_set_none(&core.tree);

    M_Array_init(&core.module.path, M_TYPE_CHAR, 1);
    M_Array_init(&core.module.data, M_TYPE_CHAR, 1);

    printf("=== Lambda-5 ===\n");
}
void M_Server_parse(const char* input){
    M_ErrorStack_revert(&core.err_stack, 0);

    M_Array_clear(&core.module.path);
    M_Array_clear(&core.module.data);
    M_Str_from_cstr(&core.module.path, "program.ul");
    M_Str_from_cstr(&core.module.data, input);
    
    M_Module_Pos pos = M_Module_getbegin(&core.module);

    M_Object_clear(&core.tree);
    core.tree = M_Module_parse_File(&pos, &core.symtable, &core.err_stack);
}
char* M_Server_color_HTML(){
    return M_syntax_coloring_HTML(&core.tree);
}
char* M_Server_get_errors(){
    M_Str out = M_ErrorStack_toStr(&core.err_stack);
    M_Str_push(&out, '\0');
    return out.data_char;
}
void M_Server_print_tree(){
    printf("AST: [");
    M_Object_repr(&core.tree);
    printf("]\n");
}
void M_Server_quit(){
    M_SymbolTable_clear(&core.symtable);
    M_ErrorStack_clear(&core.err_stack);
    M_Object_clear(&core.tree);
    M_Module_clear(&core.module);
}
void M_Server_test(){
    char input[] = "30";

    M_ErrorStack_revert(&core.err_stack, 0);

    M_Array_clear(&core.module.path);
    M_Array_clear(&core.module.data);
    M_Str_from_cstr(&core.module.path, "program.ul");
    M_Str_from_cstr(&core.module.data, input); 
    
    M_Module_Pos pos = M_Module_getbegin(&core.module);

    M_Object_clear(&core.tree);
    core.tree = M_Module_parse_FuncExpr(&pos, &core.symtable, &core.err_stack);
    M_Module_Pos_print(pos);

    M_Server_print_tree();
}


#ifdef __EMSCRIPTEN__
#else
int main(){
    M_Server_init();

    M_Server_test();

    char* output = M_Server_color_HTML();
    printf("SColor: [%s]\n", output);
    free(output);

    char* error = M_Server_get_errors();
    printf("Error: [%s]\n", error);
    free(error);

    M_Server_quit();
    
    return 0;
}
#endif