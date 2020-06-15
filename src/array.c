#include "array.h"

void __M_Array_alloc(M_Array* const self, const size_t len){
    self->reserve_len = (len > 0) ? len : 1;
    
    #define ALLOC(TAG)\
        self->TAG = M_malloc(self->reserve_len, sizeof* self->TAG);
    
    switch(self->type){
        case M_TYPE_TYPE:       ALLOC(data_type)        break;
        case M_TYPE_KEYWORD:    ALLOC(data_keyword)     break;
        case M_TYPE_SYMBOL:     ALLOC(data_symbol)      break;
        
        case M_TYPE_BOOL:       ALLOC(data_bool)        break;
        case M_TYPE_CHAR:       ALLOC(data_type)        break;
        case M_TYPE_INT:        ALLOC(data_int)         break;
        case M_TYPE_FLOAT:      ALLOC(data_float)       break;
        
        case M_TYPE_ARRAY:      
        case M_TYPE_TUPLE:
            ALLOC(data_arc)
            break;

        default:
            M_panic_type(self->type, "in Array_alloc");
            break;
    }
    #undef ALLOC
}
void __M_Array_realloc(M_Array* const self, const size_t len){
    self->reserve_len = (len > 0) ? len : 1;

    #define REALLOC(TAG)\
        self->TAG = M_realloc(self->TAG, self->reserve_len, sizeof* self->TAG);
    
    switch(self->type){
        case M_TYPE_TYPE:       REALLOC(data_type)      break;
        case M_TYPE_KEYWORD:    REALLOC(data_keyword)   break;
        case M_TYPE_SYMBOL:     REALLOC(data_symbol)    break;
    
        case M_TYPE_BOOL:       REALLOC(data_bool)      break;
        case M_TYPE_CHAR:       REALLOC(data_char)      break;
        case M_TYPE_INT:        REALLOC(data_int)       break;
        case M_TYPE_FLOAT:      REALLOC(data_float)     break;
        
        case M_TYPE_ARRAY:      
        case M_TYPE_TUPLE:
            REALLOC(data_arc)
            break;

        default:
            M_panic_type(self->type, "in Array_realloc");
            break;
    }
    #undef REALLOC
}
void __M_Array_delete(M_Array* const self){
    #define ARC_DELETE(DELETER)                     \
        for(size_t i = 0; i < self->len; i++){      \
            M_ARC_Object* arc = self->data_arc[i];  \
            if(arc->ref_count == 1){                \
                DELETER;                            \
                free(arc);                          \
            }                                       \
            else{                                   \
                arc->ref_count--;                   \
            }                                       \
        }                                           \
        free(self->data_arc);

    switch(self->type){
        case M_TYPE_TYPE:       free(self->data_type);      break;
        case M_TYPE_KEYWORD:    free(self->data_keyword);   break;
        case M_TYPE_SYMBOL:     free(self->data_symbol);    break;

        case M_TYPE_BOOL:       free(self->data_bool);      break;
        case M_TYPE_CHAR:       free(self->data_char);      break;
        case M_TYPE_INT:        free(self->data_int);       break;
        case M_TYPE_FLOAT:      free(self->data_float);     break;

        case M_TYPE_ARRAY: { ARC_DELETE(M_Array_clear(&arc->v_array)) break; }
        case M_TYPE_TUPLE: { ARC_DELETE(M_Tuple_clear(&arc->v_tuple)) break; }
        case M_TYPE_STRUCT: { ARC_DELETE(M_Struct_clear(&arc->v_struct)) break; }
        
        default:            
            M_panic_type(self->type, "in Array_delete");
            break;
    }
    #undef ARC_DELETE
}
void __M_Array_grow(M_Array* const self){
    if(self->reserve_len <= self->len){
        __M_Array_realloc(self, self->reserve_len * 2);  
    }
}

void M_Array_init(M_Array* const self, const M_Type type, const size_t len){
    self->len = 0;
    self->type = type;
    __M_Array_alloc(self, len);
}
M_Array M_Array_new(const M_Type type, const size_t len){
    M_Array self;
    M_Array_init(&self, type, len);
    return self;
}

