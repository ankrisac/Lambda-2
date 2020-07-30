#define _GNU_SOURCE
#include "core.h"
#include "module.h"

M_Lint linter;

void M_CompilerJS_init(){
    M_Lint_init(&linter);
}
void M_CompilerJS_quit(){
    M_Lint_clear(&linter);
}

void M_CompilerJS_print_tree(){
    printf("AST: [");
    M_Object_repr(&linter.tree);
    printf("]\n");
}

void M_CompilerJS_parse(const char* in_str){
    M_Str inp;
    M_Str_from_cstr(&inp, in_str);
    M_Lint_parse(&linter, inp);
}
char* M_CompilerJS_color_HTML(){
    M_Str str = M_Lint_color_HTML(&linter);
    M_Str_push(&str, '\0');
    return str.data_char;
}
char* M_CompilerJS_get_errors(){
    M_Str str = M_Lint_get_errors(&linter);
    M_Str_push(&str, '\0');
    return str.data_char;
}

void M_CompilerJS_test(){
    char input[] = "x = R#\"hello\"\"#;";

    M_ErrorStack_revert(&linter.err_stack, 0);

    M_Array_clear(&linter.module.path);
    M_Array_clear(&linter.module.data);
    M_Str_from_cstr(&linter.module.path, "program.ul");
    M_Str_from_cstr(&linter.module.data, input); 
    
    M_Module_Pos pos = M_Module_getbegin(&linter.module);

    M_Object_clear(&linter.tree);
    linter.tree = M_Module_parse_File(&pos, &linter.symtable, &linter.err_stack);
    M_Module_Pos_print(pos);

    M_CompilerJS_print_tree();
}


int main(){
    M_CompilerJS_init();
    M_CompilerJS_test();

    char* output = M_CompilerJS_color_HTML();
    printf("SColor: [%s]\n", output);
    free(output);

    char* error = M_CompilerJS_get_errors();
    printf("Error: [%s]\n", error);
    free(error);

    M_CompilerJS_quit();
    
    return 0;
}
