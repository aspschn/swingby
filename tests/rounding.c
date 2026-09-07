#include <swingby/swingby.h>

typedef float (*rounding_function_t)(float);

rounding_function_t _rounding_function(enum sb_rounding_policy policy)
{
    switch (policy) {
    case SB_ROUNDING_POLICY_ROUND:
        return sb_float_round;
    case SB_ROUNDING_POLICY_CEIL:
        return sb_float_ceil;
    case SB_ROUNDING_POLICY_FLOOR:
        return sb_float_floor;
    default:
        return sb_float_round;  // TODO: Cover all cases.
    }
}

static int test_rounding()
{
    enum sb_rounding_policy policy = SB_ROUNDING_POLICY_FLOOR;
    if (_rounding_function(policy)(1.25f) != 1.0f) {
        return 1;
    }

    if (_rounding_function(SB_ROUNDING_POLICY_CEIL)(1.1f) != 2.0f) {
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    return test_rounding();
}