void M_Array_from_cstr(M_Array* const self, const char* const str){
    size_t len = strlen(str);
    M_Array_init(self, M_TYPE_CHAR, len);    
    memcpy(self->data_char, str, len); 
    self->len = len;
}

void M_Array_clear(M_Array* const self){
    __M_Array_delete(self);
}
void M_Array_delete(M_Array* const self){
    M_Array_clear(self);
    free(self);
}

void M_Array_copy(M_Array* const self, const M_Array* const src){
    if(self == src){
        return;
    }
    
    self->type = src->type;
    self->len = src->len;
    __M_Array_alloc(self, self->len);

    #define COPY(TAG)\
        memcpy(self->TAG, src->TAG, self->len * sizeof* src->TAG);

    switch(self->type){
        case M_TYPE_TYPE:       COPY(data_type)     break;
        case M_TYPE_KEYWORD:    COPY(data_keyword)  break;
        case M_TYPE_SYMBOL:     COPY(data_symbol)   break;
        
        case M_TYPE_BOOL:       COPY(data_bool)     break;
        case M_TYPE_CHAR:       COPY(data_char)     break;
        case M_TYPE_INT:        COPY(data_int)      break;
        case M_TYPE_FLOAT:      COPY(data_float)    break;

        case M_TYPE_ARRAY:
            for(size_t i = 0; i < self->len; i++){
                M_Array_copy(&self->data_arc[i]->v_array, &src->data_arc[i]->v_array);
            }
            break;
        case M_TYPE_TUPLE:
            for(size_t i = 0; i < self->len; i++){
                M_Tuple_copy(&self->data_arc[i]->v_tuple, &src->data_arc[i]->v_tuple);
            }
            break;

        default:
            M_panic_type(self->type, "in Array_copy");
            break;
    }
    #undef COPY
}

