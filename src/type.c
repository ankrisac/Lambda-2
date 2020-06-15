#include "type.h"

void M_panic(const char* const msg){
    printf("CompilerError: Unreachable situation [%s]\n", msg);
}
void M_panic_type(const M_Type type, const char* const msg){
    printf("CompilerError: Unexpected type [");
    M_Type_print(type);
    printf("] %s\n", msg);
}

void* M_malloc(const size_t len, size_t obj_size){
    void* ptr = malloc(len * obj_size);
    if(ptr == NULL){
        M_panic("MemoryError: Malloc failed, insufficient memory");
    }
    return ptr;
}
void* M_calloc(const size_t len, size_t obj_size){
    void* ptr = calloc(len, obj_size);
    if(ptr == NULL){
        M_panic("MemoryError: Malloc failed, insufficient memory");
    }
    return ptr;
}
void* M_realloc(void* ptr, const size_t len, size_t obj_size){
    ptr = realloc(ptr, len * obj_size);
    if(ptr == NULL){
        M_panic("MemoryError: Realloc failed, insufficient memory");
    }
    return ptr;
}

void M_Status_print(const M_Status error){
    printf("Error: ");
    switch(error){
        case M_STATUS_OK:               printf("No error");         break;
        case M_STATUS_UNDEFINED_SYMBOL: printf("Undefined symbol"); break;
        case M_STATUS_TYPE_MISMATCH:    printf("Type mismatch");    break;
        case M_STATUS_ARITY_MISMATCH:   printf("Arity mismatch");   break;
        case M_STATUS_COMPILER_ERROR:   printf("Compiler error");   break;
        case M_STATUS_DIVISION_BY_ZERO: printf("Division by zero"); break;
        case M_STATUS_OUT_OF_RANGE:     printf("Out of range");     break;
        case M_STATUS_FILE_ERROR:       printf("File error");       break;
        
        default:
            M_panic("in Status_print");
            break;
    }
}
void M_Type_print(const M_Type type){
    switch(type){
        case M_TYPE_TYPE:       printf("Type");     break;
        case M_TYPE_NONE:       printf("None");     break;

        case M_TYPE_ERROR:      printf("Error");    break;
        case M_TYPE_KEYWORD:    printf("Keyword");  break;
        case M_TYPE_SYMBOL:     printf("Symbol");   break;    
    
        case M_TYPE_BOOL:       printf("Bool");     break;
        case M_TYPE_CHAR:       printf("Char");     break;
        case M_TYPE_INT:        printf("Int");      break;
        case M_TYPE_FLOAT:      printf("Float");    break;

        case M_TYPE_EXPR:       printf("Expr");     break;
        case M_TYPE_ARRAY:      printf("Array");    break;
        case M_TYPE_TUPLE:      printf("Tuple");    break;
        case M_TYPE_STRUCT:     printf("Struct");   break;

        case M_TYPE_TABLE:      printf("Table");    break;
        default:
            M_panic("in Type_print");
            break;
    }
}
void M_Keyword_print(const M_Keyword keyword){
    switch(keyword){
        case M_KEYWORD_TYPE:    printf("Type");     break;
        default:
            M_panic("in Keyword_print");
            break;
    }
}
void M_Symbol_print(const M_Symbol* const val){
    printf("%.*s", (int)val->data.len, val->data.data_char);
}
void M_Char_repr(const M_Char val){
    if(val < 32){
        switch(val){
            case '\n': printf("\\n"); break;
            case '\t': printf("\\t"); break;
            case '\r': printf("\\r"); break;
            case '\f': printf("\\f"); break;
            default: printf("\\%u", val); break;
        }
    }
    else{
        printf("%c", val);
    }
}

M_ARC_Object* M_ARC_Object_new(){
    M_ARC_Object* self = M_malloc(1, sizeof* self);
    self->ref_count = 1;
    return self;
}

void M_Object_set_none(M_Object* const self){
    self->type = M_TYPE_NONE;
}
void M_Object_set_error(M_Object* const self, const M_Status error){
    self->type = M_TYPE_ERROR;
    self->v_error = error;
}
void M_Object_set_keyword(M_Object* const self, const M_Keyword val){
    self->type = M_TYPE_KEYWORD;
    self->v_keyword = val;
}
void M_Object_set_type(M_Object* const self, const M_Type val){
    self->type = M_TYPE_TYPE;
    self->v_type = val;
}
void M_Object_set_symbol(M_Object* const self, const M_Symbol* const val){
    self->type = M_TYPE_SYMBOL;
    self->v_symbol = val;
}

