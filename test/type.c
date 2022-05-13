
#include "test.h"
#include "../src/type.h"

TEST(sym_tbl_init_free) {
    type_sym_tbl *tbl = type_sym_tbl_init(TYPE_SYM_TBL_BUCKETS);
    type_sym_tbl_free(tbl);
}

TEST(sym_tbl_insert_find) {
    type_sym_tbl *tbl = type_sym_tbl_init(TYPE_SYM_TBL_BUCKETS);
    type_sym_tbl_item *entry;
    type_sym_tbl_status status;
    const char *const v_name = "test";
    size_t len = strlen(v_name);
    status = type_sym_tbl_insert(&tbl, &entry, VAR_GROUP_PFX(LOCAL), len, v_name);
    if (status != TYPE_SYM_TBL_STATUS_PFX(ADDED)) TEST_FAIL();
    if (entry->group != VAR_GROUP_PFX(LOCAL)) TEST_FAIL();
    if (entry->len != len) TEST_FAIL();
    if (strcmp(v_name, entry->v_name) != 0) TEST_FAIL();
    if (tbl->used != 1) TEST_FAIL();
    entry = NULL;
    status = type_sym_tbl_find(&tbl, &entry, VAR_GROUP_PFX(LOCAL), len, v_name);
    if (status != TYPE_SYM_TBL_STATUS_PFX(FOUND)) TEST_FAIL();
    if (entry->group != VAR_GROUP_PFX(LOCAL)) TEST_FAIL();
    if (entry->len != len) TEST_FAIL();
    if (strcmp(v_name, entry->v_name) != 0) TEST_FAIL();
    const char *const not_found = "asdf";
    status = type_sym_tbl_find(&tbl, &entry, VAR_GROUP_PFX(LOCAL), strlen(not_found), not_found);
    if (status != TYPE_SYM_TBL_STATUS_PFX(NOT_FOUND)) TEST_FAIL();
    type_sym_tbl_free(tbl);
}

INIT_TESTS(
    ADD_TEST(sym_tbl_init_free);
    ADD_TEST(sym_tbl_insert_find);
);
