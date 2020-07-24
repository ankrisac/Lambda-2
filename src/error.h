#include "core.h"

void M_ErrorStack_init(M_ErrorStack* const self);
void M_ErrorStack_clear(M_ErrorStack* const self);
void M_ErrorStack_delete(M_ErrorStack* const self);

void M_ErrorStack_push(M_ErrorStack* const self, M_Str* const str);
void M_ErrorStack_drop(M_ErrorStack* const self);
void M_ErrorStack_dropn(M_ErrorStack* const self, const size_t n);
void M_ErrorStack_revert(M_ErrorStack* const self, const size_t prev_pos);

void M_ErrorStack_print(const M_ErrorStack* const self);
void M_ErrorStack_pushLocMsg(M_ErrorStack* const self, const M_Module_Pos pos, const char* msg);
void M_ErrorStack_pushLocStrMsg(M_ErrorStack* const self, const M_Module_Pos pos, M_Str msg);