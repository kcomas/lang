
#pragma once

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_STATUS_PFX(NAME) FILE_STATUS_##NAME

typedef enum {
    FILE_STATUS_PFX(OK),
    FILE_STATUS_PFX(CANNOT_OPEN),
    FILE_STATUS_PFX(CANNOT_STAT),
    FILE_STATUS_PFX(CANNOT_READ)
} file_status;

file_status file_to_c_string(const char *const filename, char **str);
