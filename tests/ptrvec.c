#include "../src/main.h"
#include "../src/ptrvec.h"

#include "../src/alloc.h"

#include "test.h"

int
main(void)
{
    struct ptrvec ptrvec;

    alloc_init();

    TEST_CHECK("ptrvec_init()");

    ptrvec_init(&ptrvec);

    TEST_ASSERT(IMPLIES(ptrvec.ptr == NULL, ptrvec.length == 0 &&
                        ptrvec.capacity == 0));

    TEST_PASS();

    TEST_TODO(Implement ptrvec tests);

    alloc_free();

    return 0;
}