void M_Object_set_bool(M_Object* const self, const M_Bool val){
    self->type = M_TYPE_BOOL;
    self->v_bool = val;
}
void M_Object_set_char(M_Object* const self, const M_Char val){
    self->type = M_TYPE_CHAR;
    self->v_char = val;
}
void M_Object_set_int(M_Object* const self, const M_Int val){
    self->type = M_TYPE_INT;
    self->v_int = val;
}
void M_Object_set_float(M_Object* const self, const M_Float val){
    self->type = M_TYPE_FLOAT;
    self->v_float = val;
}

void M_Object_set_array(M_Object* const self, const M_Array val){
    self->type = M_TYPE_ARRAY;
    self->arc = M_ARC_Object_new();
    self->arc->v_array = val;
}
void M_Object_set_tuple(M_Object* const self, const M_Tuple val){
    self->type = M_TYPE_TUPLE;
    self->arc = M_ARC_Object_new();
    self->arc->v_tuple = val;
}
void M_Object_set_struct(M_Object* const self, const M_Struct val){
    self->type = M_TYPE_STRUCT;    
    self->arc = M_ARC_Object_new();
    self->arc->v_struct = val;
}

void M_Object_clear(M_Object* const self){
    #define ARC_CLEAR(EXPR)             \
        if(self->arc->ref_count == 1){  \
            EXPR;                       \
            free(self->arc);            \
        }                               \
        else{                           \
            self->arc->ref_count--;     \
        }

    switch(self->type){
        case M_TYPE_NONE:
        case M_TYPE_ERROR:

        case M_TYPE_TYPE:        
        case M_TYPE_KEYWORD:
        case M_TYPE_SYMBOL:

        case M_TYPE_BOOL:
        case M_TYPE_CHAR:
        case M_TYPE_INT:
        case M_TYPE_FLOAT:
            break;

        case M_TYPE_ARRAY:  ARC_CLEAR(M_Array_clear(&self->arc->v_array))   break;
        case M_TYPE_TUPLE:  ARC_CLEAR(M_Tuple_clear(&self->arc->v_tuple))   break;
        case M_TYPE_STRUCT: ARC_CLEAR(M_Struct_clear(&self->arc->v_struct)) break;

        default:
            M_panic_type(self->type, "in Object_clear");
            break;
    }
}
void M_Object_delete(M_Object* const self){
    M_Object_clear(self);
    free(self);
}

