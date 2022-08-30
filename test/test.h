
#pragma once

#include <stdio.h>
#include <stdlib.h>

#define TEST_STATUS_PFX(NAME) TEST_STATUS_##NAME

typedef enum {
    TEST_STATUS_PFX(PASS),
    TEST_STATUS_PFX(FAIL),
} test_status;

typedef struct _test_item test_item;

typedef void test_fn(test_item *ti);

typedef struct _test_item {
    test_status status;
    int fi_line; // at which the test failed
    const char *name;
    test_fn *fn; // current test passed in
    test_item *next;
} test_item;

inline test_item *test_item_init(const char *const name, test_fn *const fn) {
    test_item *ti = calloc(1, sizeof(test_item));
    ti->name = name;
    ti->fn = fn;
    return ti;
}

inline void test_item_free(test_item *const ti) {
    free(ti);
}

typedef struct {
    size_t total, passed, failed;
    test_item *head, *tail;
} test_queue;

inline test_queue *test_queue_init(void) {
    return calloc(1, sizeof(test_queue));
}

inline void test_queue_add(test_queue *const tq, test_item *const ti) {
    tq->total++;
    if (tq->head == NULL) tq->tail = tq->head = ti;
    else tq->tail = tq->tail->next = ti;
}

inline void test_queue_free(test_queue *const tq) {
    test_item *head = tq->head;
    while (head != NULL) {
        test_item *tmp = head;
        head = head->next;
        test_item_free(tmp);
    }
    free(tq);
}

#define TEST_END(STATUS) do { \
    ti->status = TEST_STATUS_PFX(STATUS); \
    ti->fi_line = __LINE__; \
    return; \
} while (0)

#define TEST_FAIL() TEST_END(FAIL)

#define TEST(NAME) void NAME(__attribute__((unused)) test_item *const ti)

#define ADD_TEST(NAME) test_queue_add(tq, test_item_init(#NAME, &NAME))

#define INIT_TESTS(BODY) const char *register_tests(test_queue *const tq) { \
    BODY \
    return __FILE__; \
}
