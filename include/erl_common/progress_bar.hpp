#include <iostream>
#include "assert.hpp"

namespace erl::common {
    class ProgressBar {
        int m_progress_ = 0;
        int m_num_cycles_ = 0;
        int m_last_percentage_ = 0;
        bool m_do_show_bar_ = true;
        bool m_update_is_called_ = false;

        std::string m_done_ = "=";
        std::string m_todo_ = " ";
        std::string m_opening_bracket_ = "[";
        std::string m_closing_bracket_ = "]";

        std::ostream &m_os_ = std::cerr;

    public:
        inline ProgressBar() = default;

        inline ProgressBar(int n, bool show_bar, std::ostream &out)
            : m_num_cycles_(n),
              m_do_show_bar_(show_bar),
              m_os_(out) {}

        ProgressBar(const ProgressBar &) = delete;
        ProgressBar &
        operator=(const ProgressBar &) = delete;
        ProgressBar(ProgressBar &&) = delete;
        ProgressBar &
        operator=(ProgressBar &&) = delete;

        ~ProgressBar() { m_os_ << std::endl << std::flush; }

        inline void
        Reset() {
            m_progress_ = 0;
            m_last_percentage_ = 0;
            m_update_is_called_ = false;
        }

        inline void
        SetNumCycles(int n) {
            ERL_ASSERTM(n > 0, "n must be positive.");
            m_num_cycles_ = n;
        }

        inline void
        Update() {
            ERL_ASSERTM(m_num_cycles_ > 0, "Please set the number of cycles at first.");
            if (!m_update_is_called_) {
                if (m_do_show_bar_) {
                    m_os_ << m_opening_bracket_;
                    for (int _ = 0; _ < 50; _++) { m_os_ << m_todo_; }
                    m_os_ << m_closing_bracket_ << " 0%";
                } else
                    m_os_ << "0%";
            }

            m_update_is_called_ = true;

            // compute percentage, if did not change, do nothing and return
            int perc = m_progress_ * 100 / (m_num_cycles_ - 1);
            if (perc < m_last_percentage_) { return; }

            // update percentage each unit
            if (perc == m_last_percentage_ + 1) {
                // erase the correct  number of characters
                if (perc <= 10) {
                    m_os_ << "\b\b" << perc << '%';
                } else {
                    m_os_ << "\b\b\b" << perc << '%';
                }
            }

            if (m_do_show_bar_) {
                // update bar every ten units
                if (perc % 2 == 0) {
                    // erase closing bracket
                    m_os_ << std::string(m_closing_bracket_.size(), '\b');
                    // erase trailing percentage characters
                    if (perc < 10) {
                        m_os_ << "\b\b\b";
                    } else if (perc < 100) {
                        m_os_ << "\b\b\b\b";
                    } else if (perc == 100) {
                        m_os_ << "\b\b\b\b\b";
                    }

                    // erase 'todo_char'
                    for (int j = 0; j < 50 - (perc - 1) / 2; ++j) { m_os_ << std::string(m_todo_.size(), '\b'); }

                    // add one additional 'done_char'
                    if (perc == 0) {
                        m_os_ << m_todo_;
                    } else {
                        m_os_ << m_done_;
                    }

                    // refill with 'todo_char'
                    for (int j = 0; j < 50 - (perc - 1) / 2 - 1; ++j) { m_os_ << m_todo_; }

                    // readd trailing percentage characters
                    m_os_ << m_closing_bracket_ << ' ' << perc << '%';
                }
            }
            m_last_percentage_ = perc;
            ++m_progress_;
            m_os_ << std::flush;
        }
    };
}  // namespace erl::common
