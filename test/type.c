
#include "test.h"
#include "../src/type.h"

TEST(sym_tbl_init_free) {
    type_sym_tbl *tbl = type_sym_tbl_init(TYPE_SYM_TBL_BUCKETS);
    type_sym_tbl_free(tbl);
}

static type_sym_tbl *build_test_tbl(const char *const entries[], size_t test_entries_len) {
    type_sym_tbl *tbl = type_sym_tbl_init(TYPE_SYM_TBL_BUCKETS);
    type_sym_tbl_item *entry;
    type_sym_tbl_status status;
    for (size_t i = 0; i < test_entries_len; i++) {
        const char *const v_name = entries[i];
        size_t len = strlen(v_name);
        status = type_sym_tbl_insert(&tbl, &entry, len, v_name);
        if (status != TYPE_SYM_TBL_STATUS_PFX(ADDED) || entry->len != len || strcmp(entry->v_name, v_name) != 0 || tbl->used != i + 1) {
            type_sym_tbl_free(tbl);
            return NULL;
        }
        entry = NULL;
        status = type_sym_tbl_find(&tbl, &entry, len, v_name);
        if (status != TYPE_SYM_TBL_STATUS_PFX(FOUND) || entry->len != len || strcmp(entry->v_name, v_name) != 0) {
            type_sym_tbl_free(tbl);
            return NULL;
        }
    }
    return tbl;
}

#define BUILD_TEST_TBL(entries) build_test_tbl(entries, sizeof(entries) / sizeof(entries[0]))

TEST(sym_tbl_not_found) {
    const char *const entries[] = { "test", "test2", "test3" };
    type_sym_tbl *tbl = BUILD_TEST_TBL(entries);
    if (tbl == NULL) TEST_FAIL();
    type_sym_tbl_item *entry;
    const char *const not_found = "asdf";
    if (type_sym_tbl_find(&tbl, &entry, strlen(not_found), not_found) != TYPE_SYM_TBL_STATUS_PFX(NOT_FOUND)) TEST_FAIL();
    type_sym_tbl_free(tbl);
}

TEST(sym_tbl_insert_fail) {
    const char *const entries[] = { "test", "test" };
    type_sym_tbl *tbl = BUILD_TEST_TBL(entries);
    if (tbl != NULL) TEST_FAIL();
}

INIT_TESTS(
    ADD_TEST(sym_tbl_init_free);
    ADD_TEST(sym_tbl_not_found);
    ADD_TEST(sym_tbl_insert_fail);
);
