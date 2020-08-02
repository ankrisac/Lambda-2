#pragma once
#include "core.h"

void M_JSLint_init(M_JSLint* const self);
void M_JSLint_clear(M_JSLint* const self);

void M_JSLint_parse(M_JSLint* const self, const M_Str input);
M_Str M_JSLint_color(M_JSLint* const self);
M_Str M_JSLint_get_errors(M_JSLint* const self);