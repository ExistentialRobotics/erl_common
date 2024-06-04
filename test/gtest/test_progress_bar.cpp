#include "erl_common/progress_bar.hpp"
#include "erl_common/test_helper.hpp"

#include <chrono>

TEST(ProgressBar, SingleBar) {
    using namespace erl::common;
    const auto setting = std::make_shared<ProgressBar::Setting>();
    setting->total = 100;
    setting->symbol_done = '|';
    setting->description = "single bar";
    std::cout << std::string(setting->line_width, '=') << std::endl;
    const std::shared_ptr<ProgressBar> bar = ProgressBar::Open(setting);
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    for (std::size_t i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        bar->Update();
    }
    bar->Update();
    bar->Close();
    // std::cout << std::endl;
}

TEST(ProgressBar, SingleBarLog) {
    using namespace erl::common;
    const auto setting = std::make_shared<ProgressBar::Setting>();
    setting->total = 100;
    setting->symbol_done = '|';
    setting->description = "single bar";
    std::cout << std::string(setting->line_width, '=') << std::endl;
    const std::shared_ptr<ProgressBar> bar = ProgressBar::Open(setting);
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    for (std::size_t i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        bar->Update(1, "progress: " + std::to_string(i) + "%\nHello!");
    }
    bar->Update();
    bar->Close();
    std::cout << std::endl;
}

TEST(ProgressBar, TwoBars) {
    using namespace erl::common;
    const auto setting1 = std::make_shared<ProgressBar::Setting>();
    setting1->total = 100;
    setting1->symbol_done = '|';
    setting1->description = "bar1";
    setting1->position = 1;
    const auto bar1 = ProgressBar::Open(setting1);
    const auto setting2 = std::make_shared<ProgressBar::Setting>();
    setting2->total = 50;
    setting2->symbol_done = '>';
    setting2->description = "bar2";
    setting2->position = 0;  // bar2 will be on top of bar1
    const auto bar2 = ProgressBar::Open(setting2);
    ProgressBar::Write(std::string(setting1->line_width, '='));
    for (std::size_t i = 0; i < 100; ++i) {
        for (std::size_t j = 0; j < 50; ++j) {
            bar2->Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        bar1->Update();
        bar2->Reset();
    }
    bar1->Close();
    bar2->Close();
    std::cout << std::endl;
}

TEST(ProgressBar, TwoBarsLog) {
    using namespace erl::common;
    const auto setting1 = std::make_shared<ProgressBar::Setting>();
    setting1->total = 100;
    setting1->symbol_done = '#';
    setting1->description = "bar1";
    setting1->position = 1;
    const auto bar1 = ProgressBar::Open(setting1);
    const auto setting2 = std::make_shared<ProgressBar::Setting>();
    setting2->total = 50;
    setting2->symbol_done = '>';
    setting2->description = "bar2";
    setting2->position = 0;  // bar2 will be on top of bar1
    const auto bar2 = ProgressBar::Open(setting2);
    std::cout << std::string(setting1->line_width, '=') << std::endl;
    for (std::size_t i = 0; i < 100; ++i) {
        for (std::size_t j = 0; j < 50; ++j) {
            bar2->Update(1, "progress2: " + std::to_string(j) + "%\nHello!");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        bar1->Print(">> progress1: " + std::to_string(i) + "%");
        bar1->Update();
        bar2->Reset();
    }
    bar1->Close();
    bar2->Close();
    std::cout << std::endl;
}

TEST(ProgressBar, DynamicDescription) {
    using namespace erl::common;
    const auto setting = std::make_shared<ProgressBar::Setting>();
    setting->total = 100;
    setting->symbol_done = '|';
    setting->description = "single bar";
    std::cout << std::string(setting->line_width, '=') << std::endl;
    const std::shared_ptr<ProgressBar> bar = ProgressBar::Open(setting);
    for (std::size_t i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        bar->SetDescription("progress: " + std::to_string(i) + "%");
        bar->Update();
    }
    bar->Close();
    std::cout << std::endl;
}
