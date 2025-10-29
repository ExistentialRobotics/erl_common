#include "erl_common/random.hpp"

namespace erl::common {
    std::mt19937_64 g_random_engine{std::random_device()()};
}
