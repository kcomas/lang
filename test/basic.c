
#include "test.h"

TEST(basic_pass) {
    // passes by default
}

TEST(basic_fail) {
    TEST_FAIL();
}

INIT_TESTS(
    ADD_TEST(basic_pass);
    ADD_TEST(basic_fail);
)
