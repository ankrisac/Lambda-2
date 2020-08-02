#pragma once
#include "../core.h"

void M_Str_push(M_Str* const self, const char val);

void M_Str_from_cstr(M_Array* const self, const char* const str);
void M_Str_from_int(M_Str* const self, const M_Int val);

void M_Str_join(M_Str* const self, const M_Str* const other);
void M_Str_join_cstr(M_Str* const self, const char* const str);
void M_Str_join_int(M_Str* const self, const M_Int val);