#pragma once
#include "core.h"

void M_SymbolTable_init(M_SymbolTable* const self);
void M_SymbolTable_clear(M_SymbolTable* const self);
void M_SymbolTable_print(const M_SymbolTable* const self);

const M_Symbol* M_Symbol_new(M_SymbolTable* const self, const M_Array* const str);
const M_Symbol* M_Symbol_from_cstr(M_SymbolTable* const self, const char* const str);