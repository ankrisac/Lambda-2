#include "group.h"

M_Object M_Module_wrap_FuncExpr(M_Expr expr){    
    M_Object out;
    switch(expr.len){
        case 1:
            out = expr.data[0];
            expr.len--;
            M_Expr_clear(&expr);
            return out;
        case 0:
            M_Object_set_error(&out, M_STATUS_PARSE_ERROR);
            return out;
        default:     
            M_Object_set_expr(&out, expr);
            return out;
    }
}
M_Object M_Module_parse_FuncExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace){
    M_Module_Pos iter = *pos;
    iter.begin = iter.end;

    M_Object head = M_Module_parse_Atom(&iter, table, err_trace);
    M_Module_Pos_advance(pos, &iter);
        
    if(head.type == M_TYPE_ERROR){
        return head;
    }
    
    M_Expr expr;
    M_Expr_init(&expr, 2);
    M_Expr_push(&expr, &head, &iter);
    
    while(true){
        M_Module_parse_Space(&iter);
        iter.begin = iter.end;

        size_t error_count = err_trace->error_stack.len;
        M_Object tail = M_Module_parse_Atom(&iter, table, err_trace);

        if(tail.type != M_TYPE_ERROR){
            M_Expr_push(&expr, &tail, &iter);
            M_Module_Pos_advance(pos, &iter);
            continue;
        }

        if(iter.begin == iter.end){
            M_ErrorStack_revert(err_trace, error_count);
            M_Object_clear(&tail);
        }
        else{
            M_Expr_push(&expr, &tail, &iter);
            M_Module_Pos_advance(pos, &iter);
        }

        return M_Module_wrap_FuncExpr(expr);
    }
}

/* expr is consumed */
M_Object M_Module_wrap_infix(M_Expr expr){
    M_Object out;

    switch(expr.len){
        case 2:
            out = expr.data[1];
            expr.len--;
            M_Expr_clear(&expr);
            return out;
        case 1:
        case 0:
            M_Expr_clear(&expr);
            M_Object_set_error(&out, M_STATUS_PARSE_ERROR);
            return out;
        default:
            break;
    }

    M_Object_set_expr(&out, expr);
    return out;
}

M_Object M_Module_parse_InfixExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace){
    M_Object tail;
    M_Expr expr;

    M_Module_Pos iter = *pos;
    iter.begin = iter.end;

    M_Expr_init(&expr, 2);
    M_Object_set_symbol(&tail, M_Symbol_from_cstr(table, "infix"));
    M_Expr_push(&expr, &tail, &iter);

    size_t error_count;
    while(true){
        M_Module_parse_Space(&iter);
        iter.begin = iter.end;

        error_count = err_trace->error_stack.len;
        tail = M_Module_parse_FuncExpr(&iter, table, err_trace);

        if(tail.type != M_TYPE_ERROR){        
            M_Module_Pos_advance(pos, &iter);
            M_Expr_push(&expr, &tail, &iter);           
            continue;
        }

        if(iter.begin == iter.end){
            M_Object_clear(&tail);
            M_ErrorStack_revert(err_trace, error_count);
        }
        else{
            M_Expr_push(&expr, &tail, &iter);    
            M_Module_Pos_advance(pos, &iter);
            return M_Module_wrap_infix(expr);
        }
    
        error_count = err_trace->error_stack.len;
        tail = M_Module_parse_Operator(&iter, table);      

        if(tail.type != M_TYPE_ERROR){
            M_Module_Pos_advance(pos, &iter);
            M_Expr_push(&expr, &tail, &iter);    
            continue;
        }            

        if(iter.begin == iter.end){
            M_Object_clear(&tail);
            M_ErrorStack_revert(err_trace, error_count);
            return M_Module_wrap_infix(expr);
        }

        M_Expr_push(&expr, &tail, &iter);    
        M_Module_Pos_advance(pos, &iter);
        return M_Module_wrap_infix(expr);
    }
}

