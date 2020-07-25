#define _GNU_SOURCE
#include "core.h"
#include "module.h"

void recursive_coloring(const M_Expr* const tree, M_Str* const str, const M_Str* const in, size_t* pos){
    for(size_t i = 0; i < tree->len; i++){
        const M_Object elem = tree->data[i];
        const M_Module_Pos elem_pos = tree->pos[i];

        for(; *pos < elem_pos.begin; (*pos)++){
            M_Str_push(str, in->data_char[*pos]);
        }

        switch(elem.type){
            case M_TYPE_ERROR:
                M_Str_join_cstr(str, "\033[32m");
                for(size_t i = elem_pos.begin; i < elem_pos.end; i++, (*pos)++){
                    M_Str_push(str, in->data_char[i]);
                }
                M_Str_join_cstr(str, "\033[0m");
                break;
            case M_TYPE_INT:
            case M_TYPE_FLOAT:
                M_Str_join_cstr(str, "\033[34m");
                for(size_t i = elem_pos.begin; i < elem_pos.end; i++, (*pos)++){
                    M_Str_push(str, in->data_char[i]);
                }
                M_Str_join_cstr(str, "\033[0m");
                break;
            case M_TYPE_SYMBOL:
                M_Str_join_cstr(str, "\033[1;32m");
                for(size_t i = elem_pos.begin; i < elem_pos.end; i++, (*pos)++){       
                    switch(in->data_char[i]){
                        case '#':
                        case '"':
                            M_Str_join_cstr(str, "\033[1;31m");
                            break;
                        default:
                            break;
                    }
                    M_Str_push(str, in->data_char[i]);
                }
                M_Str_join_cstr(str, "\033[0m");
                break;
            case M_TYPE_EXPR:
                recursive_coloring(&elem.arc->v_expr, str, in, pos);
                break;
            default:
                M_PANIC_TYPE(elem.type, "")
                break;
        }
    }
}
void syntax_coloring(const M_Object* const tree){
    M_Str str;
    M_Str_from_cstr(&str, "");

    if(tree->type == M_TYPE_EXPR){
        const M_Expr expr = tree->arc->v_expr;
        size_t i = 0;
        M_Module_Pos pos = expr.pos[0];
        M_Array in = pos.module->data;

        recursive_coloring(&expr, &str, &in, &i);

        for(; i < in.len; i++){
            M_Str_push(&str, in.data_char[i]);
        }
    }
    M_Array_print(&str);
    M_Array_clear(&str);

    printf("\n");
}

int main(){
    M_Str path;
    M_Str_from_cstr(&path, "text.ul");

    M_Module self;
    M_Module_loadfile(&self, &path);

    M_Module_Pos pos = M_Module_getbegin(&self);

    M_ErrorStack errstack;
    M_ErrorStack_init(&errstack);

    M_SymbolTable symtable;
    M_SymbolTable_init(&symtable);

    M_Object obj = M_Module_parse_File(&pos, &symtable, &errstack);

    if(obj.type != M_TYPE_ERROR){
        printf("Parsed results: [");
        M_Object_repr(&obj);
        printf("]:");
        M_Type_print(obj.type);
        printf("\n");

        M_Module_Pos_print(pos);
    }
    printf("Errors: \n");
    M_ErrorStack_print(&errstack);

    printf("Coloring!:\n");
    syntax_coloring(&obj);

    M_Object_clear(&obj);

    M_SymbolTable_clear(&symtable);
    M_ErrorStack_clear(&errstack);

    M_Module_clear(&self);
    M_Array_clear(&path);

    return 0;
}