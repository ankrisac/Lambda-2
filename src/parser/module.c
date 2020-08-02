#include "module.h"

bool M_Module_Pos_equal(const M_Module_Pos* const self, const M_Module_Pos* const other){
    return self == other || 
        (self->module == other->module
            && self->begin == other->begin 
            && self->end == other->end 
            && self->line == other->line);
}

void M_Module_loadfile(M_Module* const self, const M_Array* const path){
    M_Array_copy(&self->path, path);

    if(M_FileIO_loadfile(&self->path, &self->data) != M_STATUS_OK){
        printf("Failed to load file: [");
        M_Array_print(path);
        printf("]\n");
    }
}
void M_Module_loadstr(M_Module* const self, M_Array data, const M_Array* const virtual_path){
    M_Array_copy(&self->path, virtual_path);
    self->data = data;
}

void M_Module_clear(M_Module* const self){
    M_Array_clear(&self->path);
    M_Array_clear(&self->data);
}

void M_Module_print(M_Module* const self){
    printf("[");
    M_Array_print(&self->path);
    printf("]\n");
    M_Array_print(&self->data);
    printf("\n");
}

M_Module_Pos M_Module_getbegin(const M_Module* const self){
    return (M_Module_Pos){ .module = self, .begin = 0, .end = 0, .line = 1 }; 
}


M_Str M_Module_Pos_to_str(const M_Module_Pos mpos){
    //pos is not validated

    M_Str outln;
    M_Str_from_cstr(&outln, "in '" M_ITALIC);
    M_Str_join(&outln, &mpos.module->path);
    M_Str_join_cstr(&outln, M_RESET "':\n");

    size_t begin = mpos.begin;
    size_t end = mpos.end;
    size_t len = mpos.module->data.len;
    size_t line = mpos.line;

    const char* data = mpos.module->data.data_char;

    for(;0 < begin && begin < len; begin--){
        if(data[begin] == '\n'){
            begin++;
            break;
        }
    }

    for(;end < len && data[end] != '\n'; end++);

    for(size_t i = mpos.begin; i < mpos.end; i++){
        if(data[i] == '\n'){
            line--;
        }
    }
    
    M_Str_push(&outln, ' ');
    M_Str_join_int(&outln, line);
    M_Str_join_cstr(&outln, " | ");
    for(size_t i = begin; i < mpos.begin; i++){
        M_Str_push(&outln, data[i]);
    }

    M_Str_join_cstr(&outln, M_COLOR);
    for(size_t i = mpos.begin; i < mpos.end; i++){
        if(data[i] == '\n'){
            line++;   
            M_Str_join_cstr(&outln, M_RESET "\n");
            M_Str_push(&outln, ' ');
            M_Str_join_int(&outln, line);
            M_Str_join_cstr(&outln, " | " M_COLOR);
        }
        else{
            M_Str_push(&outln, data[i]);
        }
    }

    M_Str_join_cstr(&outln, M_RESET M_UNDERLINE);
    if(mpos.end < end){
        M_Str_push(&outln, data[mpos.end]);
    }

    M_Str_join_cstr(&outln, M_RESET);
    for(size_t i = mpos.end + 1; i < end; i++){
        M_Str_push(&outln, data[i]);
    }
    M_Str_push(&outln, '\n');
    return outln;
}
void M_Module_Pos_print(const M_Module_Pos mpos){
    M_Str str = M_Module_Pos_to_str(mpos);
    M_Array_print(&str);
    M_Array_clear(&str);
}

char M_Module_Pos_peek(const M_Module_Pos* const pos){
    if(pos->end < pos->module->data.len){
        return pos->module->data.data_char[pos->end];
    }
    return '\0';
}
char M_Module_Pos_peekahead(const M_Module_Pos* const pos, size_t n){
    if(pos->end + n < pos->module->data.len){
        return pos->module->data.data_char[pos->end + n];
    }
    return '\0';
}
char M_Module_Pos_peekbehind(const M_Module_Pos* const pos, size_t n){
    if(n < pos->end){
        return pos->module->data.data_char[pos->end - n];
    }
    return '\0';
}
char M_Module_Pos_pop(M_Module_Pos* const pos){
    if(pos->end < pos->module->data.len){
        char val = pos->module->data.data_char[pos->end];
        if(val == '\n'){
            pos->line++;
        }

        pos->end++;
        return val;
    }
    return '\0';
}
bool M_Module_Pos_next(M_Module_Pos* const pos){
    if(pos->end < pos->module->data.len){
        char val = pos->module->data.data_char[pos->end];
        if(val == '\n'){
            pos->line++;
        }

        pos->end++;
        return true;
    }
    return false;
}
bool M_Module_Pos_prev(M_Module_Pos* const pos){
    if(pos->begin < pos->end){
        pos->end--;

        char val = pos->module->data.data_char[pos->end];
        if(val == '\n'){
            pos->line--;
        }

        return true;
    }
    return false;
}
void M_Module_Pos_advance(M_Module_Pos* const pos, const M_Module_Pos* const src){
    pos->end = src->end;
    pos->line = src->line;
}

void M_ModuleList_init(M_ModuleList* const self){
    M_SymbolTable_init(&self->path_list);
    
    self->modules_len = 0;
    self->modules_reserve_len = 16;
    self->modules = M_malloc(self->modules_reserve_len, sizeof* self->modules);
}
void M_ModuleList_clear(M_ModuleList* const self){
    M_SymbolTable_clear(&self->path_list);
    
    free(self->modules);
}
