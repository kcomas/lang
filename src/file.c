
#include "file.h"

file_status file_to_c_string(const char *const filename, char **str) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) return FILE_STATUS_PFX(CANNOT_OPEN);
    struct stat sb;
    if (fstat(fd, &sb) == -1) return FILE_STATUS_PFX(CANNOT_STAT);
    *str = calloc(1, sizeof(char) * sb.st_size + 1); // null term
    if (read(fd, *str, sb.st_size) != sb.st_size) {
        free(*str);
        close(fd);
        return FILE_STATUS_PFX(CANNOT_READ);
    }
    close(fd);
    return FILE_STATUS_PFX(OK);
}
