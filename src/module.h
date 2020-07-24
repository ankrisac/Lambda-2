#pragma once
#include "core.h"

bool M_Module_Pos_equal(const M_Module_Pos* const self, const M_Module_Pos* const other);

void M_Module_loadfile(M_Module* const self, const M_Array* const path);
void M_Module_clear(M_Module* const self);

void M_Module_print(M_Module* const self);
M_Module_Pos M_Module_getbegin(const M_Module* const self);


M_Str M_Module_Pos_print(const M_Module_Pos mpos);

char M_Module_Pos_peek(const M_Module_Pos* const pos);
char M_Module_Pos_pop(M_Module_Pos* const pos);
bool M_Module_Pos_next(M_Module_Pos* const pos);


M_Float M_Module_parse_Frac(M_Module_Pos* const pos);
M_Object M_Module_parse_Number(M_Module_Pos* const pos);

M_Object M_Module_parse_Operator(M_Module_Pos* const pos, M_SymbolTable* const table);
M_Object M_Module_parse_Symbol(M_Module_Pos* const pos, M_SymbolTable* const table);
M_Object M_Module_parse_String(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_Atom(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);

M_Object M_Module_parse_FuncExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_InfixExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);

M_Object M_Module_parse_TupleExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_ListExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_BlockExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_ScopeExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);

void M_ModuleList_init(M_ModuleList* const self);
void M_ModuleList_clear(M_ModuleList* const self);