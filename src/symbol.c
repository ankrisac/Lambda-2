#include "symbol.h"

size_t str_hash(const M_Array* const str){
    //DJB2 Hash function
    
    //Iterate primes
    //1 -> 2 -> 3 -> 5 -> 11 -> 31 -> 127 -> 709 -> 5381
    
    size_t hash_val = 5381;

    for(size_t i = 0; i < str->len; i++){
        hash_val = ((hash_val << 5) + hash_val) + (size_t)str->data_char[i];
    }

    return hash_val;
}

void M_SymbolTable_init(M_SymbolTable* const self){
    self->len = 2;
    self->elems = 0;
    self->keys = M_calloc(self->len, sizeof* self->keys);
}
void M_SymbolTable_clear(M_SymbolTable* const self){
    for(size_t i = 0; i < self->len; i++){
        if(self->keys[i] != NULL){
            M_Array_clear(&self->keys[i]->data);
            free(self->keys[i]);
        }
    }
    free(self->keys);
}
void M_SymbolTable_print(const M_SymbolTable* const self){
    printf("[%zu/%zu]{", self->elems, self->len);
    if(self->elems > 0){
        size_t i = 0;

        for(size_t j = 0; i < self->len && j + 1 < self->elems; i++){
            if(self->keys[i] != NULL){
                M_Symbol_print(self->keys[i]);
                printf(", ");
                j++;
            }
        }
        for(; i < self->len; i++){
            if(self->keys[i] != NULL){
                M_Symbol_print(self->keys[i]);
            }
        }
    }
    printf("}");
}

size_t __M_Symbol_GTable_index_symbol(M_Symbol** const keys, const size_t key_len, const M_Symbol* const symbol){
    size_t j = symbol->hash;
    size_t perturb = j;
    size_t index = j % key_len;
            
    while(keys[index] != NULL && !M_Array_equal(&keys[index]->data, &symbol->data)){
        j = ((j << 2) + j) + 1 + perturb;
        perturb >>= 5;
        index = j % key_len;
    }
    return index;
}
void M_Symbol_GTable_resize(M_SymbolTable* const self, size_t len){
    M_Symbol** new_keys = M_calloc(len, sizeof* new_keys);

    for(size_t i = 0; i < self->len; i++){
        new_keys[__M_Symbol_GTable_index_symbol(new_keys, len, self->keys[i])] = self->keys[i];
    }
    free(self->keys);
    self->keys = new_keys;
    self->len = len;
}

const M_Symbol* M_Symbol_new(M_SymbolTable* const self, const M_Array* const str){
    if(str->type != M_TYPE_CHAR){
        M_PANIC_TYPE(str->type, "in Symbol_new");
    }

    if(3 * self->elems >= 2 * self->len){
        M_Symbol_GTable_resize(self, self->len * 4);
    }

    M_Symbol symbol = { .data = *str, .hash = str_hash(str)};
    size_t index = __M_Symbol_GTable_index_symbol(self->keys, self->len, &symbol);

    if(self->keys[index] == NULL){
        self->keys[index] = M_malloc(1, sizeof* self->keys[index]);
        M_Array_copy(&self->keys[index]->data, &symbol.data);
        self->keys[index]->hash = symbol.hash;
        self->elems++;
    }

    return self->keys[index];
}
const M_Symbol* M_Symbol_from_cstr(M_SymbolTable* const self, const char* const str){
    M_Str str_arr;
    M_Str_from_cstr(&str_arr, str);
    
    const M_Symbol* sym = M_Symbol_new(self, &str_arr);
    M_Array_clear(&str_arr);

    return sym;
}