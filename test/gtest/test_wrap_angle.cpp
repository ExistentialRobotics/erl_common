#include "erl_common/angle_utils.hpp"
#include "erl_common/test_helper.hpp"

TEST(ERL_COMMON, WrapAnglePi) {
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(0), 0);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(M_PI), -M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(-M_PI), -M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(2 * M_PI), 0);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(-2 * M_PI), 0);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(3 * M_PI), -M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(-3 * M_PI), -M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(3. / 4 * M_PI), 3. / 4 * M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(-3. / 4 * M_PI), -3. / 4 * M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(5. / 4 * M_PI), -3. / 4 * M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAnglePi(-5. / 4 * M_PI), 3. / 4 * M_PI);
}

TEST(ERL_COMMON, WrapAngleTwoPi) {
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(0), 0);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(M_PI), M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(-M_PI), M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(2 * M_PI), 0);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(-2 * M_PI), 0);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(3 * M_PI), M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(-3 * M_PI), M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(3. / 4 * M_PI), 3. / 4 * M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(-3. / 4 * M_PI), 5. / 4 * M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(5. / 4 * M_PI), 5. / 4 * M_PI);
    EXPECT_DOUBLE_EQ(erl::common::WrapAngleTwoPi(-5. / 4 * M_PI), 3. / 4 * M_PI);
}
