#include "stack.h"

void M_Stack_init(M_Stack* const self){
    self->len = 100;
    self->data = M_malloc(self->len, sizeof* self->data);
    self->stack_ptr = 0;
}
void M_Stack_clear(M_Stack* const self){
    for(size_t i = 0; i < self->stack_ptr; i++){
        M_Object_clear(&self->data[i]);
    }
    free(self->data);
}
void __M_Stack_grow(M_Stack* const self){
    if(self->stack_ptr >= self->len){
        self->len *= 4;
        self->data = M_realloc(self->data, self->len, sizeof* self->data);
    }
}


void M_Stack_push_error(M_Stack* const self, const M_Status status){
    __M_Stack_grow(self);
    M_Object_set_error(&self->data[self->stack_ptr++], status);
}
void M_Stack_push_none(M_Stack* const self){
    __M_Stack_grow(self);
    M_Object_set_none(&self->data[self->stack_ptr++]);
}

void M_Stack_push_type(M_Stack* const self, const M_Type val){
    __M_Stack_grow(self);
    M_Object_set_type(&self->data[self->stack_ptr++], val);
}
void M_Stack_push_keyword(M_Stack* const self, const M_Keyword val){
    __M_Stack_grow(self);
    M_Object_set_keyword(&self->data[self->stack_ptr++], val);
}
void M_Stack_push_symbol(M_Stack* const self, const M_Symbol* const val){
    __M_Stack_grow(self);
    M_Object_set_symbol(&self->data[self->stack_ptr++], val);
}


void M_Stack_push_bool(M_Stack* const self, const M_Bool val){
    __M_Stack_grow(self);
    M_Object_set_bool(&self->data[self->stack_ptr++], val);
}
void M_Stack_push_char(M_Stack* const self, const M_Char val){
    __M_Stack_grow(self);
    M_Object_set_char(&self->data[self->stack_ptr++], val);
}
void M_Stack_push_int(M_Stack* const self, const M_Int val){
    __M_Stack_grow(self);
    M_Object_set_int(&self->data[self->stack_ptr++], val);
}
void M_Stack_push_float(M_Stack* const self, const M_Float val){
    __M_Stack_grow(self);
    M_Object_set_int(&self->data[self->stack_ptr++], val);
}

void M_Stack_push_array(M_Stack* const self, const M_Array val){
    __M_Stack_grow(self);
    M_Object_set_array(&self->data[self->stack_ptr++], val);
}
void M_Stack_push_tuple(M_Stack* const self, const M_Tuple val){
    __M_Stack_grow(self);
    M_Object_set_tuple(&self->data[self->stack_ptr++], val);
}
void M_Stack_push_struct(M_Stack* const self, const M_Struct val){
    __M_Stack_grow(self);
    M_Object_set_struct(&self->data[self->stack_ptr++], val);
}

void M_Stack_push_cstr(M_Stack* const self, const char* const str){
    M_Array str_arr;
    M_Array_from_cstr(&str_arr, str);
    M_Stack_push_array(self, str_arr);
}

void M_Stack_push(M_Stack* const self, M_Object* const obj){
    __M_Stack_grow(self);
    self->data[self->stack_ptr++] = *obj;
}
void M_Stack_push_share(M_Stack* const self, M_Object* const obj){
    __M_Stack_grow(self);
    M_Object_share(&self->data[self->stack_ptr++], obj);
}
void M_Stack_push_copy(M_Stack* const self, const M_Object* const obj){
    __M_Stack_grow(self);
    M_Object_copy(&self->data[self->stack_ptr++], obj);
}
void M_Stack_pop(M_Stack* const self, M_Object* const obj){
    if(self->stack_ptr > 0){
        self->stack_ptr--;
        M_Object_share(obj, &self->data[self->stack_ptr]);
    }
    else{
        M_panic("in Stack_pop");
    }
}
void M_Stack_pop_copy(M_Stack* const self, M_Object* const obj){
    if(self->stack_ptr > 0){
        self->stack_ptr--;
        M_Object_share(obj, &self->data[self->stack_ptr]);
    }
    else{
        M_panic("in Stack_pop_copy");
    }
}
void M_Stack_drop(M_Stack* const self){
    if(self->stack_ptr > 0){
        self->stack_ptr--;
        M_Object_clear(&self->data[self->stack_ptr]);
    }
    else{
        M_panic("in Stack_drop");
    }
}
void M_Stack_dropn(M_Stack* const self, const size_t n){
    if(self->stack_ptr >= n){
        for(size_t i = self->stack_ptr - n; i < self->stack_ptr; i++){
            M_Object_clear(&self->data[i]);
        }
        self->stack_ptr -= n;
    }
    else{
        M_panic("in Stack_dropn");    
    }
}
void M_Stack_print(const M_Stack* const self){
    printf("Stack:\n");

    for(size_t i = 0; i < self->stack_ptr && i < 1000; i++){
        printf("- %3zu| ", i);
        M_Object_repr(&self->data[i]);
        printf("\n");
    }
    if(self->stack_ptr >= 1000){
        printf("...\n");
    }
}

