
#include "type.h"

extern inline type_sym_tbl_item *type_sym_tbl_item_init(var_group group, size_t len, const char *const v_name);

extern inline type_sym_tbl *type_sym_tbl_init(size_t size);

static size_t djb2(const char *str) {
    size_t hash = 5831;
    char c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}

type_sym_tbl_status _sym_table_findsert(type_sym_tbl **tbl, type_sym_tbl_item **entry, var_group group, size_t len, const char *const v_name, bool find_only, bool insert_only) {
    size_t hash = djb2(v_name);
    size_t idx, rehash = 0;
    type_sym_tbl_item *tmp = NULL;
    while (rehash < TYPE_SYM_TBL_REHASH) {
        tmp = (*tbl)->buckets[idx = (hash + rehash++) % (*tbl)->size];
        if (tmp == NULL || (len == tmp->len && (strcmp(v_name, tmp->v_name) == 0))) break; // not found || found
    }
    if (group != tmp->group) return TYPE_SYM_TBL_STATUS_PFX(VAR_GROUP_MISMATCH);
    if (tmp == NULL && find_only) return TYPE_SYM_TBL_STATUS_PFX(NOT_FOUND);
    if (tmp != NULL && insert_only) return TYPE_SYM_TBL_STATUS_PFX(ALLREADY_EXISTS);
    if (tmp != NULL) {
        *entry = tmp;
        return TYPE_SYM_TBL_STATUS_PFX(FOUND);
    }
    if (tmp == NULL && rehash == TYPE_SYM_TBL_REHASH) {
        // TODO resize and rebuild tbl
    }
}

extern inline type *type_init(type_name name, type_data data);
