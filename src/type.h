
#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TYPE_NAME_PFX(NAME) TYPE_NAME_##NAME

typedef enum {
    TYPE_NAME_PFX(UNKNOWN),
    TYPE_NAME_PFX(U0), // void
    TYPE_NAME_PFX(U1), // bool
    TYPE_NAME_PFX(U8),
    TYPE_NAME_PFX(U16),
    TYPE_NAME_PFX(U32),
    TYPE_NAME_PFX(U64),
    TYPE_NAME_PFX(I8),
    TYPE_NAME_PFX(I16),
    TYPE_NAME_PFX(I32),
    TYPE_NAME_PFX(I64),
    TYPE_NAME_PFX(F32),
    TYPE_NAME_PFX(F64),
    TYPE_NAME_PFX(STR),
    TYPE_NAME_PFX(MOD),
    TYPE_NAME_PFX(FN)
} type_name;

typedef struct _type type;

void type_free(type *t);

typedef struct {
    size_t len; // does not inc null term
    type *v_type;
    char v_name[]; // must be null termed
} type_sym_tbl_item;

inline type_sym_tbl_item *type_sym_tbl_item_init(size_t len, const char *const v_name) {
    type_sym_tbl_item *item = calloc(1, sizeof(type_sym_tbl_item) + sizeof(char) * len + sizeof(char)); // null byte
    item->len = len;
    strcpy(item->v_name, v_name);
    return item;
}

inline void type_sym_tbl_item_free(type_sym_tbl_item *item) {
    type_free(item->v_type);
    free(item);
}

#ifndef TYPE_SYM_TBL_BUCKETS
    #define TYPE_SYM_TBL_BUCKETS 10
#endif

#ifndef TYPE_SYM_TBL_ADDRESSING
    #define TYPE_SYM_TBL_ADDRESSING 5
#endif

#ifndef TYPE_SYM_TBL_RESIZE
    #define TYPE_SYM_TBL_RESIZE 2
#endif

typedef struct {
    size_t used, size;
    type_sym_tbl_item *buckets[]; // open address
} type_sym_tbl;

inline type_sym_tbl *type_sym_tbl_init(size_t size) {
    type_sym_tbl *tbl = calloc(1, sizeof(type_sym_tbl) + sizeof(type_sym_tbl_item*) * size);
    tbl->size = size;
    return tbl;
}

inline void type_sym_tbl_free(type_sym_tbl *tbl) {
    for (size_t i = 0; i < tbl->size; i++) if (tbl->buckets[i] != NULL) type_sym_tbl_item_free(tbl->buckets[i]);
    free(tbl);
}

#define TYPE_SYM_TBL_STATUS_PFX(NAME) TYPE_SYM_TBL_STATUS_##NAME

typedef enum {
    TYPE_SYM_TBL_STATUS_PFX(FOUND),
    TYPE_SYM_TBL_STATUS_PFX(ADDED),
    TYPE_SYM_TBL_STATUS_PFX(NOT_FOUND),
    TYPE_SYM_TBL_STATUS_PFX(ALLREADY_EXISTS),
    TYPE_SYM_TBL_STATUS_PFX(ADDRESSING_FALIED)
} type_sym_tbl_status;

// entry is the found item or the inserted item
type_sym_tbl_status _type_sym_tbl_findsert(type_sym_tbl **tbl, type_sym_tbl_item **entry, size_t len, const char *const v_name, bool find_only, bool insert_only);

inline type_sym_tbl_status type_sym_tbl_findsert(type_sym_tbl **tbl, type_sym_tbl_item **entry, size_t len, const char *const v_name) {
    return _type_sym_tbl_findsert(tbl, entry, len, v_name, false, false);
}

inline type_sym_tbl_status type_sym_tbl_find(type_sym_tbl **tbl, type_sym_tbl_item **entry, size_t len, const char *const v_name) {
    return _type_sym_tbl_findsert(tbl, entry, len, v_name, true, false);
}

inline type_sym_tbl_status type_sym_tbl_insert(type_sym_tbl **tbl, type_sym_tbl_item **entry, size_t len, const char *const v_name) {
    return _type_sym_tbl_findsert(tbl, entry, len, v_name, false, true);
}

typedef struct {
    size_t len; // exports len
    type_sym_tbl *tbl;
    type_sym_tbl_item *exports[];
} type_mod;

#define PARENT_TYPE_PFX(NAME) PARENT_TYPE_PFX_##NAME

typedef enum {
    PARENT_TYPE_PFX(MOD),
    PARENT_TYPE_PFX(FN)
} parent_type;

typedef struct _type_fn {
    parent_type pt;
    size_t len; // args len
    type *ret;
    union {
        type_mod *mod;
        struct _type_fn *fn;
    } parent;
    type_sym_tbl *tbl;
    type_sym_tbl_item *args[];
} type_fn;

typedef union {
    type_mod *mod;
    type_fn *fn;
} type_data;

typedef struct _type {
    type_name name;
    type_data data;
} type;

inline type *type_init(type_name name, type_data data) {
    type *t = calloc(1, sizeof(type));
    t->name = name;
    t->data = data;
    return t;
}