#define RETURN_ERROR(TYPE)          \
    M_Stack_dropn(self, n);         \
    M_Stack_push_error(self, TYPE); \
    return TYPE; 

#define RETURN_ERROR_TYPE                  \
    RETURN_ERROR(M_STATUS_TYPE_MISMATCH)     

#define REDUCE(TYPE, TAG, REDUCE_EXPR)                  \
    TYPE val = self->data[ptr].TAG;                     \
    for(size_t i = ptr + 1; i < self->stack_ptr; i++){  \
        if(type == self->data[ptr].type){               \
            REDUCE_EXPR                                 \
        }                                               \
        else{                                           \
            RETURN_ERROR_TYPE                           \
        }                                               \
    }                                                   \
    M_Stack_dropn(self, n - 1);                         \
    self->data[self->stack_ptr].TAG = val;    

#define REDUCE_OP(TYPE, TAG, REDUCE_OP)                     \
    REDUCE(TYPE, TAG, val REDUCE_OP self->data[ptr].TAG;)

#define MATH_ARITY_CHECK                        \
    if(self->stack_ptr < n || n == 0){          \
        RETURN_ERROR(M_STATUS_ARITY_MISMATCH)   \
    }                                           \
    size_t ptr = self->stack_ptr - n;           \
    M_Type type = self->data[ptr].type;


#define ARITY_CHECK(N)                          \
    if(n != N || self->stack_ptr < N ){         \
        RETURN_ERROR(M_STATUS_ARITY_MISMATCH)   \
    }                                           \
    size_t ptr = self->stack_ptr - n;           \
    M_Type type = self->data[ptr].type;

M_Status M_Stack_fn_print(M_Stack* const self, const size_t n){
    if(self->stack_ptr < n){
        return M_STATUS_ARITY_MISMATCH;
    }

    size_t ptr = self->stack_ptr - n;
    for(size_t i = ptr; i < self->stack_ptr; i++){
        M_Object_print(&self->data[i]);
        M_Object_clear(&self->data[i]);
    }
    self->stack_ptr -= n;
    return M_STATUS_OK;
}
M_Status M_Stack_fn_input(M_Stack* const self, const size_t n){
    if(n != 0){
        RETURN_ERROR(M_STATUS_ARITY_MISMATCH) 
    }                                   
    
    M_Array str;
    M_IO_cin(&str);
    M_Stack_push_array(self, str);

    return M_STATUS_OK;
}


