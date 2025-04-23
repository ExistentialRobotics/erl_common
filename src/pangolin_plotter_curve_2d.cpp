#ifdef ERL_USE_PANGOLIN
    #include "erl_common/pangolin_plotter_curve_2d.hpp"

    #include "erl_common/logging.hpp"

    #include <utility>

namespace erl::common {

    PangolinPlotterCurve2D::PangolinPlotterCurve2D(
        std::shared_ptr<PangolinWindow> window,
        const std::string &plotter_name,
        const std::vector<std::string> &labels,
        const int plot_window_width,
        const float plot_x_init,
        const float plot_x_tick,
        const float plot_y_tick,
        const pangolin::Colour &bg_color,
        const pangolin::Colour &axis_color,
        const pangolin::Colour &tick_color)
        : m_window_(std::move(window)),
          m_plotter_(&m_data_log_, plot_x_init, plot_x_init + static_cast<float>(plot_window_width) * plot_x_tick, -1.0f, 1.0f, plot_x_tick, plot_y_tick),
          m_display_(m_window_->AddDisplay(plotter_name, m_plotter_)),
          m_log_buffer_(labels.size()),
          m_max_values_(plot_window_width, std::numeric_limits<float>::lowest()),
          m_min_values_(plot_window_width, std::numeric_limits<float>::max()) {

        m_data_log_.SetLabels(labels);
        m_plotter_.SetBounds(0.0, 1.0, 0.0, 1.0);
        m_plotter_.SetBackgroundColour(bg_color);
        m_plotter_.SetAxisColour(axis_color);
        m_plotter_.SetTickColour(tick_color);
        m_plotter_.ClearSeries();
        for (std::size_t i = 1; i < labels.size(); ++i) { m_plotter_.AddSeries("$0", "$" + std::to_string(i), pangolin::DrawingModeLine); }
    }

    void
    PangolinPlotterCurve2D::Append(const float x, const std::vector<float> &values) {
        ERL_ASSERTM(values.size() == m_log_buffer_.size() - 1, "Number of values does not match the number of series in the plotter");

        m_window_->Activate();
        m_display_.Activate();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // log data
        m_log_buffer_[0] = x;
        std::copy(values.begin(), values.end(), m_log_buffer_.begin() + 1);
        m_data_log_.Log(m_log_buffer_);

        // scroll along x-axis
        auto range = m_plotter_.GetView();
        m_plotter_.ScrollViewSmooth(x > range.x.max ? x - range.x.max : 0.0f, 0.0f);
        const auto [min, max] = std::minmax_element(values.begin(), values.end());
        m_min_values_[m_head_] = *min;
        m_max_values_[m_head_++] = *max;
        if (m_head_ == m_min_values_.size()) { m_head_ = 0; }

        // update y-axis range
        range = m_plotter_.GetView();
        range.y.min = *std::min_element(m_min_values_.begin(), m_min_values_.end());
        range.y.max = *std::max_element(m_max_values_.begin(), m_max_values_.end());
        const float d = (range.y.max - range.y.min) * 0.2f;
        range.y.min -= d;
        range.y.max += d;
        m_plotter_.SetViewSmooth(range);
    }
}  // namespace erl::common
#endif
