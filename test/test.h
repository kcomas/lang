
#pragma once

#include <stdio.h>
#include <stdlib.h>

#define TEST_STATUS_PFX(NAME) TEST_STATUS_##NAME

typedef enum {
    TEST_STATUS_PFX(WAIT),
    TEST_STATUS_PFX(PASS),
    TEST_STATUS_PFX(FAIL)
} test_status;

typedef struct _test_item {
    test_status status;
    const char *name;
    test_status (*fn)(void);
    struct _test_item *next;
} test_item;

inline test_item *item_init(const char *name, test_status (*fn)(void)) {
    test_item *ti = calloc(1, sizeof(test_item));
    ti->name = name;
    ti->fn = fn;
    return ti;
}

inline void item_free(test_item *ti) {
    free(ti);
}

typedef struct {
    size_t total, passed, failed, invalid;
    test_item *head, *tail;
} test_queue;

inline test_queue *queue_init(void) {
    return calloc(1, sizeof(test_queue));
}

inline void queue_add(test_queue *tq, test_item *ti) {
    tq->total++;
    if (tq->head == NULL) {
        tq->head = ti;
        tq->tail = tq->head;
    } else {
        tq->tail->next = ti;
        tq->tail = tq->tail->next;
    }
}

inline void queue_free(test_queue *tq) {
    test_item *head = tq->head;
    while (head != NULL) {
        test_item *tmp = head;
        head = head->next;
        item_free(tmp);
    }
    free(tq);
}

#define TEST_PASS() return TEST_STATUS_PFX(PASS)

#define TEST_FAIL() return TEST_STATUS_PFX(FAIL)

#define TEST_INVALID() return TEST_STATUS_PFX(WAIT)

#define TEST(NAME) test_status NAME(void)

#define ADD_TEST(NAME) queue_add(tq, item_init(#NAME, NAME))

#define INIT_TESTS(BODY) const char *_register_tests(test_queue *tq) { \
    BODY \
    return __FILE__; \
}