M_Status M_Stack_fn_add(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    switch(type){        
        case M_TYPE_CHAR:   { REDUCE_OP(M_Int, v_int, +=) break; }
        case M_TYPE_INT:    { REDUCE_OP(M_Int, v_int, +=) break; }
        case M_TYPE_FLOAT:  { REDUCE_OP(M_Float, v_float, +=) break; }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_sub(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    switch(type){
        case M_TYPE_CHAR:   { REDUCE_OP(M_Int, v_int, -=) break; }
        case M_TYPE_INT:    { REDUCE_OP(M_Int, v_int, -=) break; }
        case M_TYPE_FLOAT:  { REDUCE_OP(M_Float, v_float, -=) break; }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_mul(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    switch(type){
        case M_TYPE_CHAR:   { REDUCE_OP(M_Int, v_int, *=) break; }
        case M_TYPE_INT:    { REDUCE_OP(M_Int, v_int, *=) break; }
        case M_TYPE_FLOAT:  { REDUCE_OP(M_Float, v_float, *=) break; }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_div(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    switch(type){
        case M_TYPE_CHAR: {
            M_Char val = self->data[ptr].v_char;                                   
            for(size_t i = ptr + 1; i < self->stack_ptr; i++){  
                if(type == self->data[ptr].type){          
                    if(self->data[ptr].v_char != 0){
                        val /= self->data[ptr].v_char;
                    } 
                    else{
                        RETURN_ERROR(M_STATUS_DIVISION_BY_ZERO)
                    }                     
                }                                           
                else{                                       
                    RETURN_ERROR_TYPE                              
                }                                           
            }                                               
            M_Stack_dropn(self, n - 1);                     
            self->data[self->stack_ptr].v_char = val;
            break;
        }
        case M_TYPE_INT: { 
            M_Int val = self->data[ptr].v_int;                                   
            for(size_t i = ptr + 1; i < self->stack_ptr; i++){  
                if(type == self->data[ptr].type){          
                    if(self->data[ptr].v_int != 0){
                        val /= self->data[ptr].v_int;
                    } 
                    else{
                        RETURN_ERROR(M_STATUS_DIVISION_BY_ZERO)
                    }                     
                }                                           
                else{                                       
                    RETURN_ERROR_TYPE                              
                }                                           
            }                                               
            M_Stack_dropn(self, n - 1);                     
            self->data[self->stack_ptr].v_int = val;
            break;
        }
        case M_TYPE_FLOAT:  { 
            REDUCE_OP(M_Float, v_float, /=) 
            break; 
        }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_mod(M_Stack* const self, const size_t n){
    ARITY_CHECK(2)
    switch(type){
        case M_TYPE_CHAR: {
            M_Char val;
            if(self->data[ptr + 1].type == type){
                if(self->data[ptr + 1].v_char != 0){
                    val = self->data[ptr].v_char % self->data[ptr].v_char;
                }
                else{
                    RETURN_ERROR(M_STATUS_DIVISION_BY_ZERO)
                }
            }
            else{                                       
                RETURN_ERROR_TYPE                              
            }                                           
            M_Stack_dropn(self, n - 1);                     
            self->data[self->stack_ptr].v_char = val;
            break;
        }
        case M_TYPE_INT: { 
            M_Int val;
            if(self->data[ptr + 1].type == type){
                if(self->data[ptr + 1].v_int != 0){
                    val = self->data[ptr].v_int % self->data[ptr].v_int;
                }
                else{
                    RETURN_ERROR(M_STATUS_DIVISION_BY_ZERO)
                }
            }
            else{                                       
                RETURN_ERROR_TYPE                              
            }                                           
            M_Stack_dropn(self, n - 1);                     
            self->data[self->stack_ptr].v_int = val;
            break;
        }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}


M_Status M_Stack_fn_bitwise_and(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    switch(type){
        case M_TYPE_BOOL:    { REDUCE_OP(M_Int, v_bool, &=) break; }
        case M_TYPE_CHAR:    { REDUCE_OP(M_Int, v_char, &=) break; }
        case M_TYPE_INT:    { REDUCE_OP(M_Int, v_int, &=) break; }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_bitwise_or(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    switch(type){
        case M_TYPE_BOOL:    { REDUCE_OP(M_Int, v_bool, |=) break; }
        case M_TYPE_CHAR:    { REDUCE_OP(M_Int, v_char, |=) break; }
        case M_TYPE_INT:    { REDUCE_OP(M_Int, v_int, |=) break; }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_bitwise_xor(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    switch(type){
        case M_TYPE_BOOL:    { REDUCE_OP(M_Int, v_bool, ^=) break; }
        case M_TYPE_CHAR:    { REDUCE_OP(M_Int, v_char, ^=) break; }
        case M_TYPE_INT:    { REDUCE_OP(M_Int, v_int, ^=) break; }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_bitwise_not(M_Stack* const self, const size_t n){
    ARITY_CHECK(1)
    switch(type){
        case M_TYPE_BOOL:{ 
            self->data[self->stack_ptr].v_bool = !self->data[self->stack_ptr].v_bool;
            break;
        }
        case M_TYPE_CHAR:{ 
            self->data[self->stack_ptr].v_char = ~self->data[self->stack_ptr].v_char;
            break;
        }
        case M_TYPE_INT:{ 
            self->data[self->stack_ptr].v_int = ~self->data[self->stack_ptr].v_int;
            break;
        }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}

M_Status M_Stack_fn_and(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    switch(type){
        case M_TYPE_BOOL: { 
            for(size_t i = ptr; i < self->stack_ptr; i++){  
                if(type == self->data[ptr].type){
                    if(!self->data[ptr].v_bool){          
                        M_Stack_dropn(self, n - 1);                     
                        self->data[self->stack_ptr].v_bool = false;
                        return M_STATUS_OK;
                    }
                }                                           
                else{                                       
                    RETURN_ERROR_TYPE     
                }                                           
            }                                               
            M_Stack_dropn(self, n - 1);                     
            self->data[self->stack_ptr].v_bool = true;          
            break;
        }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_or(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    switch(type){
        case M_TYPE_BOOL:{ 
            for(size_t i = ptr; i < self->stack_ptr; i++){  
                if(type == self->data[ptr].type){
                    if(self->data[ptr].v_bool){          
                        M_Stack_dropn(self, n - 1);                     
                        self->data[self->stack_ptr].v_bool = true;
                        return M_STATUS_OK;
                    }
                }                                           
                else{                            
                    RETURN_ERROR_TYPE       
                }                                           
            }                                                      
            M_Stack_dropn(self, n - 1);                     
            self->data[self->stack_ptr].v_bool = false;          
            break;                                       
        }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_not(M_Stack* const self, const size_t n){
    ARITY_CHECK(1)

    switch(type){
        case M_TYPE_BOOL:{ 
            self->data[self->stack_ptr].v_bool = !self->data[self->stack_ptr].v_bool;
            break;
        }
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}


M_Status M_Stack_fn_seqget(M_Stack* const self, const size_t n){
    ARITY_CHECK(2)
    #define SEQGET(TYPE, GETTER)                \
        M_Object index = self->data[ptr + 1];   \
        if(index.type != TYPE){                 \
            RETURN_ERROR_TYPE                   \
        }                                       \
        M_Object out;                           \
        M_Status status = GETTER;               \
        switch(status){                         \
            case M_STATUS_OK:                   \
                M_Stack_dropn(self, 1);         \
                M_Stack_push(self, &out);       \
                break;                          \
            default: RETURN_ERROR(status)       \
        }

    switch(type){
        case M_TYPE_ARRAY: {
            SEQGET(M_TYPE_INT, M_Array_get(&self->data[ptr].arc->v_array, index.v_int, &out))
            break;
        }
        case M_TYPE_TUPLE: {
            SEQGET(M_TYPE_INT, M_Tuple_get(&self->data[ptr].arc->v_tuple, index.v_int, &out))
            break;
        }
        case M_TYPE_STRUCT: {
            SEQGET(M_TYPE_SYMBOL, M_Struct_get(&self->data[ptr].arc->v_struct, index.v_symbol, &out))
            break;
        }
        default: RETURN_ERROR_TYPE
    }
    #undef ARRGET

    return M_STATUS_OK;
}
M_Status M_Stack_fn_seqset(M_Stack* const self, const size_t n){
    ARITY_CHECK(3)
    #define SEQSET(TYPE, GETTER)                \
        M_Object index = self->data[ptr + 1];   \
        if(index.type != TYPE){                 \
            RETURN_ERROR_TYPE                   \
        }                                       \
        M_Object out = self->data[ptr + 2];     \
        M_Status status = GETTER;               \
        switch(status){                         \
            case M_STATUS_OK:                   \
                M_Stack_dropn(self, 2);         \
                M_Stack_push(self, &out);       \
                break;                          \
            default: RETURN_ERROR(status)       \
        }

    switch(type){
        case M_TYPE_ARRAY: {
            SEQSET(M_TYPE_INT, M_Array_set(&self->data[ptr].arc->v_array, index.v_int, &out))
            break;
        }
        case M_TYPE_TUPLE: {
            SEQSET(M_TYPE_INT, M_Tuple_set(&self->data[ptr].arc->v_tuple, index.v_int, &out))
            break;
        }
        case M_TYPE_STRUCT: {
            M_Object index = self->data[ptr + 1];   
            if(index.type != M_TYPE_SYMBOL){                 
                RETURN_ERROR_TYPE                   
            }                                       
            M_Object out = self->data[ptr + 2];     
            M_Struct_set(&self->data[ptr].arc->v_struct, index.v_symbol, &out);
            M_Stack_dropn(self, 2);         
            break;
        }
        default: RETURN_ERROR_TYPE
    }
    #undef SEQSET

    return M_STATUS_OK;
}

M_Status M_Stack_fn_seqpush(M_Stack* const self, const size_t n){
    ARITY_CHECK(2)
    switch(type){
        case M_TYPE_ARRAY:
            M_Array_push(&self->data[ptr].arc->v_array, &self->data[ptr + 1]);
            M_Stack_drop(self);
            break;
        case M_TYPE_TUPLE:
            M_Tuple_push(&self->data[ptr].arc->v_tuple, &self->data[ptr + 1]);
            M_Stack_drop(self);
            break;
        default: RETURN_ERROR_TYPE   
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_seqpop(M_Stack* const self, const size_t n){
    ARITY_CHECK(1)
    switch(type){
        case M_TYPE_ARRAY: { 
            M_Object out;
            M_Status status = M_Array_pop(&self->data[ptr].arc->v_array, &out);

            switch(status){
                case M_STATUS_OK:
                    M_Stack_drop(self);
                    M_Stack_push(self, &out);
                    break;
                default: RETURN_ERROR(status)
            }
            break;
        }
        case M_TYPE_TUPLE: {
            M_Object out;
            M_Status status = M_Tuple_pop(&self->data[ptr].arc->v_tuple, &out);
            
            switch(status){
                case M_STATUS_OK:
                    M_Stack_drop(self);
                    M_Stack_push(self, &out);
                    break;
                default: RETURN_ERROR(status)
            }
            break;
        }
        default: RETURN_ERROR_TYPE   
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_seqjoin(M_Stack* const self, const size_t n){
    MATH_ARITY_CHECK
    M_Object head = self->data[ptr];

    switch(type){
        case M_TYPE_ARRAY: {
            for(size_t i = ptr + 1; i < self->stack_ptr; i++){
                if(type == self->data[i].type){
                    switch(M_Array_join(&head.arc->v_array, &self->data[i].arc->v_array)){
                        case M_STATUS_OK: break;
                        default: RETURN_ERROR_TYPE
                    }
                }
                else{
                    RETURN_ERROR_TYPE
                }
            }
            M_Stack_dropn(self, n - 1);
            break;
        }
        case M_TYPE_TUPLE: {
            for(size_t i = ptr + 1; i < self->stack_ptr; i++){
                if(type == self->data[i].type){
                    M_Tuple_join(&head.arc->v_tuple, &self->data[i].arc->v_tuple);
                }
                else{
                    RETURN_ERROR_TYPE
                }
            }
            M_Stack_dropn(self, n - 1);
            break;
        }
        default: RETURN_ERROR_TYPE   
    }
    return M_STATUS_OK;
}
M_Status M_Stack_fn_seqsplice(M_Stack* const self, const size_t n){
    ARITY_CHECK(3)    

    switch(type){
        case M_TYPE_ARRAY: {
            M_Object head = self->data[ptr];
            M_Object i = self->data[ptr + 1];
            M_Object j = self->data[ptr + 2];

            if(i.type != M_TYPE_INT || j.type != M_TYPE_INT){
                RETURN_ERROR_TYPE
            }

            M_Array splice;
            M_Status status = M_Array_splice(&splice, &head.arc->v_array, i.v_int, j.v_int);

            if(status != M_STATUS_OK){
                RETURN_ERROR(status)
            }

            M_Stack_dropn(self, n);
            M_Stack_push_array(self, splice);
            break;
        }
        case M_TYPE_TUPLE: {
            M_Object head = self->data[ptr];
            M_Object i = self->data[ptr + 1];
            M_Object j = self->data[ptr + 2];

            if(i.type != M_TYPE_INT || j.type != M_TYPE_INT){
                RETURN_ERROR_TYPE
            }

            M_Tuple splice;
            M_Status status = M_Tuple_splice(&splice, &head.arc->v_tuple, i.v_int, j.v_int);

            if(status != M_STATUS_OK){
                RETURN_ERROR(status)
            }

            M_Stack_dropn(self, n);
            M_Stack_push_tuple(self, splice);
            break;
        }
        default: RETURN_ERROR_TYPE   
    }
    return M_STATUS_OK;
}

M_Status M_Stack_fn_seqreverse(M_Stack* const self, const size_t n){
    ARITY_CHECK(1)

    switch(type){
        case M_TYPE_ARRAY:
            M_Array_reverse(&self->data[ptr].arc->v_array);
            break;
        case M_TYPE_TUPLE:
            M_Tuple_reverse(&self->data[ptr].arc->v_tuple);
            break;
        default: RETURN_ERROR_TYPE
    }
    return M_STATUS_OK;
}


M_Status M_Stack_fn_eq(M_Stack* const self, const size_t n){
    ARITY_CHECK(2)
    M_Object lhs = self->data[ptr];
    M_Object rhs = self->data[ptr + 1];
    bool val = (type == rhs.type) ? M_Object_equal(&lhs, &rhs) : false;
    M_Stack_dropn(self, 2);
    M_Stack_push_bool(self, val);
    return M_STATUS_OK;
}
M_Status M_Stack_fn_neq(M_Stack* const self, const size_t n){
    ARITY_CHECK(2)
    M_Object lhs = self->data[ptr];
    M_Object rhs = self->data[ptr + 1];
    bool val = (type == rhs.type) ? !M_Object_equal(&lhs, &rhs) : true;
    M_Stack_dropn(self, 2);
    M_Stack_push_bool(self, val);
    return M_STATUS_OK;
}

#define COMPARE_FUNCTION(OP)                                                                        \
    ARITY_CHECK(2)                                                                                  \
    bool val = false;                                                                               \
    M_Object lhs = self->data[ptr];                                                                 \
    M_Object rhs = self->data[ptr + 1];                                                             \
    if(type == rhs.type){                                                                           \
        switch(type){                                                                               \
            case M_TYPE_NONE:                                                                       \
            case M_TYPE_KEYWORD:                                                                    \
            case M_TYPE_SYMBOL:                                                                     \
            case M_TYPE_TYPE:                                                                       \
            case M_TYPE_BOOL:                                                                       \
                RETURN_ERROR_TYPE                                                                   \
            case M_TYPE_CHAR:   val = (lhs.v_char   OP rhs.v_char);     break;                      \
            case M_TYPE_INT:    val = (lhs.v_int    OP rhs.v_int);      break;                      \
            case M_TYPE_FLOAT:  val = (lhs.v_float  OP rhs.v_float);    break;                      \
            case M_TYPE_ARRAY:  val = M_Array_equal(&lhs.arc->v_array, &rhs.arc->v_array);  break;  \
            case M_TYPE_TUPLE:  val = M_Tuple_equal(&lhs.arc->v_tuple, &rhs.arc->v_tuple);  break;  \
            default:                                                                                \
                M_panic_type(type, "in Stack_fn_lt");                                               \
                return M_STATUS_COMPILER_ERROR;                                                     \
        }                                                                                           \
    }                                                                                               \
    M_Stack_dropn(self, 2);                                                                         \
    M_Stack_push_bool(self, val);                                                                   \
    return M_STATUS_OK;

M_Status M_Stack_fn_lt(M_Stack* const self, const size_t n){
    COMPARE_FUNCTION(<)
}
M_Status M_Stack_fn_lte(M_Stack* const self, const size_t n){
    COMPARE_FUNCTION(<=)
}
M_Status M_Stack_fn_gt(M_Stack* const self, const size_t n){
    COMPARE_FUNCTION(>)
}
M_Status M_Stack_fn_gte(M_Stack* const self, const size_t n){
    COMPARE_FUNCTION(>=)
}
#undef COMPARE_FUNCTION

#undef REDUCE_OP
#undef MATH_ARITY_CHECK
#undef ARITY_CHECK
#undef RETURN_ERROR_TYPE