M_Status M_Object_copy(M_Object* const self, const M_Object* const src){
    if(self == src){
        return M_STATUS_OK;
    }
    
    *self = *src;

    switch(self->type){
        case M_TYPE_NONE:
        case M_TYPE_ERROR:

        case M_TYPE_KEYWORD:
        case M_TYPE_TYPE:
        case M_TYPE_SYMBOL:

        case M_TYPE_BOOL:
        case M_TYPE_CHAR:
        case M_TYPE_INT:
        case M_TYPE_FLOAT:
            break;

        case M_TYPE_ARRAY:
            self->arc = M_ARC_Object_new();
            M_Array_copy(&self->arc->v_array, &src->arc->v_array);
            break;
        case M_TYPE_TUPLE:
            self->arc = M_ARC_Object_new();
            M_Tuple_copy(&self->arc->v_tuple, &src->arc->v_tuple);
            break;
        case M_TYPE_STRUCT:
            self->arc = M_ARC_Object_new();
            M_Struct_copy(&self->arc->v_struct, &src->arc->v_struct);
            break;

        default:
            M_panic_type(self->type, "in Object_copy");
            return M_STATUS_COMPILER_ERROR;
    }
    return M_STATUS_OK;
}
void M_Object_share(M_Object* const self, M_Object* const src){
    *self = *src;

    switch(self->type){
        case M_TYPE_NONE:
        case M_TYPE_ERROR:

        case M_TYPE_TYPE:
        case M_TYPE_KEYWORD:
        case M_TYPE_SYMBOL:
        
        case M_TYPE_BOOL:
        case M_TYPE_CHAR:
        case M_TYPE_INT:
        case M_TYPE_FLOAT:
            break;

        case M_TYPE_ARRAY:
        case M_TYPE_TUPLE:
        case M_TYPE_STRUCT:
            self->arc = src->arc;
            self->arc->ref_count++;
            break;

        default:
            M_panic_type(self->type, "in Object_share");
            break;
    }
}
void M_Object_repr(const M_Object* const self){
    switch(self->type){
        case M_TYPE_NONE:       printf("None");                             break;
        case M_TYPE_ERROR:      M_Status_print(self->v_error);              break;

        case M_TYPE_TYPE:       M_Type_print(self->v_type);                 break;
        case M_TYPE_KEYWORD:    M_Keyword_print(self->v_keyword);           break;
        case M_TYPE_SYMBOL:     M_Symbol_print(self->v_symbol);             break;
        
        case M_TYPE_BOOL:       printf(self->v_bool ? "True" : "False");    break;
        case M_TYPE_CHAR:       M_Char_repr(self->v_char);                  break;
        case M_TYPE_INT:        printf("%lli", self->v_int);                break;
        case M_TYPE_FLOAT:      printf("%g", self->v_float);                break;

        case M_TYPE_ARRAY:      M_Array_repr(&self->arc->v_array);          break;
        case M_TYPE_TUPLE:      M_Tuple_repr(&self->arc->v_tuple);          break;
        case M_TYPE_STRUCT:     M_Struct_repr(&self->arc->v_struct);        break;

        default:
            M_panic_type(self->type, "in Object_repr");
            break;
    }
}
void M_Object_print(const M_Object* const self){
    switch(self->type){
        case M_TYPE_NONE:       printf("None");                             break;
        case M_TYPE_ERROR:      M_Status_print(self->v_error);              break;
        
        case M_TYPE_TYPE:       M_Type_print(self->v_type);                 break;
        case M_TYPE_KEYWORD:    M_Keyword_print(self->v_keyword);           break;
        case M_TYPE_SYMBOL:     M_Symbol_print(self->v_symbol);             break;
        
        case M_TYPE_BOOL:       printf(self->v_bool ? "True" : "False");    break;
        case M_TYPE_CHAR:       printf("%c", self->v_char);                 break;
        case M_TYPE_INT:        printf("%lli", self->v_int);                break;
        case M_TYPE_FLOAT:      printf("%g", self->v_float);                break;

        case M_TYPE_ARRAY:      M_Array_print(&self->arc->v_array);         break;
        case M_TYPE_TUPLE:      M_Tuple_print(&self->arc->v_tuple);         break;
        case M_TYPE_STRUCT:     M_Struct_print(&self->arc->v_struct);       break;

        default:
            M_panic_type(self->type, "in Object_print");
            break;
    }
}
void M_Object_desc(const M_Object* const self){
    M_Type_print(self->type);
    printf(":");
    M_Object_print(self);
}

bool M_Object_equal(const M_Object* const self, const M_Object* const src){
    if(self == src){ return true; }

    switch(self->type){
        case M_TYPE_NONE:       return true;    
        case M_TYPE_TYPE:       return self->v_type     == src->v_type;
        case M_TYPE_KEYWORD:    return self->v_keyword  == src->v_keyword;
        case M_TYPE_SYMBOL:     return self->v_error    == src->v_error;

        case M_TYPE_CHAR:   return self->v_char     == src->v_char;
        case M_TYPE_BOOL:   return self->v_bool     == src->v_bool;
        case M_TYPE_INT:    return self->v_int      == src->v_int;
        case M_TYPE_FLOAT:  return self->v_float    == src->v_float;
        
        case M_TYPE_ARRAY:  return M_Array_equal(&self->arc->v_array, &src->arc->v_array);
        case M_TYPE_TUPLE:  return M_Tuple_equal(&self->arc->v_tuple, &src->arc->v_tuple);
        case M_TYPE_STRUCT: return M_Struct_equal(&self->arc->v_struct, &src->arc->v_struct);

        default:
            M_panic_type(self->type, "in Object_equal");   
            return false;
    }
}