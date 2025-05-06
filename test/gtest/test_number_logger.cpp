#include "erl_common/number_logger.hpp"
#include "erl_common/progress_bar.hpp"
#include "erl_common/random.hpp"
#include "erl_common/test_helper.hpp"

TEST(NumberLogger, Basic) {
    GTEST_PREPARE_OUTPUT_DIR();
    using namespace erl::common;
    NumberLogger logger(test_output_dir / "basic.csv", "row", /*append*/ false, /*on_screen*/ true);
    logger.AddColumn("x");
    logger.AddColumn("y");

    constexpr int num_rows = 100;
    std::normal_distribution dist;
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < 1000; ++j) {
            logger.Log("x", dist(g_random_engine));
            logger.Log("y", dist(g_random_engine) * 2 + 1.0);
        }
        logger.Print();
    }
}

TEST(NumberLogger, WithProgressBar) {
    GTEST_PREPARE_OUTPUT_DIR();
    using namespace erl::common;
    NumberLogger logger(
        test_output_dir / "with_progress_bar.csv",
        "epoch",
        /*append*/ false,
        /*on_screen*/ true);
    logger.AddColumn("x");
    logger.AddColumn("y");

    constexpr int num_rows = 100;
    const std::shared_ptr<ProgressBar> pbar = ProgressBar::Open();
    pbar->SetDescription("NumberLogger Basic Test");
    pbar->SetTotal(num_rows);

    std::normal_distribution dist;
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < 1000; ++j) {
            logger.Log("x", dist(g_random_engine));
            logger.Log("y", dist(g_random_engine) * 2 + 1.0);
        }
        logger.Print();
        pbar->Update();
    }
    pbar->Close();
}
