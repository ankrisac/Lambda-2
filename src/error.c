#include "error.h"

void M_ErrorStack_init(M_ErrorStack* const self){
    M_Array_init(&self->error_stack, M_TYPE_ARRAY, 4);
}
void M_ErrorStack_clear(M_ErrorStack* const self){
    M_Array_clear(&self->error_stack);
}
void M_ErrorStack_delete(M_ErrorStack* const self){
    M_ErrorStack_clear(self);
    free(self);
}

void M_ErrorStack_push(M_ErrorStack* const self, M_Str* const str){
    __M_Array_push_Array(&self->error_stack, str);
}

void M_ErrorStack_drop(M_ErrorStack* const self){
    M_Array_drop(&self->error_stack);   
}
void M_ErrorStack_dropn(M_ErrorStack* const self, const size_t n){
    M_Array_dropn(&self->error_stack, n);
}
void M_ErrorStack_revert(M_ErrorStack* const self, const size_t prev_pos){
    if(prev_pos <= self->error_stack.len){
        M_Array_dropn(&self->error_stack, self->error_stack.len - prev_pos);
    }
}

void M_ErrorStack_print(const M_ErrorStack* const self){
    for(size_t i = 0; i < self->error_stack.len; i++){
        printf("[%zu]: ", i);
        M_Array_print(&self->error_stack.data_arc[i]->v_array);   
    }
}
void M_ErrorStack_pushLocMsg(M_ErrorStack* const self, const M_Module_Pos pos, const char* msg){
    M_Str err, loc = M_Module_Pos_to_str(pos);
    M_Str_from_cstr(&err, msg);
    M_Str_join_cstr(&err, " : ");
    M_Str_join(&err, &loc);

    M_ErrorStack_push(self, &err);
    M_Array_clear(&err);
    M_Array_clear(&loc);
}
void M_ErrorStack_pushLocStrMsg(M_ErrorStack* const self, const M_Module_Pos pos, M_Str msg){
    M_Str loc = M_Module_Pos_to_str(pos);
    M_Str_join_cstr(&msg, " : ");
    M_Str_join(&msg, &loc);

    M_ErrorStack_push(self, &msg);
    M_Array_clear(&msg);
    M_Array_clear(&loc);
}