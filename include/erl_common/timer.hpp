#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

namespace erl::common {
    class Timer {

        std::atomic<bool> m_active_;
        bool m_triggered_ = false;
        std::optional<std::thread> m_timer_thread_;
        std::mutex m_mutex_;
        std::condition_variable m_cv_;

    public:
        Timer()
            : m_active_(false) {}

        Timer(const Timer &) = delete;
        Timer &
        operator=(const Timer &) = delete;
        Timer(Timer &&) = delete;
        Timer &
        operator=(Timer &&) = delete;

        ~Timer() { Stop(); }

        void
        Start(const int interval_ms, const std::function<void()> &callback) {
            if (m_timer_thread_.has_value()) { Reset(); }

            m_active_ = true;
            m_timer_thread_ = std::thread([interval_ms, callback, this]() {
                std::unique_lock<std::mutex> lock(m_mutex_);
                if (m_cv_.wait_for(lock, std::chrono::milliseconds(interval_ms), [this] {
                        return !m_active_;
                    })) {
                    return;  // Timer was stopped
                }
                callback();  // Execute the callback function
                m_triggered_ = true;
                m_active_ = false;
            });
        }

        void
        Stop() {
            if (!m_timer_thread_.has_value()) { return; }
            {
                const std::scoped_lock lock(m_mutex_);
                m_active_ = false;
            }
            m_cv_.notify_all();  // Notify the timer thread to stop
            if (m_timer_thread_->joinable()) {
                m_timer_thread_->join();
                m_timer_thread_ = std::nullopt;
            }
        }

        void
        Reset() {
            m_triggered_ = false;
            Stop();
        }

        [[nodiscard]] bool
        IsActive() const {
            return m_active_;
        }

        [[nodiscard]] bool
        IsTriggered() const {
            return m_triggered_;
        }
    };
}  // namespace erl::common