void M_Array_print(const M_Array* const self){
    switch(self->type){
        case M_TYPE_TYPE:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    M_Type_print(self->data_type[i]);
                    printf(", ");
                }
                M_Type_print(self->data_type[self->len - 1]);
            }
            printf("]");
            break;          
    
        case M_TYPE_KEYWORD:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    M_Keyword_print(self->data_keyword[i]);
                    printf(", ");
                }
                M_Keyword_print(self->data_type[self->len - 1]);
            }
            printf("]");
            break;
        case M_TYPE_SYMBOL:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    M_Symbol_print(self->data_symbol[i]);
                    printf(", ");
                }
                M_Symbol_print(self->data_symbol[self->len - 1]);
            }
            printf("]");
            break;
            
        case M_TYPE_BOOL:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    printf(self->data_bool[i] ? "True" : "False");
                    printf(", ");
                }
                printf(self->data_bool[self->len - 1] ? "True" : "False");
            }
            printf("]");
            break;
        case M_TYPE_CHAR:
            printf("%.*s", (int)self->len, self->data_char);
            break;
        case M_TYPE_INT:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    printf("%lli, ", self->data_int[i]);
                }
                printf("%lli", self->data_int[self->len - 1]);
            }
            printf("]");
            break;
        case M_TYPE_FLOAT:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    printf("%f, ", self->data_float[i]);
                }
                printf("%f", self->data_float[self->len - 1]);
            }
            printf("]");
            break;

        case M_TYPE_ARRAY:
            for(size_t i = 0; i < self->len - 1; i++){
                M_Array_repr(&self->data_arc[i]->v_array);
                printf(", ");
            }
            M_Array_repr(&self->data_arc[self->len - 1]->v_array);
            break;
        case M_TYPE_TUPLE:
            for(size_t i = 0; i < self->len - 1; i++){
                M_Tuple_repr(&self->data_arc[i]->v_tuple);
                printf(", ");
            }
            M_Tuple_repr(&self->data_arc[self->len - 1]->v_tuple);
            break;
        
        default:
            M_panic_type(self->type, "in Array_print");
            break;
    }
}
void M_Array_repr(const M_Array* const self){
    switch(self->type){
        case M_TYPE_TYPE:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    M_Type_print(self->data_type[i]);
                    printf(", ");
                }
                M_Type_print(self->data_type[self->len - 1]);
            }
            printf("]");
            break;          
        case M_TYPE_KEYWORD:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    M_Keyword_print(self->data_keyword[i]);
                    printf(", ");
                }
                M_Type_print(self->data_type[self->len - 1]);
            }
            printf("]");
            break;
        
        case M_TYPE_BOOL:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    printf(self->data_bool[i] ? "True" : "False");
                    printf(", ");
                }
                printf(self->data_bool[self->len - 1] ? "True" : "False");
            }
            printf("]");
            break;
        case M_TYPE_CHAR:{
            printf("\"");
            for(size_t i = 0; i < self->len; i++){
                M_Char_repr(self->data_char[i]);
            }
            printf("\"");
            break;   
        }
        case M_TYPE_INT:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    printf("%lli, ", self->data_int[i]);
                }
                printf("%lli", self->data_int[self->len - 1]);
            }
            printf("]");
            break;
        case M_TYPE_FLOAT:
            printf("[");
            if(self->len > 0){     
                for(size_t i = 0; i < self->len - 1; i++){
                    printf("%f, ", self->data_float[i]);
                }
                printf("%f", self->data_float[self->len - 1]);
            }
            printf("]");
            break;

        case M_TYPE_ARRAY:
            for(size_t i = 0; i < self->len - 1; i++){
                M_Array_repr(&self->data_arc[i]->v_array);
                printf(", ");
            }
            M_Array_repr(&self->data_arc[self->len - 1]->v_array);
            break;
        case M_TYPE_TUPLE:
            for(size_t i = 0; i < self->len - 1; i++){
                M_Tuple_repr(&self->data_arc[i]->v_tuple);
                printf(", ");
            }
            M_Tuple_repr(&self->data_arc[self->len - 1]->v_tuple);
            break;
        
        default:
            M_panic_type(self->type, "in Array_print");
            break;
    }
}
void M_Array_desc(const M_Array* const self){
    M_Type_print(self->type);
    printf("[%zu/%zu]:", self->len, self->reserve_len);
    M_Array_print(self);
}

void __M_Array_push_Type(M_Array* const self, const M_Type val){
    __M_Array_grow(self);
    self->data_type[self->len++] = val;
}
void __M_Array_push_Keyword(M_Array* const self, const M_Keyword val){
    __M_Array_grow(self);
    self->data_keyword[self->len++] = val;
}
void __M_Array_push_Symbol(M_Array* const self, const M_Symbol* const val){
    __M_Array_grow(self);
    self->data_symbol[self->len++] = val;
}

void __M_Array_push_Bool(M_Array* const self, const M_Bool val){
    __M_Array_grow(self);
    self->data_bool[self->len++] = val;
}
void __M_Array_push_Char(M_Array* const self, const M_Char val){
    __M_Array_grow(self);
    self->data_char[self->len++] = val;
}
void __M_Array_push_Int(M_Array* const self, const M_Int val){
    __M_Array_grow(self);
    self->data_int[self->len++] = val;
}
void __M_Array_push_Float(M_Array* const self, const M_Float val){
    __M_Array_grow(self);
    self->data_float[self->len++] = val;
}

void __M_Array_push_Array(M_Array* const self, const M_Array* const val){
    __M_Array_grow(self);
    self->data_arc[self->len] = M_ARC_Object_new();
    M_Array_copy(&self->data_arc[self->len++]->v_array, val);
}
void __M_Array_push_Tuple(M_Array* const self, const M_Tuple* const val){
    __M_Array_grow(self);
    self->data_arc[self->len] = M_ARC_Object_new();
    M_Tuple_copy(&self->data_arc[self->len++]->v_tuple, val);
}

