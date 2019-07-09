#include "../googletest/googletest/include/gtest/gtest.h"
#include "pattern.h"


TEST (PatternTest, LiteralTest) {
    // structure
    auto pattern = lit("class");
    ASSERT_EQ("class", pattern.value);
    ASSERT_EQ(5,       pattern.size);

    // match failure
    EXPECT_EQ(false,   pattern("Dog = class {}", 0));

    // match success
    EXPECT_EQ(true,    pattern("Dog = class {}", 6));
    EXPECT_EQ(6,       pattern.start);
    EXPECT_EQ(10,      pattern.end);}


// TEST (PatternTest, UntilTest) {
//     // structure
//     Pattern pattern = until(literal("class"));
//     ASSERT_EQ("class", pattern.value);
//     ASSERT_EQ(5,       pattern.size);

//     // match failure
//     EXPECT_EQ(false,   pattern("Dog = class {}", 6));

//     // match success
//     EXPECT_EQ(true,    pattern("Dog = class {}", 0));
//     EXPECT_EQ(0,       pattern.start));
//     EXPECT_EQ(6,       pattern.end));}

// }
