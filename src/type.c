
#include "type.h"

extern inline type_sym_tbl_item *type_sym_tbl_item_init(var_group group, size_t len, const char *const v_name);

extern inline void type_sym_tbl_item_free(type_sym_tbl_item *item);

extern inline type_sym_tbl *type_sym_tbl_init(size_t size);

extern inline void type_sym_tbl_free(type_sym_tbl *tbl);

static size_t djb2(const char *str) {
    size_t hash = 5831;
    char c;
    while ((c = *str++)) hash = (hash << 5) + hash + c;
    return hash;
}

type_sym_tbl_status _type_sym_tbl_findsert(type_sym_tbl **tbl, type_sym_tbl_item **entry, var_group group, size_t len, const char *const v_name, bool find_only, bool insert_only) {
    size_t hash = djb2(v_name), idx, addressing = 0;
    type_sym_tbl_item *tmp = NULL;
    while (addressing < TYPE_SYM_TBL_ADDRESSING) {
        tmp = (*tbl)->buckets[idx = (hash + addressing++) % (*tbl)->size];
        if (tmp == NULL || (len == tmp->len && strcmp(v_name, tmp->v_name) == 0)) break; // not found || found
    }
    if (tmp != NULL && group != tmp->group) return TYPE_SYM_TBL_STATUS_PFX(VAR_GROUP_MISMATCH);
    if (tmp == NULL && find_only) return TYPE_SYM_TBL_STATUS_PFX(NOT_FOUND);
    if (tmp != NULL && insert_only) return TYPE_SYM_TBL_STATUS_PFX(ALLREADY_EXISTS);
    if (tmp != NULL) {
        *entry = tmp;
        return TYPE_SYM_TBL_STATUS_PFX(FOUND);
    }
    if (tmp == NULL && addressing == TYPE_SYM_TBL_ADDRESSING) {
        // resize and rebuild tbl
        type_sym_tbl *new_tbl = type_sym_tbl_init((*tbl)->size * TYPE_SYM_TBL_RESIZE);
        for (size_t i = 0; i < (*tbl)->size; i++) {
            idx = addressing = 0;
            tmp = (*tbl)->buckets[i];
            if (tmp == NULL) continue;
            hash = djb2(tmp->v_name);
            while (addressing < TYPE_SYM_TBL_ADDRESSING) {
                if (new_tbl->buckets[idx = (hash + addressing++) % new_tbl->size] == NULL) {
                    new_tbl->buckets[idx] = tmp;
                    new_tbl->used++;
                    break;
                }
            }
            if (addressing == TYPE_SYM_TBL_ADDRESSING) {
                free(new_tbl); // delete new tbl
                return TYPE_SYM_TBL_STATUS_PFX(ADDRESSING_FALIED);
            }
        }
        free(*tbl); // don't run tbl_free old items moved not copied
        *tbl = new_tbl;
        return _type_sym_tbl_findsert(tbl, entry, group, len, v_name, find_only, insert_only); // add new item
    }
    tmp = type_sym_tbl_item_init(group, len, v_name);
    *entry = tmp;
    (*tbl)->buckets[idx] = tmp;
    (*tbl)->used++;
    return TYPE_SYM_TBL_STATUS_PFX(ADDED);
}

extern inline type_sym_tbl_status type_sym_tbl_findsert(type_sym_tbl **tbl, type_sym_tbl_item **entry, var_group group, size_t len, const char *const v_name);

extern inline type_sym_tbl_status type_sym_tbl_find(type_sym_tbl **tbl, type_sym_tbl_item **entry, var_group group, size_t len, const char *const v_name);

extern inline type_sym_tbl_status type_sym_tbl_insert(type_sym_tbl **tbl, type_sym_tbl_item **entry, var_group group, size_t len, const char *const v_name);

extern inline type *type_init(type_name name, type_data data);

void type_free(type *t) {
    // TODO free different types
    free(t);
}
