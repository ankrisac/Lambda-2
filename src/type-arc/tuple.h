#pragma once
#include "../core.h"

void __M_Tuple_alloc(M_Tuple* const self, const size_t len);
void __M_Tuple_realloc(M_Tuple* const self, const size_t len);
void __M_Tuple_delete(M_Tuple* const self);
void __M_Tuple_grow(M_Tuple* const self);

void M_Tuple_init(M_Tuple* const self, const size_t len);
M_Tuple M_Tuple_new(const size_t len);

void M_Tuple_clear(M_Tuple* const self);
void M_Tuple_delete(M_Tuple* const self);

void M_Tuple_copy(M_Tuple* const self, const M_Tuple* const src);

void M_Tuple_print(const M_Tuple* const self);
void M_Tuple_repr(const M_Tuple* const self);


M_Status M_Tuple_get(const M_Tuple* const self, const M_Int i, M_Object* const obj);
M_Status M_Tuple_set(M_Tuple* const self, const  M_Int i, const M_Object* const obj);

M_Status M_Tuple_push(M_Tuple* const self, const M_Object* const obj);
M_Status M_Tuple_pop(M_Tuple* const self, M_Object* const obj);
M_Status M_Tuple_pop_copy(M_Tuple* const self, M_Object* const obj);

M_Status M_Expr_drop(M_Expr* const self, size_t n);
M_Status M_Expr_dropn(M_Expr* const self, size_t n);

void M_Tuple_join(M_Tuple* const self, const M_Tuple* const other);
M_Status M_Tuple_splice(M_Tuple* const self, const M_Tuple* const other, const size_t begin, const size_t end);
void M_Tuple_reverse(M_Tuple* const self);

bool M_Tuple_equal(const M_Tuple* const self, const M_Tuple* const other);
