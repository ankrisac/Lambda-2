#pragma once
#include "core.h"

void __M_Array_alloc(M_Array* const self, const size_t len);
void __M_Array_realloc(M_Array* const self, const size_t len);
void __M_Array_delete(M_Array* const self);
void __M_Array_grow(M_Array* const self);

void M_Array_init(M_Array* const self, const M_Type type, const size_t len);
M_Array M_Array_new(const M_Type, const size_t len);

void M_Array_from_cstr(M_Array* const self, const char* const str);

void M_Array_clear(M_Array* const self);
void M_Array_delete(M_Array* const self);

void M_Array_copy(M_Array* const self, const M_Array* const src);

void M_Array_print(const M_Array* const self);
void M_Array_repr(const M_Array* const self);
void M_Array_desc(const M_Array* const self);


void __M_Array_push_Type(M_Array* const self, const M_Type val);
void __M_Array_push_Keyword(M_Array* const self, const M_Keyword val);
void __M_Array_push_Symbol(M_Array* const self, const M_Symbol* const val);

void __M_Array_push_Bool(M_Array* const self, const M_Bool val);
void __M_Array_push_Char(M_Array* const self, const M_Char val);
void __M_Array_push_Int(M_Array* const self, const M_Int val);
void __M_Array_push_Float(M_Array* const self, const M_Float val);

void __M_Array_push_Array(M_Array* const self, const M_Array* const val);


M_Status M_Array_get(M_Array* const self, const M_Int i, M_Object* const obj);
M_Status M_Array_set(M_Array* const self, const M_Int i, const M_Object* const obj);

M_Status M_Array_push(M_Array* const self, const M_Object* const obj);
M_Status M_Array_drop(M_Array* const self);
M_Status M_Array_pop(M_Array* const self, M_Object* const obj);
M_Status M_Array_pop_copy(M_Array* const self, M_Object* const obj);

M_Status M_Array_join(M_Array* const self, const M_Array* const other);
M_Status M_Array_splice(M_Array* const self, const M_Array* const other, const M_Int begin, const M_Int end);


short M_Array_compare(const M_Array* const self, const M_Array* const other);
bool M_Array_equal(const M_Array* const self, const M_Array* const other);

void M_Array_reverse(M_Array* const self);
