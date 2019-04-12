#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "hash_map.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

TEST(SampleTest, test){
    KeyToInt* node = create_nodes();
    printf("%i", node->key);
}

TEST(SampleAgain, test){
    EXPECT_EQ(0, 0);
}
