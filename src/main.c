#define _GNU_SOURCE
#include "core.h"
#include "module.h"

#include <dirent.h>


int main(){
    M_ModuleList self;
    M_ModuleList_init(&self);


    M_Array path;

    M_Array_from_cstr(&path, "file1.txt");    
    M_ModuleList_import(&self, &path);
    M_Array_clear(&path);

    M_Array_from_cstr(&path, "file1.txt");    
    M_ModuleList_import(&self, &path);
    M_Array_clear(&path);


    /*
    M_Array_from_cstr(&path, "file2.txt");
    M_ModuleList_import(&self, &path);
    M_ModuleList_import(&self, &path);
    M_Array_clear(&path);
    */

    M_ModuleList_clear(&self);


    return 0;
}
