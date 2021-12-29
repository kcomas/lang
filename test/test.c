
#include "test.h"

extern inline test_item *test_item_init(const char *const name, test_fn fn);

extern inline void test_item_free(test_item *const ti);

extern inline test_queue *test_queue_init(void);

extern inline void test_queue_add(test_queue *const tq, test_item *const ti);

extern inline void test_queue_free(test_queue *const tq);

extern const char *register_tests(test_queue *const tq);

int main(void) {
    test_queue *tq = test_queue_init();
    const char *test_file = register_tests(tq);
    printf("\e[1;97mTESTING:\e[0m %s\n", test_file);
    test_item *head = tq->head;
    while (head != NULL) {
        printf("\e[1;97mRUNNING:\e[0m %s ", head->name);
        head->fn(head);
        switch (head->status) {
            case TEST_STATUS_PFX(PASS):
                tq->passed++;
                printf("\e[1;32mPASSED\e[0m\n");
                break;
            case TEST_STATUS_PFX(FAIL):
                tq->failed++;
                printf("\e[1;31mFAILED\e[0m \e[1;97mAT LINE:\e[0m %d\n", head->fi_line);
                break;
            case TEST_STATUS_PFX(INVALID):
                tq->invalid++;
                printf("\e[1;35mINVALID\e[0m \e[1;97mAT LINE:\e[0m %d\n", head->fi_line);
                break;
        }
        head = head->next;
    }
    printf("\e[1;97mFINISHED:\e[0m %s \e[1;36mTOTAL:\e[0m %lu ", test_file, tq->total);
    printf("\e[1;32mPASSED:\e[0m %lu \e[1;31mFAILED:\e[0m %lu ", tq->passed, tq->failed);
    printf("\e[1;35mINVALID:\e[0m %lu\n", tq->invalid);
    int status = TEST_STATUS_PFX(PASS);
    if (tq->failed > 0 && tq->invalid > 0) status = TEST_STATUS_PFX(FAIL) | TEST_STATUS_PFX(INVALID);
    else if (tq->failed > 0) status = TEST_STATUS_PFX(FAIL);
    else if (tq->invalid > 0) status = TEST_STATUS_PFX(INVALID);
    test_queue_free(tq);
    return status;
}
