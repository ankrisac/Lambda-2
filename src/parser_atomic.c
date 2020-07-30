#include "parser_atomic.h"

M_Float M_Module_parse_Frac(M_Module_Pos* const pos){
    M_Float frac = 0, exp = 1.0;

    do{
        char chr = M_Module_Pos_peek(pos);
        switch(chr){
            M_CASE_NUM{
                exp *= 0.1;
                frac += exp * (M_Float)(chr - '0');
                break;
            }
            case '_':
                break;
            default:
                return frac;
        }
    } while(M_Module_Pos_next(pos));

    return frac;
}
M_Object M_Module_parse_Number(M_Module_Pos* const pos, M_ErrorStack* const err_trace){
    M_Object out = (M_Object){
        .type = M_TYPE_ERROR,
        .v_error = M_STATUS_PARSE_ERROR
    };

    M_Int integer = 0;
    char chr;

    switch((chr = M_Module_Pos_peek(pos))){
        M_CASE_NUM break;
        default:
            return out;
    }

    do{
        switch((chr = M_Module_Pos_peek(pos))){
            case '_':
                break;
            M_CASE_NUM { 
                integer = 10 * integer + (M_Int)(chr - '0');
                break;
            }
            case '.':
                M_Module_Pos_next(pos);
                switch((chr = M_Module_Pos_peek(pos))){
                    M_CASE_NUM
                        return (M_Object) { 
                            .type = M_TYPE_FLOAT, 
                            .v_float = (float)integer + M_Module_parse_Frac(pos) 
                        };
                    default:
                        M_Module_Pos_prev(pos);
                        return (M_Object){
                            .type = M_TYPE_INT,
                            .v_int = integer
                        };
                }
                break;
            default:
                return (M_Object){
                    .type = M_TYPE_INT,
                    .v_int = integer
                };
        }
    } while(M_Module_Pos_next(pos));

    return out;
}
M_Object M_Module_parse_Operator(M_Module_Pos* const pos, M_SymbolTable* const table){
    M_Object out = (M_Object){ 
        .type = M_TYPE_ERROR, 
        .v_error = M_STATUS_PARSE_ERROR 
    };
    M_Module_Pos iter = *pos;
    
    M_Str str;
    char chr;

    M_Array_init(&str, M_TYPE_CHAR, 8);
    do{
        switch((chr = M_Module_Pos_peek(&iter))){
            M_CASE_OPERATOR{
                M_Str_push(&str, chr);
                break;
            }
            case '\0':
                if(str.len > 0){
                    M_Object_set_symbol(&out, M_Symbol_new(table, &str));
                    M_Module_Pos_advance(pos, &iter);
                }                
                M_Array_clear(&str);    
                return out;
            default: {
                if(str.len > 0){
                    M_Object_set_symbol(&out, M_Symbol_new(table, &str));
                    M_Module_Pos_advance(pos, &iter);
                }                
                M_Array_clear(&str);    
                return out;
            }
        }
    } while(M_Module_Pos_next(&iter));

    M_Array_clear(&str);
    return out;
}
M_Object M_Module_parse_Symbol(M_Module_Pos* const pos, M_SymbolTable* const table){
    M_Object out = (M_Object){ 
        .type = M_TYPE_ERROR, 
        .v_error = M_STATUS_PARSE_ERROR
    };
    M_Module_Pos iter = *pos;
    
    M_Str str;
    char chr;

    M_Array_init(&str, M_TYPE_CHAR, 8);
    do{
        switch((chr = M_Module_Pos_peek(&iter))){ 
            M_CASE_SPACE 
            M_CASE_STRING
            M_CASE_SYNTAX 
            M_CASE_OPERATOR 
            case '\0': {
                if(str.len > 0){
                    M_Object_set_symbol(&out, M_Symbol_new(table, &str));
                    M_Module_Pos_advance(pos, &iter);
                }
                M_Array_clear(&str);
                return out;
            } 
            default:
                M_Str_push(&str, chr);
                break;
        }
    } while(M_Module_Pos_next(&iter));

    M_Array_clear(&str);
    return out;
}

