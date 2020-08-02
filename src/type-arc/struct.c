#include "struct.h"

void M_Struct_init(M_Struct* const self, const size_t len){
    self->len = (len > 0) ? len : 1;
    self->elems = 0;
    self->keys = M_calloc(self->len, sizeof* self->keys);
    self->data = M_malloc(self->len, sizeof* self->data);
}
M_Struct M_Struct_new(const size_t len){
    M_Struct self;
    M_Struct_init(&self, len);
    return self;
}

void M_Struct_clear(M_Struct* const self){
    for(size_t i = 0; i < self->len; i++){
        if(self->keys[i] != NULL){
            M_Object_clear(&self->data[i]);
        }
    }
    free(self->keys);
    free(self->data);
}
void M_Struct_delete(M_Struct* const self){
    M_Struct_clear(self);
    free(self);
}

size_t __M_Struct_index_symbol(const M_Struct* const self, const M_Symbol* const symbol, const size_t mask){
    size_t j = symbol->hash;
    size_t perturb = j;
    size_t index = j % mask;
            
    while(self->keys[index] != NULL && self->keys[index] != symbol){
        j = ((j << 2) + j) + 1 + perturb;
        perturb >>= 5;
        index = j % mask;
    }
    return index;
}
void __M_Struct_grow(M_Struct* const self, const size_t len){
    const M_Symbol** new_keys = M_calloc(len, sizeof* new_keys);
    M_Object* data = M_malloc(len, sizeof* data);

    for(size_t i = 0; i < self->len; i++){
        if(self->keys[i] != NULL){
            size_t index = __M_Struct_index_symbol(self, self->keys[i], self->len);
            new_keys[index] = self->keys[i];
            data[index] = self->data[i];
        }        
    }

    free(self->data);
    free(self->keys);

    self->keys = new_keys;
    self->data = data;
    self->len = len;
}


void M_Struct_copy(M_Struct* const self, const M_Struct* const src){
    if(self == src){
        return;
    }

    M_Struct_init(self, 2 * src->elems);
    for(size_t i = 0; i < self->len; i++){
        if(self->keys[i] != NULL){
            M_Object_copy(&self->data[__M_Struct_index_symbol(self, self->keys[i], self->len)], &self->data[i]);
        }
    }
}

void M_Struct_repr(const M_Struct* const self){
    printf("{");
    if(self->elems > 0){
        printf(" ");
    }

    for(size_t i = 0, j = 0; i < self->len; i++){
        if(self->keys[i] != NULL){
            j++;
            M_Symbol_print(self->keys[i]);
            printf(": ");
            M_Object_repr(&self->data[i]);
            
            if(j < self->elems){
                printf(", ");
            }
            else{
                printf(" ");
            }
        }    
    }
    printf("}");
}
void M_Struct_print(const M_Struct* const self){
    M_Struct_repr(self);
}

M_Status M_Struct_set(M_Struct* const self, const M_Symbol* sym, const M_Object* const obj){
    if(3 * self->elems > 2 * self->len){
        __M_Struct_grow(self, self->len * 4);
    }
    
    size_t index = __M_Struct_index_symbol(self, sym, self->len);

    M_Status status;

    if(self->keys[index] == NULL){
        self->keys[index] = sym;
        status = M_Object_copy(&self->data[index], obj);
        self->elems++;
    }
    else{
        M_Object_clear(&self->data[index]);
        status = M_Object_copy(&self->data[index], obj);
        
        if(status != M_STATUS_OK){
            M_Object_set_none(&self->data[index]);
        }
    }

    return status;
}
M_Status M_Struct_get(const M_Struct* const self, const M_Symbol* sym, M_Object* const obj){
    size_t index = __M_Struct_index_symbol(self, sym, self->len); 

    if(self->keys[index] == NULL){
        return M_STATUS_UNDEFINED_SYMBOL;
    }
    
    M_Object_share(obj, &self->data[index]);
    return M_STATUS_OK;
}

bool M_Struct_in(const M_Struct* const self, const M_Symbol* sym){
    return self->keys[__M_Struct_index_symbol(self, sym, self->len)] != NULL;
}
bool M_Struct_equal(const M_Struct* const self, const M_Struct* other){
    if(self == other){
        return true;
    }
    if(self->elems != other->elems){
        return false;
    }

    for(size_t i = 0; i < self->len; i++){
        if(!M_Object_equal(&self->data[i], &other->data[i])){
            return false;
        }
    }
    return true;
}
