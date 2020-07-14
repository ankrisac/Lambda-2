#pragma once
#include "core.h"

void M_Status_print(const M_Status error);
void M_Type_print(const M_Type type);
void M_Keyword_print(const M_Keyword keyword);
void M_Symbol_print(const M_Symbol* const val);
void M_Symbol_repr(const M_Symbol* const val);
void M_Char_repr(const M_Char val);

M_ARC_Object* M_ARC_Object_new();

void M_Object_set_none(M_Object* const self);
void M_Object_set_error(M_Object* const self, const M_Status val);
void M_Object_set_keyword(M_Object* const self, const M_Keyword val);
void M_Object_set_type(M_Object* const self, const M_Type val);
void M_Object_set_symbol(M_Object* const self, const M_Symbol* const val);

void M_Object_set_bool(M_Object* const self, const M_Bool val);
void M_Object_set_char(M_Object* const self, const M_Char val);
void M_Object_set_int(M_Object* const self, const M_Int val);
void M_Object_set_float(M_Object* const self, const M_Float val);

void M_Object_set_array(M_Object* const self, const M_Array val);
void M_Object_set_tuple(M_Object* const self, const M_Tuple val);
void M_Object_set_struct(M_Object* const self, const M_Struct val);
void M_Object_set_expr(M_Object* const self, const M_Expr val);

void M_Object_clear(M_Object* const self);
void M_Object_delete(M_Object* const self);

M_Status M_Object_copy(M_Object* const self, const M_Object* const src);
void M_Object_share(M_Object* const self, M_Object* const src);

void M_Object_print(const M_Object* const self);
void M_Object_repr(const M_Object* const self);
void M_Object_desc(const M_Object* const self);

bool M_Object_equal(const M_Object* const self, const M_Object* const src);