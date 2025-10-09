#pragma once

#include "compile_definitions.hpp"

#ifdef ERL_USE_PANGOLIN
    #include "pangolin_window.hpp"

    #include <pangolin/display/image_view.h>
    #include <pangolin/plot/plotter.h>

namespace erl::common {

    /**
     * A class to plot time series data in 2D using Pangolin.
     */
    class PangolinPlotterCurve2D {
        std::shared_ptr<PangolinWindow> m_window_;
        pangolin::DataLog m_data_log_;
        pangolin::Plotter m_plotter_;
        pangolin::View& m_display_;
        std::vector<float> m_log_buffer_;  // cppcheck-suppress unusedStructMember
        // used to keep track of the max and min values within the plot window
        std::vector<float> m_max_values_;
        std::vector<float> m_min_values_;
        std::size_t m_head_ = 0;

    public:
        PangolinPlotterCurve2D(
            std::shared_ptr<PangolinWindow> window,
            const std::string& plotter_name,
            const std::vector<std::string>& labels,
            int plot_window_width,
            float plot_x_init = 0.0f,
            float plot_x_tick = 0.1f,
            float plot_y_tick = 0.1f,
            const pangolin::Colour& bg_color = {0.0f, 0.0f, 0.0f, 1.0f},
            const pangolin::Colour& axis_color = {1.0f, 1.0f, 1.0f, 1.0f},
            const pangolin::Colour& tick_color = {0.5f, 0.5f, 0.5f, 1.0f});

        void
        Reset() {
            m_data_log_.Clear();
            std::fill(
                m_max_values_.begin(),
                m_max_values_.end(),
                std::numeric_limits<float>::lowest());
            std::fill(
                m_min_values_.begin(),
                m_min_values_.end(),
                std::numeric_limits<float>::max());
            m_head_ = 0;
        }

        std::shared_ptr<PangolinWindow>
        GetWindow() {
            return m_window_;
        }

        pangolin::DataLog&
        GetDataLog() {
            return m_data_log_;
        }

        pangolin::Plotter&
        GetPlotter() {
            return m_plotter_;
        }

        [[nodiscard]] pangolin::View&
        GetDisplay() const {
            return m_display_;
        }

        void
        Append(float x, const std::vector<float>& values);
    };

}  // namespace erl::common
#endif
