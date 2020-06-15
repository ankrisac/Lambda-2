#pragma once
#include "core.h"

void M_Struct_init(M_Struct* const self, const size_t len);
M_Struct M_Struct_new(const size_t len);

void M_Struct_clear(M_Struct* const self);
void M_Struct_delete(M_Struct* const self);

void M_Struct_copy(M_Struct* const self, const M_Struct* const src);

void M_Struct_repr(const M_Struct* const self);
void M_Struct_print(const M_Struct* const self);


M_Status M_Struct_set(M_Struct* const self, const M_Symbol* sym, const M_Object* const obj);
M_Status M_Struct_get(const M_Struct* const self, const M_Symbol* sym, M_Object* const obj);

bool M_Struct_in(const M_Struct* const self, const M_Symbol* sym);
bool M_Struct_equal(const M_Struct* const self, const M_Struct* other);
