#include "expr.h"

void __M_Expr_alloc(M_Expr* const self, const size_t len){
    self->reserve_len = (len > 0) ? len : 1;
    self->data = M_malloc(self->reserve_len, sizeof* self->data);
    self->pos = M_malloc(self->reserve_len, sizeof* self->pos);
}
void __M_Expr_realloc(M_Expr* const self, const size_t len){
    self->reserve_len = (len > 0) ? len : 1;
    self->data = M_realloc(self->data, len, sizeof* self->data);
    self->pos = M_realloc(self->pos, len, sizeof* self->pos);
}
void __M_Expr_delete(M_Expr* const self){
    for(size_t i = 0; i < self->len; i++){
        M_Object_clear(&self->data[i]);
    }
    free(self->data);
    free(self->pos);
}
void __M_Expr_grow(M_Expr* const self){
    if(self->reserve_len <= self->len){
        __M_Expr_realloc(self, self->reserve_len * 2);  
    }
}

void M_Expr_init(M_Expr* const self, const size_t len){
    self->len = 0;
    __M_Expr_alloc(self, len);
}
M_Expr M_Expr_new(const size_t len){
    M_Expr self;
    M_Expr_init(&self, len);
    return self;
}

void M_Expr_clear(M_Expr* const self){
    __M_Expr_delete(self);
}
void M_Expr_delete(M_Expr* const self){
    M_Expr_clear(self);
    free(self);
}

void M_Expr_copy(M_Expr* const self, const M_Expr* const src){
    self->len = src->len;
    __M_Expr_alloc(self, self->len);

    for(size_t i = 0; i < self->len; i++){
        M_Object_copy(&self->data[i], &src->data[i]);
        self->pos[i] = src->pos[i];
    }
}

void M_Expr_print(const M_Expr* const self){
    printf("(");
    if(self->len > 0){
        for(size_t i = 0; i < self->len - 1; i++){
            M_Object_repr(&self->data[i]);
            printf(" ");
        }
        M_Object_repr(&self->data[self->len - 1]);
    }
    printf(")");
}
void M_Expr_repr(const M_Expr* const self){
    M_Expr_print(self);    
}

M_Status M_Expr_push(M_Expr* const self, const M_Object* const obj, const M_Module_Pos* const pos){
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
        case M_TYPE_TUPLE:
        case M_TYPE_EXPR: {
            __M_Expr_grow(self);
            self->data[self->len] = *obj;
            self->pos[self->len] = *pos;
            self->len++;

            return M_STATUS_OK;
        }

        default:
            return M_STATUS_TYPE_MISMATCH;
    }
}

M_Status M_Expr_drop(M_Expr* const self, size_t n){
    if(self->len > 0){
        return M_STATUS_OUT_OF_RANGE;
    }

    self->len--;    
    M_Object_clear(&self->data[self->len]);
    return M_STATUS_OK;
}
M_Status M_Expr_dropn(M_Expr* const self, size_t n){
    if(self->len < n){
        return M_STATUS_OUT_OF_RANGE;
    }

    self->len -= n;    
    for(size_t i = 0; i < n; i++){                          
        M_Object_clear(&self->data[self->len + i]);
    }    
    return M_STATUS_OK;
}

void M_Expr_join(M_Expr* const self, const M_Expr* const other){
    size_t len = self->len + other->len;
    if(len > self->reserve_len){
        __M_Expr_realloc(self, len);
    }

    for(size_t i = 0; i < other->len; i++){
        M_Object_copy(&self->data[self->len + i], &other->data[i]);
        self->pos[self->len + i] = other->pos[i];   
    }
}
M_Status M_Expr_splice(M_Expr* const self, const M_Expr* const other, const size_t begin, const size_t end){
    if(begin > end || end >= other->len){ 
        return M_STATUS_OUT_OF_RANGE; 
    }    

    size_t len = end - begin;
    M_Expr_init(self, len);
 
    for(size_t i = 0; i < len; i++){
        M_Object_copy(&self->data[i], &other->data[begin + i]);
        self->pos[self->len + i] = other->pos[i];
    }    
    return M_STATUS_OK;
}
void M_Expr_reverse(M_Expr* const self){
    size_t N = self->len - 1;
    size_t n = self->len / 2;

    M_Object temp;
    M_Module_Pos temp_pos;
    for(size_t i = 0; i < n; i++){
        temp = self->data[i];
        self->data[i] = self->data[N - i];
        self->data[N - i] = temp;
        
        temp_pos = self->pos[i];
        self->pos[i] = self->pos[N - i];
        self->pos[N - i] = temp_pos;
    }
}

bool M_Expr_equal(const M_Expr* const self, const M_Expr* const other){
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
        if(!M_Module_Pos_equal(&self->pos[i], &other->pos[i])){
            return false;
        }
    }
    return true;
}