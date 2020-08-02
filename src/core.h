#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <limits.h>
#include <string.h>
#include <assert.h>
#include <string.h>

void M_Init();
void M_Quit();

void* M_calloc(const size_t len, size_t obj_size);
void* M_malloc(const size_t len, size_t obj_size);
void* M_realloc(void* ptr, const size_t len, size_t obj_size);

typedef enum {
    M_TYPE_TYPE,
    M_TYPE_KEYWORD,
    M_TYPE_ERROR,
    M_TYPE_SYMBOL,

    M_TYPE_NONE,
    M_TYPE_BOOL,
    M_TYPE_CHAR,
    M_TYPE_INT,
    M_TYPE_FLOAT,

    M_TYPE_ARRAY,
    M_TYPE_EXPR,
    M_TYPE_STRUCT,

    M_TYPE_TUPLE,
    M_TYPE_TABLE,
    M_TYPE__LEN,
}M_Type;

typedef enum{
    M_STATUS_OK,
    M_STATUS_COMPILER_ERROR,
    
    M_STATUS_TYPE_MISMATCH,
    M_STATUS_UNDEFINED_SYMBOL,

    M_STATUS_OUT_OF_RANGE,
    M_STATUS_ARITY_MISMATCH,
    
    M_STATUS_DIVISION_BY_ZERO,
    M_STATUS_FILE_ERROR,

    M_STATUS_PARSE_ERROR
}M_Status;

typedef bool M_Bool;
typedef char M_Char;
typedef long long int M_Int;
typedef double  M_Float;

typedef enum {
    M_KEYWORD_TYPE,
    M_KEYWORD__LEN
}M_Keyword;
typedef struct M_Symbol M_Symbol;

typedef struct M_Array M_Array;
typedef struct M_Tuple M_Tuple;
typedef struct M_Struct M_Struct;
typedef struct M_Expr M_Expr;

typedef struct M_ARC_Object M_ARC_Object;
typedef struct M_GC_Object M_GC_Object;
typedef struct M_Object M_Object;
typedef M_Array M_Str;

typedef struct M_Module_Pos M_Module_Pos;

struct M_Array{
    union {
        M_Type* data_type;
        M_Keyword* data_keyword;
        
        M_Bool* data_bool;
        M_Char* data_char;
        M_Int* data_int;
        M_Float* data_float;  

        const M_Symbol** data_symbol;
        M_ARC_Object** data_arc;
    };
    M_Type type;

    size_t len;
    size_t reserve_len;
};
struct M_Tuple{
    M_Object* data;
    
    size_t len;
    size_t reserve_len;
};
struct M_Struct{
    const M_Symbol** keys;
    M_Object* data;

    size_t elems;
    size_t len;
};
struct M_Expr{
    M_Object* data;
    M_Module_Pos* pos;

    size_t len;
    size_t reserve_len;
};

struct M_Symbol{
    size_t hash;
    M_Array data;
};
typedef struct{
    M_Symbol** keys;
    size_t elems;
    size_t len;
}M_SymbolTable;

typedef struct{
    M_Array error_stack;
}M_ErrorStack;

typedef struct{
    M_Array path;
    M_Array data;
}M_Module;

typedef struct{
    M_SymbolTable path_list;

    M_Module* modules;
    size_t modules_len;
    size_t modules_reserve_len;
}M_ModuleList;

struct M_Module_Pos{
    const M_Module* module;
    
    size_t begin;
    size_t end;
    size_t line;
};

typedef struct {
    M_SymbolTable table;
}M_Parser;

struct M_ARC_Object{
    union{
        M_Array v_array;
        M_Tuple v_tuple;
        M_Struct v_struct;
        M_Expr v_expr;
    };

    size_t ref_count;
};


typedef struct{
    M_Symbol* keys;
    M_Object* data;
    
    size_t len;
    size_t reserve_len;
}M_Table;

struct M_GC_Object{
    union{
        M_Table v_table;
    };
    
    bool mark;
    size_t ref_count;
};

struct M_Object{
    union{
        M_Status v_error;
        
        M_Type v_type;
        M_Keyword v_keyword;
        const M_Symbol* v_symbol;
        
        M_Bool v_bool;
        M_Char v_char;
        M_Int v_int;
        M_Float v_float;        

        M_ARC_Object* arc;
        M_GC_Object* gc;
    };

    M_Type type;
};

typedef struct{
    M_ErrorStack err_stack;
    M_SymbolTable symtable;
    
    M_Object tree;
    M_Module module;

    M_Array style_stack;
    M_Str input;
    M_Str output;
}M_JSLint;


#define M_PANIC(MSG)\
    printf("CompilerError: Unreachable situation in %s:" MSG, __func__)

#define M_PANIC_TYPE(TYPE, MSG)                 \
    printf("CompilerError: Unexpected type ["); \
    M_Type_print(TYPE);                         \
    printf("] in %s:" MSG "\n", __func__);



#include "type-arc/object.h"
#include "type-arc/symbol.h"

#include "type-arc/array.h"
#include "type-arc/str.h"
#include "type-arc/expr.h"

#include "type-arc/tuple.h"
#include "type-arc/struct.h"

#include "stack.h"

#include "io.h"
#include "error.h"

#include "parser/module.h"
#include "parser/atomic.h"
#include "parser/group.h"
#include "lint.h"