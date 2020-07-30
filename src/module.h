#pragma once
#include "core.h"

#ifdef __linux__
#define M_UNDERLINE "\033[4m"
#define M_RESET "\033[0m"
#define M_COLOR "\033[31m\033[1m\033[4m"
#define M_ITALIC "\033[3m"
#else
#define M_RESET "</div>"
#define M_UNDERLINE "<div class='syntax_underline'>"
#define M_COLOR "<div class='syntax_error'>"
#define M_ITALIC "<div class='syntax_italic'>"
#endif

#define M_CASE_SPACE        \
    case ' ':   case'\n':   \
    case '\t':  case '\r':

#define M_CASE_STRING       \
    case '"':   case '#':

#define M_CASE_SYNTAX       \
    case ';':   case ',':   \
    case '(':   case ')':   \
    case '[':   case ']':   \
    case '{':   case '}':

#define M_CASE_NUM                                              \
    case '0':   case '1':   case '2':   case '3':   case '4':   \
    case '5':   case '6':   case '7':   case '8':   case '9':

#define M_CASE_OPERATOR                 \
    case '=':   case '<':   case '>':   \
    case '.':   case ':':   case '$':   \
    case '+':   case '-':   case '*':   \
    case '`':   case '@':               \
    case '~':   case '!':               \
    case '/':   case '|':   case '\\':  \
    case '%':   case '^':   case '&':   \

bool M_Module_Pos_equal(const M_Module_Pos* const self, const M_Module_Pos* const other);

void M_Module_loadfile(M_Module* const self, const M_Array* const path);
void M_Module_loadstr(M_Module* const self, M_Array data, const M_Array* const virtual_path);
void M_Module_clear(M_Module* const self);

void M_Module_print(M_Module* const self);
M_Module_Pos M_Module_getbegin(const M_Module* const self);

M_Str M_Module_Pos_to_str(const M_Module_Pos mpos);
void M_Module_Pos_print(const M_Module_Pos mpos);

char M_Module_Pos_peek(const M_Module_Pos* const pos);
char M_Module_Pos_peekahead(const M_Module_Pos* const pos, size_t n);
char M_Module_Pos_peekbehind(const M_Module_Pos* const pos, size_t n);

char M_Module_Pos_pop(M_Module_Pos* const pos);
bool M_Module_Pos_next(M_Module_Pos* const pos);
bool M_Module_Pos_prev(M_Module_Pos* const pos);
void M_Module_Pos_advance(M_Module_Pos* const pos, const M_Module_Pos* const src);

void M_ModuleList_init(M_ModuleList* const self);
void M_ModuleList_clear(M_ModuleList* const self);