M_Status M_Array_get(M_Array* const self, const M_Int i, M_Object* const obj){
    if(i >= self->len || i < 0){
        return M_STATUS_OUT_OF_RANGE;
    }

    obj->type = self->type;
    switch(self->type){
        case M_TYPE_TYPE:       obj->v_type = self->data_type[i];       break;
        case M_TYPE_KEYWORD:    obj->v_keyword = self->data_type[i];    break;
        case M_TYPE_SYMBOL:     obj->v_symbol = self->data_symbol[i];   break;

        case M_TYPE_BOOL:       obj->v_bool = self->data_bool[i];       break;
        case M_TYPE_CHAR:       obj->v_char = self->data_char[i];       break;
        case M_TYPE_INT:        obj->v_int = self->data_int[i];         break;
        case M_TYPE_FLOAT:      obj->v_float = self->data_float[i];     break;

        case M_TYPE_TUPLE:
        case M_TYPE_ARRAY:
            obj->arc = self->data_arc[i];
            obj->arc->ref_count++;

        default:
            M_panic_type(self->type, "in Array_arrget");
            return M_STATUS_COMPILER_ERROR;
    }
    return M_STATUS_OK;
}
M_Status M_Array_set(M_Array* const self, const M_Int i, const M_Object* const obj){
    if(i >= self->len || i < 0){
        return M_STATUS_OUT_OF_RANGE;
    }
    if(self->type != obj->type){
        return M_STATUS_TYPE_MISMATCH;
    }

    switch(self->type){
        case M_TYPE_TYPE:       self->data_type[i] =    obj->v_type;    break;
        case M_TYPE_KEYWORD:    self->data_type[i] =    obj->v_keyword; break;
        case M_TYPE_SYMBOL:     self->data_symbol[i] =  obj->v_symbol;  break;

        case M_TYPE_BOOL:       self->data_bool[i] =    obj->v_bool;    break;
        case M_TYPE_CHAR:       self->data_char[i] =    obj->v_char;    break;
        case M_TYPE_INT:        self->data_int[i] =     obj->v_int;     break;
        case M_TYPE_FLOAT:      self->data_float[i] =   obj->v_float;   break;

        case M_TYPE_ARRAY:
            self->data_arc[i] = M_ARC_Object_new();
            M_Array_copy(&self->data_arc[i]->v_array, &obj->arc->v_array);
            break;
        case M_TYPE_TUPLE:
            self->data_arc[i] = M_ARC_Object_new();
            M_Tuple_copy(&self->data_arc[i]->v_tuple, &obj->arc->v_tuple);
            break;

        default:
            M_panic_type(self->type, "in Array_arrget");
            return M_STATUS_COMPILER_ERROR;
    }
    return M_STATUS_OK;
}

