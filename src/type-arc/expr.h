#pragma once
#include "../core.h"

void __M_Expr_alloc(M_Expr* const self, const size_t len);
void __M_Expr_realloc(M_Expr* const self, const size_t len);
void __M_Expr_delete(M_Expr* const self);
void __M_Expr_grow(M_Expr* const self);

void M_Expr_init(M_Expr* const self, const size_t len);
M_Expr M_Expr_new(const size_t len);

void M_Expr_clear(M_Expr* const self);
void M_Expr_delete(M_Expr* const self);

void M_Expr_copy(M_Expr* const self, const M_Expr* const src);

void M_Expr_print(const M_Expr* const self);
void M_Expr_repr(const M_Expr* const self);

M_Status M_Expr_push(M_Expr* const self, const M_Object* const obj, const M_Module_Pos* const pos);
M_Status M_Expr_drop(M_Expr* const self, size_t n);
M_Status M_Expr_dropn(M_Expr* const self, size_t n);

void M_Expr_join(M_Expr* const self, const M_Expr* const other);
M_Status M_Expr_splice(M_Expr* const self, const M_Expr* const other, const size_t begin, const size_t end);
void M_Expr_reverse(M_Expr* const self);
bool M_Expr_equal(const M_Expr* const self, const M_Expr* const other);