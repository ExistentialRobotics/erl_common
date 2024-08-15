#pragma once
#include "logging.hpp"

#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace erl::common {

    class NumberLogger {
        std::ofstream m_file_;
        std::string m_step_name_{};
        int m_step_idx_ = 0;
        bool m_append_ = false;
        bool m_on_screen_ = false;
        bool m_header_written_ = false;
        bool m_log_called_ = false;
        std::vector<std::string> m_column_names_{};
        std::unordered_map<std::string, double> m_value_sum_{};
        std::unordered_map<std::string, double> m_weight_sum_{};
        std::unordered_map<std::string, double> m_squared_value_sum_{};

    public:
        explicit NumberLogger(
            const std::string &filename = "",
            std::string step_name = "step",
            const int init_step_idx = 0,
            const bool append = false,
            const bool on_screen = false)
            : m_step_name_(std::move(step_name)),
              m_step_idx_(init_step_idx),
              m_append_(append),
              m_on_screen_(on_screen) {
            if (filename.empty()) { return; }
            m_file_.open(filename, append ? std::ios_base::app : std::ios_base::out);
        }

        ~NumberLogger() {
            if (!m_file_.is_open()) { return; }
            m_file_.flush();
            m_file_.close();
        }

        int
        GetStepIdx() const {
            return m_step_idx_;
        }

        void
        SetStepIdx(const int row_idx) {
            m_step_idx_ = row_idx;
        }

        void
        AddColumn(const std::string &column_name) {
            ERL_ASSERTM(!m_header_written_ && !m_log_called_, "AddColumn must be called before Log and Print");
            m_column_names_.push_back(column_name);
            m_value_sum_[column_name] = 0.0;
            m_weight_sum_[column_name] = 0.0;
            m_squared_value_sum_[column_name] = 0.0;
        }

        void
        AddColumns(const std::vector<std::string> &column_names) {
            for (const std::string &column_name: column_names) { AddColumn(column_name); }
        }

        void
        Log(const std::string &column_name, const double value, const double weight = 1.0) {
            ERL_WARN_COND(std::isnan(value) || std::isinf(value), "Value is NaN or Inf");
            ERL_WARN_COND(std::isnan(weight) || std::isinf(weight), "Weight is NaN or Inf");

            ERL_DEBUG_ASSERT(!std::isnan(value) && !std::isinf(value), "Value is NaN or Inf");
            ERL_DEBUG_ASSERT(!std::isnan(weight) && !std::isinf(weight), "Weight is NaN or Inf");

            m_log_called_ = true;
            ERL_ASSERTM(m_value_sum_.find(column_name) != m_value_sum_.end(), "Column name not found, call AddColumn first");
            m_value_sum_[column_name] += value * weight;
            m_weight_sum_[column_name] += weight;
            m_squared_value_sum_[column_name] += value * value * weight;
        }

        void
        Log(const std::vector<std::tuple<std::string, double, double>> &values) {
            for (const auto &[column_name, value, weight]: values) { Log(column_name, value, weight); }
        }

        void
        Log(const std::vector<std::pair<std::string, double>> &values) {
            for (const auto &[column_name, value]: values) { Log(column_name, value); }
        }

        std::vector<std::tuple<std::string, double, double>>
        GetStatistics() const {
            std::vector<std::tuple<std::string, double, double>> statistics;
            for (const std::string &column_name: m_column_names_) {
                ERL_WARN_COND(m_weight_sum_.at(column_name) == 0.0, "Weight sum is zero for column {}", column_name);
                const double mean = m_value_sum_.at(column_name) / m_weight_sum_.at(column_name);
                const double std = std::sqrt(m_squared_value_sum_.at(column_name) / m_weight_sum_.at(column_name) - mean * mean);
                statistics.emplace_back(column_name, mean, std);
            }
            return statistics;
        }

        std::vector<std::tuple<std::string, double, double>>
        Print() {
            auto statistics = GetStatistics();
            if (m_file_.is_open()) {
                if (!m_header_written_) {
                    ERL_ASSERTM(!m_column_names_.empty(), "AddColumn must be called before Print");
                    if (!m_append_) {  // write header only if not appending
                        m_file_ << m_step_name_;
                        for (const std::string &column_name: m_column_names_) { m_file_ << "," << column_name << "_mean," << column_name << "_std"; }
                        m_file_ << std::endl;
                    }
                    m_header_written_ = true;
                }
                m_file_ << m_step_idx_;
                for (const auto &[column_name, mean, std]: statistics) { m_file_ << "," << mean << "," << std; }
                m_file_ << std::endl;
                m_file_.flush();
            }
            if (m_on_screen_) {
                std::stringstream ss;
                constexpr int min_column_width = 10;
                const int step_column_width = std::max(static_cast<int>(m_step_name_.size()) + 4, min_column_width);
                ss << fmt::format("|{:^{}}|", m_step_name_, step_column_width);

                std::vector<std::pair<int, int>> column_widths;
                for (const std::string &column_name: m_column_names_) {
                    const int mean_column_width = std::max(static_cast<int>(column_name.size()) + 9, min_column_width);
                    const int std_column_width = std::max(static_cast<int>(column_name.size()) + 8, min_column_width);
                    column_widths.emplace_back(mean_column_width, std_column_width);
                    ss << fmt::format("{:^{}}|{:^{}}|", column_name + "_mean", mean_column_width, column_name + "_std", std_column_width);
                }
                ss << std::endl;

                ss << fmt::format("|{:^{}}|", m_step_idx_, min_column_width);
                for (std::size_t i = 0; i < column_widths.size(); ++i) {
                    const auto &[mean_column_width, std_column_width] = column_widths[i];
                    const auto &[column_name, mean, std] = statistics[i];
                    ss << fmt::format("{:^{}}|{:^{}}|", fmt::format("{:.2e}", mean), mean_column_width, fmt::format("{:.2e}", std), std_column_width);
                }
                ss << std::endl;
                ss << "|" << std::string(step_column_width, '-');
                for (const auto &[mean_column_width, std_column_width]: column_widths) {
                    ss << "|" << std::string(mean_column_width, '-');
                    ss << "|" << std::string(std_column_width, '-');
                }
                ss << "|";
                if (ProgressBar::GetNumBars() == 0) { ss << std::endl; }
                ProgressBar::Write(ss.str());
            }
            ++m_step_idx_;
            ResetStatistics();
            return statistics;
        }

        void
        ResetStatistics() {
            for (const auto &column_name: m_column_names_) {
                m_value_sum_[column_name] = 0.0;
                m_weight_sum_[column_name] = 0.0;
                m_squared_value_sum_[column_name] = 0.0;
            }
        }
    };

}  // namespace erl::common