M_Status M_Array_push(M_Array* const self, const M_Object* const obj){
    if(self->type != obj->type){
        return M_STATUS_TYPE_MISMATCH;
    }
        
    switch(self->type){
        case M_TYPE_TYPE:       __M_Array_push_Type(self, obj->v_type);         break;
        case M_TYPE_KEYWORD:    __M_Array_push_Keyword(self, obj->v_keyword);   break;
        case M_TYPE_SYMBOL:     __M_Array_push_Symbol(self, obj->v_symbol);     break;

        case M_TYPE_BOOL:       __M_Array_push_Bool(self, obj->v_bool);         break;
        case M_TYPE_CHAR:       __M_Array_push_Char(self, obj->v_char);         break;
        case M_TYPE_INT:        __M_Array_push_Int(self, obj->v_int);           break;
        case M_TYPE_FLOAT:      __M_Array_push_Float(self, obj->v_float);       break;
        
        case M_TYPE_ARRAY:      __M_Array_push_Array(self, &obj->arc->v_array); break;
        case M_TYPE_TUPLE:      __M_Array_push_Tuple(self, &obj->arc->v_tuple); break;
        
        default:
            return M_STATUS_TYPE_MISMATCH;
    }
    return M_STATUS_OK;
}
M_Status M_Array_pop(M_Array* const self, M_Object* const obj){
    if(self->len == 0){
        obj->type = M_TYPE_NONE;
        return M_STATUS_OUT_OF_RANGE;
    }
    
    obj->type = self->type;
    self->len--;
    switch(self->type){
        case M_TYPE_TYPE:       obj->v_type =       self->data_type[self->len];     break;
        case M_TYPE_KEYWORD:    obj->v_keyword =    self->data_keyword[self->len];  break;
        case M_TYPE_SYMBOL:     obj->v_symbol =     self->data_symbol[self->len];   break;

        case M_TYPE_BOOL:       obj->v_bool =       self->data_bool[self->len];     break;
        case M_TYPE_CHAR:       obj->v_char =       self->data_char[self->len];     break;
        case M_TYPE_INT:        obj->v_int =        self->data_int[self->len];      break;
        case M_TYPE_FLOAT:      obj->v_float =      self->data_float[self->len];    break;
        
        case M_TYPE_TUPLE:      
        case M_TYPE_ARRAY:      
            obj->arc = self->data_arc[self->len];
            obj->arc->ref_count++;
            break;

        default:
            M_panic_type(self->type, "in Array_pop");
            return M_STATUS_COMPILER_ERROR;
    }   

    return M_STATUS_OK;
}
M_Status M_Array_drop(M_Array* const self){
    if(self->len == 0){
        return M_STATUS_OUT_OF_RANGE;
    }
    
    #define ARC_DELETE(DELETER)                         \
        M_ARC_Object* arc = self->data_arc[self->len];  \
        if(arc->ref_count == 1){                        \
            DELETER;                                    \
            free(arc);                                  \
        }                                               \
        else{                                           \
            arc->ref_count--;                           \
        }

    self->len--;
    switch(self->type){
        case M_TYPE_TYPE:     
        case M_TYPE_KEYWORD:    
        case M_TYPE_SYMBOL:     

        case M_TYPE_BOOL:
        case M_TYPE_CHAR: 
        case M_TYPE_INT: 
        case M_TYPE_FLOAT:
            break;

        case M_TYPE_ARRAY: { ARC_DELETE(M_Array_clear(&arc->v_array)) break; }
        case M_TYPE_TUPLE: { ARC_DELETE(M_Tuple_clear(&arc->v_tuple)) break; }
        case M_TYPE_STRUCT: { ARC_DELETE(M_Struct_clear(&arc->v_struct)) break; }
        
        default:
            M_panic_type(self->type, "in Array_pop");
            return M_STATUS_COMPILER_ERROR;
    }   
    #undef ARC_DELETE

    return M_STATUS_OK;
}
M_Status M_Array_pop_copy(M_Array* const self, M_Object* const obj){
    if(self->len == 0){
        obj->type = M_TYPE_NONE;
        return M_STATUS_OUT_OF_RANGE;
    }

    self->len--;
    switch(self->type){
        case M_TYPE_TYPE:       obj->v_type =       self->data_type[self->len];     break;
        case M_TYPE_KEYWORD:    obj->v_keyword =    self->data_keyword[self->len];  break;
        case M_TYPE_SYMBOL:     obj->v_symbol =     self->data_symbol[self->len];   break;

        case M_TYPE_BOOL:       obj->v_bool =       self->data_bool[self->len];     break;
        case M_TYPE_CHAR:       obj->v_char =       self->data_char[self->len];     break;
        case M_TYPE_INT:        obj->v_int =        self->data_int[self->len];      break;
        case M_TYPE_FLOAT:      obj->v_float =      self->data_float[self->len];    break;
        
        case M_TYPE_ARRAY:      
            obj->arc = M_ARC_Object_new();
            M_Array_copy(&obj->arc->v_array, &self->data_arc[self->len]->v_array);
            break;
        case M_TYPE_TUPLE:      
            obj->arc = M_ARC_Object_new();
            M_Tuple_copy(&obj->arc->v_tuple, &self->data_arc[self->len]->v_tuple);
            break;

        default:
            M_panic_type(self->type, "in Array_pop");
            return M_STATUS_COMPILER_ERROR;
    }   
    return M_STATUS_OK;
}

