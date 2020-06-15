#include "module.h"

void M_ModuleList_init(M_ModuleList* const self){
    M_SymbolTable_init(&self->dict_symbol);
    
    M_Struct_init(&self->dict, 8);
    M_Tuple_init(&self->list, 8);

    M_FileIO_cwd(&self->current_dir);
}
void M_ModuleList_clear(M_ModuleList* const self){    
    M_Array_clear(&self->current_dir);

    M_Struct_clear(&self->dict);
    M_Tuple_clear(&self->list);

    M_SymbolTable_clear(&self->dict_symbol);
}

void __M_Module_init(M_ModuleList* const self, M_Object* const obj, const M_Symbol* const path, const M_Object* const src){
    M_Tuple tuple;

    M_Object path_obj;
    M_Object_set_symbol(&path_obj, path);

    M_Object id;
    M_Object_set_int(&id, self->list.len);

    M_Tuple_init(&tuple, 3);
    M_Tuple_push(&tuple, &id);
    M_Tuple_push(&tuple, &path_obj);
    M_Tuple_push(&tuple, src); 
    M_Object_set_tuple(obj, tuple);

    M_Object_clear(&id);
    M_Object_clear(&path_obj);
}

const M_Array* M_Module_get_path(M_ModuleList* const self, const size_t module_id){
    M_Object module;
    if(M_Tuple_get(&self->list, module_id, &module) != M_STATUS_OK){
        M_panic("in Module_get_path");
        return NULL;
    }

    M_Object path;
    M_Tuple_get(&module.arc->v_tuple, M_MODULE_PATH, &path);
    return &path.v_symbol->data;
}
bool M_Module_get_id(M_ModuleList* const self, const M_Symbol* const sym, size_t* const id){
    M_Object module;
    
    if(M_Struct_get(&self->dict, sym, &module) != M_STATUS_OK){
        return false;
    }

    M_Object id_obj;
    M_Tuple_get(&module.arc->v_tuple, M_MODULE_ID, &id_obj);
    *id = (size_t)id_obj.v_int;
    M_Object_clear(&id_obj);

    return true;
}

size_t M_ModuleList_import(M_ModuleList* const self, M_Array* const abs_path){
    size_t id;

    const M_Symbol* path_sym;
    M_Object src;
    {
        path_sym = M_Symbol_new(&self->dict_symbol, abs_path);

        if(M_Module_get_id(self, path_sym, &id)){
            return id;
        }

        M_Array module_src;
        if(M_FileIO_loadfile(abs_path, &module_src) != M_STATUS_OK){
            M_panic("in ModuleList_add_file");
            return 0;
        }
        M_Object_set_array(&src, module_src);
    }

    M_Object module_obj;
    __M_Module_init(self, &module_obj, path_sym, &src);
    M_Object_clear(&src);

    M_Struct_set(&self->dict, path_sym, &module_obj);
    
    M_Tuple_push(&self->list, &module_obj);
    M_Object_clear(&module_obj);
    
    M_Struct_print(&self->dict);
    return id;
}
size_t M_ModuleList_import_modrel(M_ModuleList* const self, size_t module_id, const M_Array* const path){
    const M_Array* parent_path = M_Module_get_path(self, module_id);

    if(parent_path == NULL){
        M_panic("in import_rel");
        return 0;
    }

    M_Array abs_path;
    M_File_join_path(&abs_path, parent_path, path);
    
    size_t id = M_ModuleList_import(self, &abs_path);
    M_Array_clear(&abs_path);
    return id;
}
