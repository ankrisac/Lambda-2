#pragma once
#include "core.h"

void M_IO_cin(M_Array* const self);

M_Status M_FileIO_cwd(M_Array* const path);
M_Status M_FileIO_chdir(M_Array* const path);
M_Status M_FileIO_loadfile(M_Array* const path, M_Array* const output);

void M_File_join_path(M_Array* const output, const M_Array* const parent, const M_Array* const child);