M_Status M_Array_join(M_Array* const self, const M_Array* const other){
    if(self->type != other->type){
        return M_STATUS_TYPE_MISMATCH;
    }

    size_t new_len = self->len + other->len;
    if(self->reserve_len < new_len) {
        __M_Array_realloc(self, new_len);
    }

    #define COPY(TAG)\
        memcpy(&self->TAG[self->len], other->TAG, other->len * sizeof* self->TAG);

    switch(self->type){
        case M_TYPE_TYPE:       COPY(data_type) break;
        case M_TYPE_KEYWORD:    COPY(data_keyword) break;
        case M_TYPE_SYMBOL:     COPY(data_symbol) break;

        case M_TYPE_BOOL:   COPY(data_bool) break;
        case M_TYPE_CHAR:   COPY(data_char) break;
        case M_TYPE_INT:    COPY(data_int) break;
        case M_TYPE_FLOAT:  COPY(data_float) break;

        case M_TYPE_ARRAY:      
            for(size_t i = 0; i < other->len; i++){
                self->data_arc[self->len + i] = M_ARC_Object_new();
                M_Array_copy(&self->data_arc[self->len + i]->v_array, &other->data_arc[i]->v_array);
            }
            break;
        case M_TYPE_TUPLE:      
            for(size_t i = 0; i < other->len; i++){
                self->data_arc[self->len + i] = M_ARC_Object_new(); 
                M_Tuple_copy(&self->data_arc[self->len + i]->v_tuple, &other->data_arc[i]->v_tuple);
            }
            break;
        
        default:
            M_panic_type(self->type, "in Array_concat");
            return M_STATUS_COMPILER_ERROR;
    }

    self->len = new_len;
    return M_STATUS_OK;
}
M_Status M_Array_splice(M_Array* const self, const M_Array* const other, const M_Int begin, const M_Int end){
    if(begin > end || begin < 0 || end >= other->len){
        return M_STATUS_OUT_OF_RANGE;
    }
    size_t len = end - begin;
    
    self->type = other->type;
    self->len = len;

    #define SPLICE_BASE(TAG, EXPR)                                          \
        if(len == 0){                                                       \
            self->reserve_len = 1;                                          \
            self->TAG = M_malloc(1, sizeof* self->TAG);                     \
        }                                                                   \
        else{                                                               \
            self->reserve_len = len;                                        \
            self->TAG = M_malloc(self->reserve_len, sizeof* self->TAG);     \
            EXPR                                                            \
        }

    #define SPLICE(TAG)\
        SPLICE_BASE(TAG, memcpy(self->TAG, &other->TAG[begin], len * sizeof* self->TAG); )

    switch(self->type){
        case M_TYPE_TYPE:       SPLICE(data_type)       break;
        case M_TYPE_KEYWORD:    SPLICE(data_keyword)    break;
        case M_TYPE_SYMBOL:     SPLICE(data_symbol)     break;

        case M_TYPE_BOOL:   SPLICE(data_bool)   break;
        case M_TYPE_CHAR:   SPLICE(data_char)   break;
        case M_TYPE_INT:    SPLICE(data_int)    break;
        case M_TYPE_FLOAT:  SPLICE(data_float)  break;

        case M_TYPE_ARRAY:
            SPLICE_BASE(data_arc,
                for(size_t i = 0; i < len; i++){
                    self->data_arc[i] = M_ARC_Object_new();
                    M_Array_copy(&self->data_arc[i]->v_array, &other->data_arc[i]->v_array);
                }
            )
            break;
        case M_TYPE_TUPLE:
            SPLICE_BASE(data_arc,
                for(size_t i = 0; i < len; i++){
                    self->data_arc[i] = M_ARC_Object_new();
                    M_Tuple_copy(&self->data_arc[i]->v_tuple, &other->data_arc[i]->v_tuple);
                }
            )
            break;

        default:
            M_panic_type(self->type, "in Array_splice");
            break;
    }
    #undef SPLICE

    return M_STATUS_OK;
}
short M_Array_compare(const M_Array* const self, const M_Array* const other){
    if(self == other){
        return 0;
    }
    
    if(self->type != other->type){
        return 0;
    }

    size_t len = (self->len > other->len) ? other->len : self->len;
    long long int diff = 0;

    #define COMPARE(TAG)\
        diff = memcmp(&self->TAG[0], &other->TAG[0], len);

    switch(self->type){
        case M_TYPE_TYPE:       COMPARE(data_type) break;
        case M_TYPE_KEYWORD:    COMPARE(data_keyword) break;

        case M_TYPE_BOOL:   COMPARE(data_bool) break;
        case M_TYPE_CHAR:   COMPARE(data_char) break;
        case M_TYPE_INT:    COMPARE(data_int) break;
        case M_TYPE_FLOAT:  COMPARE(data_float) break;

        case M_TYPE_ARRAY:
            for(size_t i = 0; i < len && diff == 0; i++){
                diff = M_Array_compare(&self->data_arc[i]->v_array, &other->data_arc[i]->v_array);
            }
            break;
        
        default:
            M_panic_type(self->type, "in Array_compare");
            break;
    }

    #undef COMPARE

    if(diff == 0){
        if(self->len < other->len){
            return -1;
        }
        else{
            return (self->len == other->len) ? 0 : 1;
        }
    }
    return (diff > 0) ? 1 : -1;
}
bool M_Array_equal(const M_Array* const self, const M_Array* const other){
    if(self == other){
        return true;
    }
    if(self->type != other->type || self->len != other->len){
        return false;
    }

    #define COMPARE(TAG)\
        0 == memcmp(&self->TAG[0], &other->TAG[0], self->len);

    switch(self->type){
        case M_TYPE_TYPE:       return COMPARE(data_type)
        case M_TYPE_KEYWORD:    return COMPARE(data_keyword)
        case M_TYPE_SYMBOL:    return COMPARE(data_symbol)

        case M_TYPE_BOOL:   return COMPARE(data_bool)
        case M_TYPE_CHAR:   return COMPARE(data_char)
        case M_TYPE_INT:    return COMPARE(data_int)
        case M_TYPE_FLOAT:  return COMPARE(data_float)

        case M_TYPE_ARRAY:
            for(size_t i = 0; i < self->len; i++){
                if(!M_Array_equal(&self->data_arc[i]->v_array, &other->data_arc[i]->v_array)){
                    return false;
                }
            }
            return true;
        case M_TYPE_TUPLE:
            for(size_t i = 0; i < self->len; i++){
                if(!M_Tuple_equal(&self->data_arc[i]->v_tuple, &other->data_arc[i]->v_tuple)){
                    return false;
                }
            }
            return true;
        default:
            M_panic_type(self->type, "in Array_equal");
            return false;
    }

    #undef COMPARE
}

