#include "str.h"

void M_Str_init(M_Str* const self, size_t len){
    M_Array_init(self, M_TYPE_CHAR, len);
}
void M_Str_clear(M_Str* const self){
    M_Array_clear(self);
}

void M_Str_push(M_Str* const self, const char val){
    __M_Array_push_Char(self, val);
}

void M_Str_from_cstr(M_Array* const self, const char* const str){
    size_t len = strlen(str);
    M_Str_init(self, len);    
    memcpy(self->data_char, str, len); 
    self->len = len;
}
void M_Str_from_int(M_Str* const self, const M_Int val){
    M_Array_init(self, M_TYPE_CHAR, 20);

    if(val == 0){
        M_Str_push(self, '0');
        return;
    }
    
    M_Int temp = (val >= 0) ? val : -val;
    while(temp > 0){
        char digit = (char)(temp % 10) + '0';

        M_Str_push(self, digit);
        temp /= 10;
    }

    if(val <= 0){
        M_Str_push(self, '-');
    }

    M_Array_reverse(self);
}
void M_Str_join(M_Str* const self, const M_Str* const other){
    M_Array_join(self, other);
}
void M_Str_join_cstr(M_Str* const self, const char* const str){
    M_Str temp;
    M_Str_from_cstr(&temp, str);
    M_Str_join(self, &temp);
    M_Str_clear(&temp);
}
void M_Str_join_int(M_Str* const self, const M_Int val){
    M_Str temp;
    M_Str_from_int(&temp, val);
    M_Str_join(self, &temp);
    M_Str_clear(&temp);
}