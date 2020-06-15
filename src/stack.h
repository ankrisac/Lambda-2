#pragma once
#include "core.h"

typedef struct {
    M_Object* data;
    size_t len;
    size_t stack_ptr;
}M_Stack;

void M_Stack_init(M_Stack* const self);
void M_Stack_clear(M_Stack* const self);

void M_Stack_push_none(M_Stack* const self);
void M_Stack_push_error(M_Stack* const self, const M_Status status);
void M_Stack_push_keyword(M_Stack* const self, const M_Keyword val);
void M_Stack_push_symbol(M_Stack* const self, const M_Symbol* const val);

void M_Stack_push_bool(M_Stack* const self, const M_Bool val);
void M_Stack_push_char(M_Stack* const self, const M_Char val);
void M_Stack_push_int(M_Stack* const self, const M_Int val);
void M_Stack_push_float(M_Stack* const self, const M_Float val);

void M_Stack_push_array(M_Stack* const self, const M_Array val);
void M_Stack_push_tuple(M_Stack* const self, const M_Tuple val);
void M_Stack_push_struct(M_Stack* const self, const M_Struct val);

void M_Stack_push_cstr(M_Stack* const self, const char* const str);

void M_Stack_push_preshared(M_Stack* const self, M_Object* const obj);
void M_Stack_push(M_Stack* const self, M_Object* const obj);
void M_Stack_push_copy(M_Stack* const self, const M_Object* const obj);
void M_Stack_pop(M_Stack* const self, M_Object* const obj);
void M_Stack_pop_copy(M_Stack* const self, M_Object* const obj);

void M_Stack_drop(M_Stack* const self);
void M_Stack_dropn(M_Stack* const self, const size_t n);

void M_Stack_print(const M_Stack* const self);


M_Status M_Stack_fn_print(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_input(M_Stack* const self, const size_t n);


M_Status M_Stack_fn_add(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_sub(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_mul(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_div(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_mod(M_Stack* const self, const size_t n);

M_Status M_Stack_fn_bitwise_and(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_bitwise_or(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_bitwise_xor(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_bitwise_not(M_Stack* const self, const size_t n);

M_Status M_Stack_fn_and(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_or(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_not(M_Stack* const self, const size_t n);


M_Status M_Stack_fn_seqget(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_seqset(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_seqpush(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_seqpop(M_Stack* const self, const size_t n);

M_Status M_Stack_fn_seqjoin(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_seqsplice(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_seqreverse(M_Stack* const self, const size_t n);


M_Status M_Stack_fn_neq(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_eq(M_Stack* const self, const size_t n);

M_Status M_Stack_fn_lt(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_lte(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_gt(M_Stack* const self, const size_t n);
M_Status M_Stack_fn_gte(M_Stack* const self, const size_t n);