void M_Array_reverse(M_Array* const self){
    size_t N = self->len - 1;
    size_t n = self->len / 2;

    #define REVERSE(TYPE, TAG)                  \
        TYPE temp;                              \
        for(size_t i = 0; i < n; i++){          \
            temp = self->TAG[i];                \
            self->TAG[i] = self->TAG[N - i];    \
            self->TAG[N - i] = temp;            \
        }
        
    switch(self->type){
        case M_TYPE_TYPE:       { REVERSE(M_Type, data_type) break; }
        case M_TYPE_KEYWORD:    { REVERSE(M_Keyword, data_keyword) break; }
        case M_TYPE_SYMBOL:    { REVERSE(const M_Symbol*, data_symbol) break; }

        case M_TYPE_CHAR:   { REVERSE(M_Char, data_char) break; }
        case M_TYPE_BOOL:   { REVERSE(M_Bool, data_bool) break; }
        case M_TYPE_INT:    { REVERSE(M_Int, data_int) break; }
        case M_TYPE_FLOAT:  { REVERSE(M_Float, data_float) break; }

        case M_TYPE_ARRAY:  
        case M_TYPE_TUPLE: {
            REVERSE(M_ARC_Object*, data_arc) 
            break;    
        }

        default:
            M_panic_type(self->type, "in Array_reverse");
            break;
    }

    #undef REVERSE
}