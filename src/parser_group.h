#pragma once
#include "core.h"

M_Object M_Module_parse_FuncExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_InfixExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);

M_Object M_Module_parse_TupleExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_ListExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_BlockExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_File(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);