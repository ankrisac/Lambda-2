#pragma once
#include "core.h"

void M_IO_cin(M_Str* const self);

M_Status M_FileIO_cwd(M_Str* const path);
M_Status M_FileIO_chdir(M_Str* const path);
M_Status M_FileIO_loadfile(M_Str* const path, M_Str* const output);

void M_File_join_path(M_Str* const output, const M_Str* const parent, const M_Str* const child);