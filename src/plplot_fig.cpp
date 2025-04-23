#ifdef ERL_USE_PLPLOT
    #include "erl_common/plplot_fig.hpp"

namespace erl::common {

    const std::string PlplotFig::kDefaultAxisOpt = std::string(AxisOpt());

    PlplotFig::PlplotFig(const int width, const int height, const bool alpha_enabled)
        : m_alpha_enabled_(alpha_enabled),
          m_pls_(std::make_shared<plstream>()) {

        if (alpha_enabled) {
            m_cairo_img_ = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
        } else {
            m_cairo_img_ = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, width, height);
        }
        m_cairo_ctx_ = Cairo::Context::create(m_cairo_img_);

        m_pls_->sdev("extcairo");  // extcairo supports RGBA
        m_pls_->smema(width, height, m_cairo_img_->get_data());
        m_pls_->spage(0.0, 0.0, width, height, 0.0, 0.0);
        m_pls_->init();
        m_pls_->cmd(PLESC_DEVINIT, m_cairo_ctx_->cobj());
        m_pls_->adv(0);
    }

    cv::Mat
    PlplotFig::ToCvMat() const {
        return {
            m_cairo_img_->get_height(),
            m_cairo_img_->get_width(),
            m_alpha_enabled_ ? CV_8UC4 : CV_8UC3,
            m_cairo_img_->get_data(),
            static_cast<uint64_t>(m_cairo_img_->get_stride())};
    }

    int
    PlplotFig::Width() const {
        return m_cairo_img_->get_width();
    }

    int
    PlplotFig::Height() const {
        return m_cairo_img_->get_height();
    }

    PlplotFig &
    PlplotFig::Clear(const double r, const double g, const double b, const double a) {
        if (m_alpha_enabled_) {
            m_cairo_ctx_->set_source_rgba(r, g, b, a);
        } else {
            m_cairo_ctx_->set_source_rgb(r, g, b);
        }
        m_cairo_ctx_->paint();
        return *this;
    }

    PlplotFig &
    PlplotFig::DrawArc(
        const double center_x,
        const double center_y,
        const double radius_x,
        const double radius_y,
        const double angle_start,
        const double angle_end,
        const double rotate,
        const bool fill) {
        m_pls_->arc(center_x, center_y, radius_x, radius_y, angle_start, angle_end, rotate, fill);
        return *this;
    }

    PlplotFig::AxisOpt
    PlplotFig::AxisOpt::Off() {
        AxisOpt opt;
        opt.draw_axis = false;
        opt.draw_bottom_left_edge = false;
        opt.draw_top_right_edge = false;
        opt.draw_datetime_label = false;
        opt.use_fixed_point_numeric = false;
        opt.draw_grid_major = false;
        opt.draw_grid_minor = false;
        opt.invert_tick_markers = false;
        opt.log_scale = false;
        opt.draw_top_right_tick_labels = false;
        opt.draw_bottom_left_tick_labels = false;
        opt.use_custom_labeling_function = false;
        opt.draw_tick_minor = false;
        opt.draw_tick_major = false;
        return opt;
    }

    PlplotFig::AxisOpt::operator std::string() const {
        std::string result;
        if (draw_axis) { result += 'a'; }
        if (draw_bottom_left_edge) { result += 'b'; }
        if (draw_top_right_edge) { result += 'c'; }
        if (draw_datetime_label) { result += 'd'; }
        if (use_fixed_point_numeric) { result += 'f'; }
        if (draw_grid_major) { result += 'g'; }
        if (draw_grid_minor) { result += 'h'; }
        if (invert_tick_markers) { result += 'i'; }
        if (log_scale) { result += 'l'; }
        if (draw_top_right_tick_labels) { result += 'm'; }
        if (draw_bottom_left_tick_labels) { result += 'n'; }
        if (use_custom_labeling_function) { result += 'o'; }
        if (draw_tick_minor) { result += 's'; }
        if (draw_tick_major) { result += 't'; }
        if (draw_perpendicular_tick_labels) { result += 'v'; }
        return result;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawAxis(const bool draw) {
        draw_axis = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawBottomLeftEdge(const bool draw) {
        draw_bottom_left_edge = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawTopRightEdge(const bool draw) {
        draw_top_right_edge = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawDatetimeLabel(const bool draw) {
        draw_datetime_label = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::UseFixedPointNumeric(const bool fixed_point) {
        use_fixed_point_numeric = fixed_point;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawGridMajor(const bool draw) {
        draw_grid_major = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawGridMinor(const bool draw) {
        draw_grid_minor = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::InvertTickMarkers(const bool invert) {
        invert_tick_markers = invert;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::LogScale(const bool log) {
        log_scale = log;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawTopRightTickLabels(const bool draw) {
        draw_top_right_tick_labels = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawBottomLeftTickLabels(const bool draw) {
        draw_bottom_left_tick_labels = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::UseCustomLabelingFunction(const bool use) {
        use_custom_labeling_function = use;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawTickMinor(const bool draw) {
        draw_tick_minor = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawTickMajor(const bool draw) {
        draw_tick_major = draw;
        return *this;
    }

    PlplotFig::AxisOpt &
    PlplotFig::AxisOpt::DrawPerpendicularTickLabels(const bool draw) {
        draw_perpendicular_tick_labels = draw;
        return *this;
    }

    PlplotFig &
    PlplotFig::DrawAxes(
        const double origin_x,
        const double origin_y,
        const double tick_interval_x,
        const double tick_interval_y,
        const int num_subticks_x,
        const int num_subticks_y,
        const std::optional<AxisOpt> &axis_opt_x,
        const std::optional<AxisOpt> &axis_opt_y) {

        m_pls_->axes(
            origin_x,
            origin_y,
            axis_opt_x.has_value() ? std::string(axis_opt_x.value()).c_str() : kDefaultAxisOpt.c_str(),
            tick_interval_x,
            num_subticks_x,
            axis_opt_y.has_value() ? std::string(axis_opt_y.value()).c_str() : kDefaultAxisOpt.c_str(),
            tick_interval_y,
            num_subticks_y);
        return *this;
    }

    PlplotFig &
    PlplotFig::DrawAxesBox(
        const std::optional<AxisOpt> &axis_opt_x,
        const std::optional<AxisOpt> &axis_opt_y,
        const double tick_interval_x,
        const double tick_interval_y,
        const int num_subticks_x,
        const int num_subticks_y) {

        m_pls_->box(
            axis_opt_x.has_value() ? std::string(axis_opt_x.value()).c_str() : kDefaultAxisOpt.c_str(),
            tick_interval_x,
            num_subticks_x,
            axis_opt_y.has_value() ? std::string(axis_opt_y.value()).c_str() : kDefaultAxisOpt.c_str(),
            tick_interval_y,
            num_subticks_y);
        return *this;
    }

    PlplotFig &
    PlplotFig::DrawAxesBox3(
        const std::optional<AxisOpt> &axis_opt_x,
        const std::optional<AxisOpt> &axis_opt_y,
        const std::optional<AxisOpt> &axis_opt_z,
        const char *axis_label_x,
        const char *axis_label_y,
        const char *axis_label_z,
        const double tick_interval_x,
        const double tick_interval_y,
        const double tick_interval_z,
        const int num_subticks_x,
        const int num_subticks_y,
        const int num_subticks_z) {

        m_pls_->box3(
            axis_opt_x.has_value() ? std::string(axis_opt_x.value()).c_str() : kDefaultAxisOpt.c_str(),
            axis_label_x,
            tick_interval_x,
            num_subticks_x,
            axis_opt_y.has_value() ? std::string(axis_opt_y.value()).c_str() : kDefaultAxisOpt.c_str(),
            axis_label_y,
            tick_interval_y,
            num_subticks_y,
            axis_opt_z.has_value() ? std::string(axis_opt_z.value()).c_str() : kDefaultAxisOpt.c_str(),
            axis_label_z,
            tick_interval_z,
            num_subticks_z);
        return *this;
    }

    PlplotFig &
    PlplotFig::SetCurrentColor(Color0 color) {
        m_pls_->col0(static_cast<int>(color));  // 17.13
        return *this;
    }

    PlplotFig &
    PlplotFig::SetLabels(const char *x_label, const char *y_label, const char *title) {
        m_pls_->lab(x_label, y_label, title);  // 17.69
        return *this;
    }

    PlplotFig &
    PlplotFig::Legend(
        double &width_out,
        double &height_out,
        const int n_legend,
        const std::vector<const char *> &texts,
        const std::vector<int> &styles,
        const std::vector<int> &text_colors,
        const std::vector<int> &line_colors,
        const std::vector<int> &line_styles,
        const std::vector<double> &line_widths,
        const std::vector<const char *> &symbols,
        const std::vector<int> &symbol_colors,
        const std::vector<double> &symbol_scales,
        const std::vector<int> &symbol_numbers,
        const std::vector<int> &box_colors,
        const std::vector<int> &box_fill_patterns,
        const std::vector<double> &box_scales,
        const std::vector<double> &box_fill_line_widths,
        const int box_style,
        const int position,
        const double position_offset_x,
        const double position_offset_y,
        const double width,
        const int bg_color0,
        const int legend_box_line_color0,
        const int legend_box_line_style,
        const int n_row,
        const int n_col,
        const double text_offset,
        const double text_scale,
        const double text_spacing,
        const double text_justification) {

        m_pls_->legend(
            &width_out,
            &height_out,
            box_style,
            position,
            position_offset_x,
            position_offset_y,
            width,
            bg_color0,
            legend_box_line_color0,
            legend_box_line_style,
            n_row,
            n_col,
            n_legend,
            styles.data(),
            text_offset,
            text_scale,
            text_spacing,
            text_justification,
            text_colors.data(),
            texts.data(),
            box_colors.data(),
            box_fill_patterns.data(),
            box_scales.data(),
            box_fill_line_widths.data(),
            line_colors.data(),
            line_styles.data(),
            line_widths.data(),
            symbol_colors.data(),
            symbol_scales.data(),
            symbol_numbers.data(),
            symbols.data());
        return *this;
    }

    PlplotFig &
    PlplotFig::SetLineStyle(
        const int line_style,
        const std::optional<int> line_nms,
        const std::optional<int> line_mark_size,
        const std::optional<int> line_space_size) {
        if (line_nms.has_value()) {
            ERL_ASSERTM(line_mark_size.has_value(), "line_nms is set but line_mark_size is not.");
            ERL_ASSERTM(line_space_size.has_value(), "line_nms is set but line_space_size is not.");
            const int line_mark_size_ = line_mark_size.value();
            const int line_space_size_ = line_space_size.value();
            m_pls_->styl(line_nms.value(), &line_mark_size_, &line_space_size_);  // 17.155
        } else {
            m_pls_->lsty(line_style);  // 17.74
        }
        return *this;
    }

    PlplotFig &
    PlplotFig::DrawLine(const int num_points, const double *xs, const double *ys) {
        m_pls_->line(num_points, xs, ys);  // 17.72
        return *this;
    }

    PlplotFig &
    PlplotFig::DrawLine3(const int num_points, const double *xs, const double *ys, const double *zs) {
        m_pls_->line3(num_points, xs, ys, zs);  // 17.73
        return *this;
    }

    PlplotFig &
    PlplotFig::SetTitle(const char *title, const double displacement, const double pos, const double just) {
        m_pls_->mtex("t", displacement, pos, just, title);  // 17.84
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxisLabelX(const char *text, const bool top, const bool perpendicular, const double displacement, const double pos, const double just) {
        m_pls_->mtex(top ? (perpendicular ? "tv" : "t") : (perpendicular ? "bv" : "b"), displacement, pos, just, text);  // 17.84
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxisLabelY(const char *text, const bool right, const bool perpendicular, const double displacement, const double pos, const double just) {
        m_pls_->mtex(right ? (perpendicular ? "rv" : "r") : (perpendicular ? "lv" : "l"), displacement, pos, just, text);  // 17.84
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxis3LabelX(const char *text, const bool opposite, const bool perpendicular, const double displacement, const double pos, const double just) {
        m_pls_->mtex3(opposite ? (perpendicular ? "xsv" : "xs") : (perpendicular ? "xpv" : "xp"), displacement, pos, just, text);  // 17.85
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxis3LabelY(const char *text, const bool opposite, const bool perpendicular, const double displacement, const double pos, const double just) {
        m_pls_->mtex3(opposite ? (perpendicular ? "ysv" : "ys") : (perpendicular ? "ypv" : "yp"), displacement, pos, just, text);  // 17.85
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxis3LabelZ(const char *text, const bool opposite, const bool perpendicular, const double displacement, const double pos, const double just) {
        m_pls_->mtex3(opposite ? (perpendicular ? "zsv" : "zs") : (perpendicular ? "zpv" : "zp"), displacement, pos, just, text);  // 17.85
        return *this;
    }

    PlplotFig &
    PlplotFig::PutText(const char *text, const double x, const double y, const double dx, const double dy, const double just) {
        m_pls_->ptex(x, y, dx, dy, just, text);  // 17.97
        return *this;
    }

    PlplotFig &
    PlplotFig::PutText3(
        const char *text,
        const double wx,
        const double wy,
        const double wz,
        const double dx,
        const double dy,
        const double dz,
        const double sx,
        const double sy,
        const double sz,
        const double just,
        Color0 color) {
        m_pls_->col0(static_cast<int>(color));                          // 17.13
        m_pls_->ptex3(wx, wy, wz, dx, dy, dz, sx, sy, sz, just, text);  // 17.98
        return *this;
    }

    PlplotFig &
    PlplotFig::SetCurrentColor0(const int index, const int r, const int g, const int b, const double alpha) {
        m_pls_->scol0a(index, r, g, b, alpha);  // 17.113
        return *this;
    }

    PlplotFig &
    PlplotFig::SetFont(const int family, const int style, const int weight) {
        m_pls_->sfont(family, style, weight);  // 17.131
        return *this;
    }

    PlplotFig &
    PlplotFig::Subplots(const int num_cols, const int num_rows) {
        m_pls_->ssub(num_cols, num_rows);
        return *this;
    }

    PlplotFig &
    PlplotFig::Scatter(const int num_points, const double *xs, const double *ys, const char *marker_utf8) {
        m_pls_->string(num_points, xs, ys, marker_utf8);
        return *this;
    }

    PlplotFig &
    PlplotFig::Scatter3D(const int num_points, const double *xs, const double *ys, const double *zs, const char *marker_utf8) {
        m_pls_->string3(num_points, xs, ys, zs, marker_utf8);
        return *this;
    }

    PlplotFig &
    PlplotFig::SetMargin(const double left, const double right, const double bottom, const double top) {
        m_pls_->vpor(left, right, bottom, top);  // 17.169
        return *this;
    }

    PlplotFig &
    PlplotFig::SetDefaultMargin() {
        m_pls_->vsta();  // 17.170
        return *this;
    }

    PlplotFig &
    PlplotFig::SetPenWidth(const int width) {
        m_pls_->width(width);
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxisLimits(const double x_min, const double x_max, const double y_min, const double y_max) {
        m_pls_->wind(x_min, x_max, y_min, y_max);  // 17.173
        return *this;
    }
}  // namespace erl::common
#endif