void M_Module_error_Group(M_ErrorStack* const err_trace, M_Module_Pos pos, 
    char token, const char* const name){
    
    M_Str err;
    M_Str_from_cstr(&err, "Expected " M_COLOR);
    M_Str_push(&err, token);
    M_Str_join_cstr(&err, M_RESET " of " M_COLOR);
    M_Str_join_cstr(&err, name);
    M_Str_join_cstr(&err, M_RESET);
    
    M_ErrorStack_pushLocStrMsg(err_trace, pos, err);
}

/*  Attempt parsing of infix expr (as a subexpr of a group)
    return true if parsing resynchronization is performed   */
bool M_Module_sync_Group(M_Expr* const expr, M_Module_Pos* const pos, 
    M_SymbolTable* const table, M_ErrorStack* const err_trace,
    char begin, char end, const char* const name)
{
    pos->begin = pos->end;
    M_Object tail = M_Module_parse_InfixExpr(pos, table, err_trace);

    if(tail.type != M_TYPE_ERROR){
        M_Expr_push(expr, &tail, pos);    
        return false;
    }

    do{
        char chr = M_Module_Pos_peek(pos);
        if(chr == ',' || chr == end){   
            return true;
        }
        if(chr == '\0'){
            break;
        }
    }while(M_Module_Pos_next(pos));
    

    M_Str err;
    M_Str_from_cstr(&err, "Expected closing " M_COLOR);
    M_Str_push(&err, end);
    M_Str_join_cstr(&err, M_RESET " of " M_COLOR);
    M_Str_join_cstr(&err, name);
    M_Str_join_cstr(&err, M_RESET);

    M_ErrorStack_pushLocStrMsg(err_trace, *pos, err);
    return true;
}

/* Generalization of tuple (a,b,..) and list [a,b,...] expr */
M_Object M_Module_parse_Group(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace,
    char begin, char end, char* const name, M_Object (*wrapper)(M_Expr))
{  
    if(M_Module_Pos_peek(pos) == begin){ 
        M_Module_Pos_next(pos);
        M_Module_parse_Space(pos);
        pos->begin = pos->end;
    }
    else{
        M_Module_Pos_advance(pos, pos);
        M_Module_error_Group(err_trace, *pos, begin, name);
        return (M_Object){
            .type = M_TYPE_ERROR,
            .v_error = M_STATUS_PARSE_ERROR
        };
    }
    
    M_Expr expr;
    M_Expr_init(&expr, 2);

    if(M_Module_Pos_peek(pos) == end){
        M_Module_Pos_next(pos);
        return wrapper(expr);
    }
    
    while(true){
        if(M_Module_sync_Group(&expr, pos, table, err_trace, begin, end, name)
        && M_Module_Pos_peek(pos) == ')'){
            M_Module_Pos_next(pos);
            return wrapper(expr);
        }
        M_Module_parse_Space(pos);
        char chr = M_Module_Pos_peek(pos);
        
        if(chr == ','){
            M_Module_Pos_next(pos);
            continue;
        }
        else if(chr == end){
            M_Module_Pos_next(pos);
            return wrapper(expr);
        } 
 
        M_Str err;
        M_Str_from_cstr(&err, 
            "Expected " M_COLOR "expr" M_RESET
            " or closing " M_COLOR);
        M_Str_push(&err, end);
        M_Str_join_cstr(&err, M_RESET " in " M_COLOR);

        M_ErrorStack_pushLocStrMsg(err_trace, *pos, err);
        return wrapper(expr);
    }
}

/*  Singleton tuples (a) are interpreted for precedence instead of tuples
    as there is no non-ambigous elegant syntax for it  */
