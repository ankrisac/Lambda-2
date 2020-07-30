#pragma once
#include "core.h"

M_Float M_Module_parse_Frac(M_Module_Pos* const pos);
M_Object M_Module_parse_Number(M_Module_Pos* const pos, M_ErrorStack* const err_trace);

M_Object M_Module_parse_Operator(M_Module_Pos* const pos, M_SymbolTable* const table);
M_Object M_Module_parse_Symbol(M_Module_Pos* const pos, M_SymbolTable* const table);
M_Object M_Module_parse_String(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
M_Object M_Module_parse_Atom(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace);
void M_Module_parse_Space(M_Module_Pos* const pos);