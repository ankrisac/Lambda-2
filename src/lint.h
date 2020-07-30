#pragma once
#include "core.h"

M_Str M_Lint_HTML(const M_Object* const tree);

void M_Lint_init(M_Lint* const self);
void M_Lint_clear(M_Lint* const self);

void M_Lint_parse(M_Lint* const self, const M_Str input);
M_Str M_Lint_color_HTML(M_Lint* const self);
M_Str M_Lint_get_errors(M_Lint* const self);