M_Object M_Module_wrap_Tuple(M_Expr expr){
    M_Object out;
    if(expr.len == 1){
        out = expr.data[0];
        expr.len--;

        M_Expr_clear(&expr);
        return out;
    }

    M_Object_set_expr(&out, expr);
    return out;
} 
M_Object M_Module_wrap_List(M_Expr expr){
    M_Object out;
    M_Object_set_expr(&out, expr);
    return out;
}
M_Object M_Module_parse_TupleExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace){
    return M_Module_parse_Group(pos, table, err_trace, '(', ')', "tuple", &M_Module_wrap_Tuple); 
}
M_Object M_Module_parse_ListExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace){
    return M_Module_parse_Group(pos, table, err_trace, '[', ']', "list", &M_Module_wrap_List); 
}

M_Object M_Module_parse_BlockExpr(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace){
    M_Module_Pos iter = *pos;
    iter.begin = iter.end;
    
    if(M_Module_Pos_peek(&iter) == '{'){
        M_Module_Pos_next(&iter);
    }
    else{
        M_Module_error_Group(err_trace, iter, '{', "block");
        return (M_Object){
            .type = M_TYPE_ERROR,
            .v_error = M_STATUS_PARSE_ERROR
        };
    }

    M_Expr expr;
    M_Expr_init(&expr, 2);

    while(true){
        M_Module_parse_Space(&iter);
        iter.begin = iter.end;

        if(M_Module_Pos_peek(&iter) == '}'){
            M_Module_Pos_next(&iter);
            M_Module_Pos_advance(pos, &iter);
            return M_Module_wrap_List(expr);
        }

        size_t error_count = err_trace->error_stack.len;
        M_Object tail = M_Module_parse_InfixExpr(&iter, table, err_trace);
        M_Expr_push(&expr, &tail, &iter);
    
        if(tail.type == M_TYPE_ERROR && iter.begin == iter.end){
            M_ErrorStack_revert(err_trace, error_count);
        }
        
        do {
            char chr = M_Module_Pos_peek(&iter);

            if(chr == ';'){
                M_Module_Pos_next(&iter);
                break;
            }
            if(chr == '}'){
                M_ErrorStack_pushLocMsg(err_trace, iter, 
                    "Expected terminator " M_COLOR ";" M_RESET
                    " after " M_COLOR "expr" M_RESET
                    " in " M_COLOR "block" M_RESET);    
                M_Module_Pos_next(&iter);
                M_Module_Pos_advance(pos, &iter);
                return M_Module_wrap_List(expr);
            }
            if(chr == '\0'){
                M_Module_error_Group(err_trace, iter, '}', "block");
                M_Module_Pos_advance(pos, &iter);
                return M_Module_wrap_List(expr);
            }
        } while(M_Module_Pos_next(&iter));
    }
}
M_Object M_Module_parse_File(M_Module_Pos* const pos, M_SymbolTable* const table, M_ErrorStack* const err_trace){
    M_Module_Pos iter = *pos;
    iter.begin = iter.end;
    
    M_Expr expr;
    M_Expr_init(&expr, 2);

    while(true){
        M_Module_parse_Space(&iter);
        iter.begin = iter.end;       

        if(M_Module_Pos_peek(&iter) == '\0'){    
            M_Module_Pos_advance(pos, &iter);
            return M_Module_wrap_List(expr);
        } 

        size_t error_count = err_trace->error_stack.len;
        M_Object tail = M_Module_parse_InfixExpr(&iter, table, err_trace);
        M_Expr_push(&expr, &tail, &iter);

        if(iter.begin == iter.end){
            M_ErrorStack_revert(err_trace, error_count);
        }

        M_Module_Pos sync_iter = iter;

        do {
            char chr = M_Module_Pos_peek(&sync_iter);

            if(chr == ';'){
                M_Module_Pos_next(&sync_iter);
                iter = sync_iter;
                break;
            }
            if(chr == '\0'){
                M_ErrorStack_pushLocMsg(err_trace, iter, 
                    "Expected terminator " M_COLOR ";" M_RESET
                    " after " M_COLOR "expr" M_RESET);                        
                M_Module_Pos_advance(pos, &sync_iter);
                return M_Module_wrap_List(expr);
            }
        } while(M_Module_Pos_next(&sync_iter));
    }
}