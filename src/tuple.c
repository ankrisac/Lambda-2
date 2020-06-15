#include "tuple.h"

void __M_Tuple_alloc(M_Tuple* const self, const size_t len){
    self->reserve_len = (len > 0) ? len : 1;
    self->data = M_malloc(self->reserve_len, sizeof* self->data);
}
void __M_Tuple_realloc(M_Tuple* const self, const size_t len){
    self->reserve_len = (len > 0) ? len : 1;
    self->data = M_realloc(self->data, len, sizeof* self->data);
}
void __M_Tuple_delete(M_Tuple* const self){
    for(size_t i = 0; i < self->len; i++){
        M_Object_clear(&self->data[i]);
    }
    free(self->data);
}
void __M_Tuple_grow(M_Tuple* const self){
    if(self->reserve_len <= self->len){
        __M_Tuple_realloc(self, self->reserve_len * 2);  
    }
}

void M_Tuple_init(M_Tuple* const self, const size_t len){
    self->len = 0;
    __M_Tuple_alloc(self, len);
}
M_Tuple M_Tuple_new(const size_t len){
    M_Tuple self;
    M_Tuple_init(&self, len);
    return self;
}

void M_Tuple_clear(M_Tuple* const self){
    __M_Tuple_delete(self);
}
void M_Tuple_delete(M_Tuple* const self){
    M_Tuple_clear(self);
    free(self);
}

void M_Tuple_copy(M_Tuple* const self, const M_Tuple* const src){
    self->len = src->len;
    __M_Tuple_alloc(self, self->len);

    for(size_t i = 0; i < self->len; i++){
        M_Object_copy(&self->data[i], &src->data[i]);
    }
}

void M_Tuple_print(const M_Tuple* const self){
    printf("[");
    if(self->len > 0){
        for(size_t i = 0; i < self->len - 1; i++){
            M_Object_repr(&self->data[i]);
            printf(", ");
        }
        M_Object_print(&self->data[self->len - 1]);
    }
    printf("]");
}
void M_Tuple_repr(const M_Tuple* const self){
    M_Tuple_print(self);    
}

M_Status M_Tuple_get(const M_Tuple* const self, const  M_Int i, M_Object* const obj){
    if(i >= self->len || i < 0){
        return M_STATUS_OUT_OF_RANGE;
    }
    M_Object_share(obj, &self->data[i]);
    return M_STATUS_OK;
}
M_Status M_Tuple_set(M_Tuple* const self, const M_Int i, const M_Object* const obj){
    if(i >= self->len || i < 0){
        return M_STATUS_OUT_OF_RANGE;
    }
    M_Object_copy(&self->data[i], obj);
    return M_STATUS_OK;
}

M_Status M_Tuple_push(M_Tuple* const self, const M_Object* const obj){
    switch(obj->type){
        case M_TYPE_NONE:
        case M_TYPE_KEYWORD:
        case M_TYPE_TYPE:
        case M_TYPE_SYMBOL:

        case M_TYPE_BOOL:
        case M_TYPE_CHAR:
        case M_TYPE_INT:
        case M_TYPE_FLOAT:

        case M_TYPE_ARRAY:
        case M_TYPE_TUPLE:{
            M_Object temp;
            __M_Tuple_grow(self);
            
            M_Object_copy(&temp, obj);
            self->data[self->len++] = temp;

            return M_STATUS_OK;
        }

        default:
            return M_STATUS_TYPE_MISMATCH;
    }
}
M_Status M_Tuple_pop(M_Tuple* const self, M_Object* const obj){
    if(self->len == 0){
        M_Object_set_none(obj);
        return M_STATUS_OUT_OF_RANGE;
    }

    self->len--;
    M_Object_share(obj, &self->data[self->len]);    
    return M_STATUS_OK;
}
M_Status M_Tuple_pop_copy(M_Tuple* const self, M_Object* const obj){
    if(self->len == 0){
        M_Object_set_none(obj);
        return M_STATUS_OUT_OF_RANGE;
    }

    self->len--;
    M_Object_copy(obj, &self->data[self->len]);    
    return M_STATUS_OK;
}

void M_Tuple_join(M_Tuple* const self, const M_Tuple* const other){
    size_t len = self->len + other->len;
    if(len > self->reserve_len){
        __M_Tuple_realloc(self, len);
    }

    for(size_t i = 0; i < other->len; i++){
        M_Object_copy(&self->data[self->len + i], &other->data[i]);
    }
}
M_Status M_Tuple_splice(M_Tuple* const self, const M_Tuple* const other, const size_t begin, const size_t end){
    if(begin > end || end >= other->len){ 
        return M_STATUS_OUT_OF_RANGE; 
    }    

    size_t len = end - begin;
    M_Tuple_init(self, len);
 
    for(size_t i = 0; i < len; i++){
        M_Object_copy(&self->data[i], &other->data[begin + i]);
    }    
    return M_STATUS_OK;
}
void M_Tuple_reverse(M_Tuple* const self){
    size_t N = self->len - 1;
    size_t n = self->len / 2;

    M_Object temp;
    for(size_t i = 0; i < n; i++){
        temp = self->data[i];
        self->data[i] = self->data[N - i];
        self->data[N - i] = temp;
    }
}

bool M_Tuple_equal(const M_Tuple* const self, const M_Tuple* const other){
    if(self == other){
        return true;
    }
    
    if(self->len != other->len){ 
        return false; 
    }
    
    for(size_t i = 0; i < self->len; i++){
        if(!M_Object_equal(&self->data[i], &other->data[i])){
            return false;
        }
    }
    return true;
}