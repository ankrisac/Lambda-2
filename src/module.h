#pragma once
#include "core.h"

void __M_Module_init(M_ModuleList* const self, M_Object* const obj, const M_Symbol* const path, const M_Object* const src);
const M_Array* M_Module_get_path(M_ModuleList* const self, const size_t module_id);
bool M_Module_get_id(M_ModuleList* const self, const M_Symbol* const sym, size_t* const id);

void M_ModuleList_init(M_ModuleList* const self);
void M_ModuleList_clear(M_ModuleList* const self);

size_t M_ModuleList_import(M_ModuleList* const self, M_Array* const abs_path);
size_t M_ModuleList_import_modrel(M_ModuleList* const self, size_t module_id, const M_Array* const path);