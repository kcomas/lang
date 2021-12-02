
#include "test.h"

TEST(basic_pass) {
    TEST_PASS();
}

TEST(basic_fail) {
    TEST_FAIL();
}

TEST(basic_invalid) {
    TEST_INVALID();
}

INIT_TESTS(
    ADD_TEST(basic_pass);
    ADD_TEST(basic_fail);
    ADD_TEST(basic_invalid);
);
