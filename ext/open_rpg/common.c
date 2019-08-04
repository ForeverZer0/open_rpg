#include "./common.h"

void rpg_table_free(void *data) {
    RPGtable *table = DATA_PTR(data);
    if (table->data) {
        xfree(table->data);
    }
    xfree(data);
}

char *rpg_read_file(const char *fname, size_t *length) {
    char *buffer = NULL;
    FILE *file = fopen(fname, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long len = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer = xmalloc(len);
        if (buffer) {
            *length = fread(buffer, 1, len, file);
        }
        fclose(file);
    }
    return buffer;
}