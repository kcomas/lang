
#include "test.h"

extern inline test_item *test_item_init(const char *name, test_status (*fn)(void));

extern inline void test_item_free(test_item *ti);

extern inline test_queue *test_queue_init(void);

extern inline void test_queue_add(test_queue *tq, test_item *ti);

extern inline void test_queue_free(test_queue *tq);

extern const char *_register_tests(test_queue *tq);

int main(void) {
    test_queue *tq = test_queue_init();
    const char *test_file = _register_tests(tq);
    printf("\e[1;97mTESTING:\e[0m %s\n", test_file);
    test_item *head = tq->head;
    while (head != NULL) {
        printf("\e[1;97mRUNNING:\e[0m %s ", head->name);
        test_status status = head->fn();
        if (status == TEST_STATUS_PFX(PASS)) {
            tq->passed++;
            printf("\e[1;32mPASSED\e[0m\n");
        } else if (status == TEST_STATUS_PFX(FAIL)) {
            tq->failed++;
            printf("\e[1;31mFAILED\e[0m\n");
        } else {
            tq->invalid++;
            printf("\e[1;35mINVALID\e[0m\n");
        }
        head = head->next;
    }
    printf("\e[1;97mFINISHED:\e[0m %s \e[1;36mTOTAL:\e[0m %lu ", test_file, tq->total);
    printf("\e[1;32mPASSED:\e[0m %lu \e[1;31mFAILED:\e[0m %lu ", tq->passed, tq->failed);
    printf("\e[1;35mINVALID:\e[0m %lu\n", tq->invalid);
    int ret = 0;
    if (tq->failed > 0 && tq->invalid > 0) ret = 1;
    else if (tq->failed > 0) ret = 2;
    else if (tq->invalid > 0) ret = 3;
    test_queue_free(tq);
    return ret;
}
