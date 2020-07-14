#define _GNU_SOURCE
#include "core.h"
#include "module.h"

int main(){
    M_Str path;
    M_Str_from_cstr(&path, "text.ul");

    M_Module self;
    M_Module_loadfile(&self, &path);

    M_Module_Pos pos = M_Module_getbegin(&self);

    M_SymbolTable symtable;
    M_SymbolTable_init(&symtable);

    M_Object obj = M_Module_parse_InfixExpr(&pos, &symtable);

    if(obj.type != M_TYPE_ERROR){
        printf("Parsed results: [");
        M_Object_repr(&obj);
        printf("]:");
        M_Type_print(obj.type);
        printf("\n");
    }
    else{
        printf("Failed!\n");
    }

    M_Module_Pos_print(pos);
    M_Object_clear(&obj);

    
    M_SymbolTable_clear(&symtable);




    M_Module_clear(&self);
    M_Array_clear(&path);


    return 0;
}
