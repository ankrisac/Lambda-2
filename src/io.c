#include "io.h"

#ifdef __linux__
#include "unistd.h"
#include <linux/limits.h>

void M_IO_cin(M_Array* const input){
    size_t buff_len = 16;
    char* buff = M_malloc(buff_len, sizeof* buff);

    char chr;

    size_t len = 0;
    while((chr = fgetc(stdin)) != EOF && len < 1024){
        if(len >= buff_len){
            buff_len *= 4;
            buff = M_realloc(buff, buff_len, sizeof* buff);
        }
        
        if(chr == '\n'){
            break;
        }
        buff[len++] = chr; 
    }
    
    input->reserve_len = buff_len;
    input->len = len;
    
    input->type = M_TYPE_CHAR;
    input->data_char = buff;
}
M_Status M_FileIO_loadfile(M_Array* const path, M_Array* const output){
    if(path->type != M_TYPE_CHAR){
        return M_STATUS_TYPE_MISMATCH;
    }

    __M_Array_push_Char(path, '\0');
    FILE* file = fopen(path->data_char, "r");
    M_Array_drop(path);

    if(file == NULL){
        return M_STATUS_FILE_ERROR;
    }

    M_Array_init(output, M_TYPE_CHAR, 32);

    char chr;
    while((chr = fgetc(file)) != EOF){
        __M_Array_push_Char(output, chr);
    }
    fclose(file);

    return M_STATUS_OK;
}
M_Status M_FileIO_cwd(M_Array* const path){
    size_t buff_len = PATH_MAX;
    char* buff = M_malloc(buff_len, sizeof* buff);

    if(getcwd(buff, buff_len) == NULL){
        return M_STATUS_FILE_ERROR;
    }

    path->reserve_len = buff_len;
    path->len = strlen(buff);
    path->data_char = buff;
    path->type = M_TYPE_CHAR;
    
    return M_STATUS_OK;
}
M_Status M_FileIO_chdir(M_Array* const path){
    if(path->type != M_TYPE_CHAR){
        return M_STATUS_TYPE_MISMATCH;
    }

    __M_Array_push_Char(path, '\0');

    if(chdir(path->data_char) != 0){
        M_Array_drop(path);
        return M_STATUS_FILE_ERROR;
    }

    M_Array_drop(path);
    return M_STATUS_OK;
}

void M_File_join_path(M_Array* const output, const M_Array* const parent, const M_Array* const child){
    M_Array_copy(output, parent);
    __M_Array_push_Char(output, '/');
    M_Array_join(output, child);
}

#else
#warning NON LINUX IO FUNCTIONS ARE PLACEHOLDERS
void M_IO_cin(M_Array* const input){
    M_PANIC("Unimplemented!");
}
M_Status M_FileIO_loadfile(M_Array* const path, M_Array* const output){
    M_PANIC("Unimplemented!");  
    return M_STATUS_COMPILER_ERROR;
}
M_Status M_FileIO_cwd(M_Array* const path){
    M_PANIC("Unimplemented!");  
    return M_STATUS_COMPILER_ERROR;
}
M_Status M_FileIO_chdir(M_Array* const path){
    M_PANIC("Unimplemented!");  
    return M_STATUS_COMPILER_ERROR;
}
void M_File_join_path(M_Array* const output, const M_Array* const parent, const M_Array* const child){
    M_PANIC("Unimplemented!");  
}
#endif