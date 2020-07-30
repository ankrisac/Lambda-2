#pragma once
#include "core.h"

void M_Str_append_HTML_char(M_Str* const str, char chr);
void M_Lint_recsyntax_HTML(const M_Expr* const tree, M_Str* const str, const M_Str* const in, size_t* pos);
M_Str M_Lint_syntax_HTML(const M_Object* const tree);

void M_Lint_init(M_Lint* const self);
void M_Lint_clear(M_Lint* const self);

void M_Lint_parse(M_Lint* const self, const M_Str input);
M_Str M_Lint_color_HTML(M_Lint* const self);
M_Str M_Lint_get_errors(M_Lint* const self);