M_Object M_Module_wrap_String(M_SymbolTable* const table, M_Str* const input_str, M_Module_Pos pos){
    M_Object out;
    M_Expr expr;
    M_Expr_init(&expr, 2);

    M_Object header, data;
    M_Object_set_symbol(&header, M_Symbol_from_cstr(table, "str"));
    M_Object_set_symbol(&data, M_Symbol_new(table, input_str));
    M_Array_clear(input_str);

    M_Module_Pos header_pos = pos;
    header_pos.begin = header_pos.end = pos.begin;
    M_Expr_push(&expr, &header, &header_pos);
    M_Expr_push(&expr, &data, &pos);
    
    M_Object_set_expr(&out, expr);
    return out;
}
M_Object M_Module_parse_String(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace){
    M_Object out = (M_Object){ 
        .type = M_TYPE_ERROR, 
        .v_error = M_STATUS_PARSE_ERROR 
    };
    
    bool raw = false;
    switch(M_Module_Pos_peek(pos)){
        case 'r':
        case 'R':
            M_Module_Pos_next(pos);
            raw = true;
            break;  
        default:
            break;
    }

    M_Str str;
    M_Array_init(&str, M_TYPE_CHAR, 8);

    size_t escape_count = 0;
    while(M_Module_Pos_peek(pos) == '#'){
        M_Module_Pos_next(pos);
        escape_count++;
    }

    if(M_Module_Pos_peek(pos) != '"'){
        M_Str error;
        M_Str_from_cstr(&error, 
            "Expected opening delimiter " M_COLOR "\"" M_RESET 
            " of " M_COLOR "string" M_RESET);
        M_ErrorStack_pushLocStrMsg(err_trace, *pos, error);
        return M_Module_wrap_String(table, &str, *pos);    
    }

    char chr;
    while(M_Module_Pos_next(pos)){
        switch((chr = M_Module_Pos_peek(pos))){
            case '"': { 
                M_Str_push(&str, chr);        
                M_Module_Pos_next(pos);
                size_t i;

                for(i = 0; i < escape_count; i++) {
                    char chr = M_Module_Pos_peek(pos);
                    if(M_Module_Pos_peek(pos) != '#'){
                        break;
                    }
                    M_Str_push(&str, chr);
                    M_Module_Pos_next(pos);
                } 
                if(i == escape_count){ 
                    M_Array_dropn(&str, escape_count + 1);
                    return M_Module_wrap_String(table, &str, *pos);
                }
                M_Module_Pos_prev(pos);
                break;
            }
            case '\\': 
                if(raw){
                    M_Str_push(&str, chr);
                }
                else{
                    if(!M_Module_Pos_next(pos)){ 
                        M_Array_clear(&str);
                        return out;
                    }

                    switch(chr = M_Module_Pos_peek(pos)){
                        case 't': M_Str_push(&str, '\t'); break;
                        case 'r': M_Str_push(&str, '\r'); break;
                        case 'n': M_Str_push(&str, '\n'); break;
                        case 'f': M_Str_push(&str, '\f'); break;
                        case 'b': M_Str_push(&str, '\b'); break;
                        default: M_Str_push(&str, chr); break;
                    }
                }
                break;
            default:
                M_Str_push(&str, chr);
                break;
        }
    }        
     
    if(str.data_char[str.len - 1] == '\0'){
        M_Array_drop(&str);
    }

    M_Str error;
    M_Str_from_cstr(&error, "Expected closing delimiter " M_COLOR "\"");
    for(size_t i = 0; i < escape_count; i++){
        M_Str_push(&error, '#');
    }
    M_Str_join_cstr(&error, M_RESET " of " M_COLOR "string" M_RESET);
    M_ErrorStack_pushLocStrMsg(err_trace, *pos, error);
    
    return M_Module_wrap_String(table, &str, *pos);
}
M_Object M_Module_parse_Atom(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace){
    char chr = M_Module_Pos_peek(pos);
    switch(chr){
        M_CASE_NUM return M_Module_parse_Number(pos, err_trace);
        
        case 'r':
        case 'R':
            switch(M_Module_Pos_peekahead(pos, 1)){
                case '#':
                case '"': return M_Module_parse_String(pos, table, err_trace);
                default: return M_Module_parse_Symbol(pos, table);
            }
            break;
        case '#':
        case '"': return M_Module_parse_String(pos, table, err_trace);

        case '(': return M_Module_parse_TupleExpr(pos, table, err_trace);
        case '[': return M_Module_parse_ListExpr(pos, table, err_trace);
        case '{': return M_Module_parse_BlockExpr(pos, table, err_trace);

        case ')':
        case ']':
        case '}':
    
        M_CASE_SPACE
        M_CASE_OPERATOR
        case ';':
        case '\0':
            return (M_Object){ 
                .type = M_TYPE_ERROR, 
                .v_error = M_STATUS_PARSE_ERROR 
            };
        default: 
            return M_Module_parse_Symbol(pos, table);
    }
}

void M_Module_parse_Space(M_Module_Pos* const pos){
    while(true){
        switch(M_Module_Pos_peek(pos)){
            M_CASE_SPACE {
                M_Module_Pos_next(pos);
                continue;
            }
            default:
                break;
        }
        break;
    }
}