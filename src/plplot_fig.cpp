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
            axis_opt_x.has_value() ? std::string(axis_opt_x.value()).c_str()
                                   : kDefaultAxisOpt.c_str(),
            tick_interval_x,
            num_subticks_x,
            axis_opt_y.has_value() ? std::string(axis_opt_y.value()).c_str()
                                   : kDefaultAxisOpt.c_str(),
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
            axis_opt_x.has_value() ? std::string(axis_opt_x.value()).c_str()
                                   : kDefaultAxisOpt.c_str(),
            tick_interval_x,
            num_subticks_x,
            axis_opt_y.has_value() ? std::string(axis_opt_y.value()).c_str()
                                   : kDefaultAxisOpt.c_str(),
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
            axis_opt_x.has_value() ? std::string(axis_opt_x.value()).c_str()
                                   : kDefaultAxisOpt.c_str(),
            axis_label_x,
            tick_interval_x,
            num_subticks_x,
            axis_opt_y.has_value() ? std::string(axis_opt_y.value()).c_str()
                                   : kDefaultAxisOpt.c_str(),
            axis_label_y,
            tick_interval_y,
            num_subticks_y,
            axis_opt_z.has_value() ? std::string(axis_opt_z.value()).c_str()
                                   : kDefaultAxisOpt.c_str(),
            axis_label_z,
            tick_interval_z,
            num_subticks_z);
        return *this;
    }

    PlplotFig &
    PlplotFig::SetCurrentColor(const Color0 color) {
        m_pls_->col0(color);  // 17.13
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetOptOrientation(const int opt_orientation_) {
        opt_orientation = opt_orientation_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetOptBackground(const bool opt_background_) {
        opt_background = opt_background_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetOptBoundingBox(const bool opt_bounding_box_) {
        opt_bounding_box = opt_bounding_box_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetOptType(const int opt_type_) {
        opt_type = opt_type_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetOptLabelPosition(const int opt_label_position_) {
        opt_label_position = opt_label_position_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetOptCap(const int opt_cap_) {
        opt_cap = opt_cap_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetPosition(const int position_) {
        position = position_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetPositionOffsetX(const double position_offset_x_) {
        position_offset_x = position_offset_x_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetPositionOffsetY(const double position_offset_y_) {
        position_offset_y = position_offset_y_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetWidth(const double width_) {
        width = width_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetHeight(const double height_) {
        height = height_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetBgColor0(const int bg_color0_) {
        bg_color0 = bg_color0_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetBboxLineColor0(const int bbox_line_color0_) {
        bbox_line_color0 = bbox_line_color0_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetBboxLineStyle(const int bbox_line_style_) {
        bbox_line_style = bbox_line_style_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetLowCapColor(const double low_cap_color_) {
        low_cap_color = low_cap_color_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetHighCapColor(const double high_cap_color_) {
        high_cap_color = high_cap_color_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetContourLineColor0(const int contour_line_color0_) {
        contour_line_color0 = contour_line_color0_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetContourLineWidth(const double contour_line_width_) {
        contour_line_width = contour_line_width_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetLabelOpts(const std::vector<int> &label_opts_) {
        label_opts = label_opts_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetLabelTexts(const std::vector<const char *> &label_texts_) {
        label_texts = label_texts_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetNumAxes(const int n_axes_) {
        n_axes = n_axes_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetAxisOpts(const std::vector<AxisOpt> &axis_opts_) {
        axis_opts = axis_opts_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetAxisTickIntervals(const std::vector<double> &axis_tick_intervals_) {
        axis_tick_intervals = axis_tick_intervals_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetAxisNumSubticks(const std::vector<int> &axis_num_subticks_) {
        axis_num_subticks = axis_num_subticks_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::SetColorMapValues(
        const std::vector<std::vector<double>> &cmap_values_) {
        cmap_values = cmap_values_;
        return *this;
    }

    PlplotFig::ColorBarOpt &
    PlplotFig::ColorBarOpt::AddColorMap(
        const int idx,
        const std::vector<double> &cmap_values_,
        const int stride) {
        if (idx >= static_cast<int>(cmap_values.size())) { cmap_values.resize(idx + 1); }
        auto &cmap_value = cmap_values[idx];
        cmap_value.clear();
        cmap_value.reserve(cmap_values_.size() / stride + 1);
        for (std::size_t i = 0; i < cmap_values_.size() + stride; i += stride) {
            cmap_value.push_back(cmap_values_[std::min(i, cmap_values_.size() - 1)]);
        }
        return *this;
    }

    PlplotFig &
    PlplotFig::ColorBar(double &width_out, double &height_out, ColorBarOpt &opt) {
        int opt_code = opt.opt_orientation | opt.opt_type | opt.opt_label_position | opt.opt_cap;
        if (opt.opt_background) { opt_code |= PL_COLORBAR_BACKGROUND; }
        if (opt.opt_bounding_box) { opt_code |= PL_COLORBAR_BOUNDING_BOX; }

        std::vector<std::string> axis_opts_strings;  // keep axis opt strings alive
        std::vector<const char *> axis_opts;
        axis_opts_strings.reserve(opt.axis_opts.size());
        axis_opts.reserve(opt.axis_opts.size());
        for (const auto &axis_opt: opt.axis_opts) {
            axis_opts_strings.emplace_back(std::string(axis_opt));
            axis_opts.push_back(axis_opts_strings.back().c_str());
        }

        ERL_ASSERTM(
            static_cast<int>(opt.cmap_values.size()) >= opt.n_axes,
            "cmap_values size is less than n_axes.");

        std::vector<int> n_values;
        std::vector<double *> values;
        n_values.reserve(opt.cmap_values.size());
        values.reserve(opt.cmap_values.size());
        for (auto &cmap_value: opt.cmap_values) {
            n_values.push_back(static_cast<int>(cmap_value.size()));
            values.push_back(cmap_value.data());
        }

        m_pls_->colorbar(
            &width_out,
            &height_out,
            opt_code,
            opt.position,
            opt.position_offset_x,
            opt.position_offset_y,
            opt.width,
            opt.height,
            opt.bg_color0,
            opt.bbox_line_color0,
            opt.bbox_line_style,
            opt.low_cap_color,
            opt.high_cap_color,
            opt.contour_line_color0,
            opt.contour_line_width,
            static_cast<int>(opt.label_texts.size()),
            opt.label_opts.data(),
            opt.label_texts.data(),
            static_cast<int>(opt.axis_opts.size()),
            axis_opts.data(),
            opt.axis_tick_intervals.data(),
            opt.axis_num_subticks.data(),
            n_values.data(),
            values.data());
        return *this;
    }

    PlplotFig &
    PlplotFig::ColorBar(ColorBarOpt &opt) {
        double width_out, height_out;
        return ColorBar(width_out, height_out, opt);
    }

    PlplotFig &
    PlplotFig::DrawContour(
        const double *data,
        const int n_rows,
        const int n_cols,
        double min_x,
        double max_x,
        double min_y,
        double max_y,
        const bool col_major,
        const std::vector<double> &levels) {
        std::vector<double *> z_ptrs;
        if (col_major) {
            z_ptrs.resize(n_cols);
            for (int i = 0; i < n_cols; ++i) {
                z_ptrs[i] = const_cast<double *>(data + i * n_rows);
            }
        } else {
            z_ptrs.resize(n_rows);
            for (int i = 0; i < n_rows; ++i) {
                z_ptrs[i] = const_cast<double *>(data + i * n_cols);
            }
        }
        PLcGrid2 grid;
        grid.nx = n_cols;
        grid.ny = n_rows;
        m_pls_->Alloc2dGrid(&grid.xg, grid.nx, grid.ny);
        m_pls_->Alloc2dGrid(&grid.yg, grid.nx, grid.ny);
        const double res_x = (max_x - min_x) / static_cast<double>(grid.nx - 1);
        const double res_y = (max_y - min_y) / static_cast<double>(grid.ny - 1);
        for (int i = 0; i < grid.nx; ++i) {
            const double x = min_x + static_cast<double>(i) * res_x;
            for (int j = 0; j < grid.ny; ++j) {
                grid.xg[i][j] = x;
                grid.yg[i][j] = min_y + static_cast<double>(j) * res_y;
            }
        }
        m_pls_->cont(
            z_ptrs.data(),
            n_rows,
            n_cols,
            1,
            n_rows,
            1,
            n_cols,
            levels.data(),
            static_cast<int>(levels.size()),
            plcallback::tr2,  // identical mapping
            &grid);
        m_pls_->Free2dGrid(grid.xg, grid.nx, grid.ny);
        m_pls_->Free2dGrid(grid.yg, grid.nx, grid.ny);
        return *this;
    }

    PlplotFig &
    PlplotFig::SetLabels(const char *x_label, const char *y_label, const char *title) {
        m_pls_->lab(x_label, y_label, title);  // 17.69
        return *this;
    }

    PlplotFig::LegendOpt::LegendOpt(int n_legend_, std::vector<const char *> texts_)
        : n_legend(n_legend_),
          texts(std::move(texts_)) {}

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetNumLegend(int n_legend_) {
        n_legend = n_legend_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetTexts(std::vector<const char *> texts_) {
        texts = std::move(texts_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetTextColors(std::vector<int> text_colors_) {
        text_colors = std::move(text_colors_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetStyles(std::vector<int> styles_) {
        styles = std::move(styles_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetLineColors(std::vector<int> line_colors_) {
        line_colors = std::move(line_colors_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetLineStyles(std::vector<int> line_styles_) {
        line_styles = std::move(line_styles_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetLineWidths(std::vector<double> line_widths_) {
        line_widths = std::move(line_widths_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetSymbols(std::vector<const char *> symbols_) {
        symbols = std::move(symbols_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetSymbolColors(std::vector<int> symbol_colors_) {
        symbol_colors = std::move(symbol_colors_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetSymbolScales(std::vector<double> symbol_scales_) {
        symbol_scales = std::move(symbol_scales_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetSymbolSizes(std::vector<int> symbol_sizes_) {
        symbol_sizes = std::move(symbol_sizes_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetBoxColors(std::vector<int> box_colors_) {
        box_colors = std::move(box_colors_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetBoxFillPatterns(std::vector<int> box_fill_patterns_) {
        box_fill_patterns = std::move(box_fill_patterns_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetBoxScales(std::vector<double> box_scales_) {
        box_scales = std::move(box_scales_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetBoxFillLineWidths(std::vector<double> box_fill_line_widths_) {
        box_fill_line_widths = std::move(box_fill_line_widths_);
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetBoxStyle(const int box_style_) {
        box_style = box_style_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetPosition(const int position_) {
        position = position_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetPositionOffsetX(const double position_offset_x_) {
        position_offset_x = position_offset_x_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetPositionOffsetY(const double position_offset_y_) {
        position_offset_y = position_offset_y_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetWidth(const double width_) {
        width = width_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetBgColor0(const int bg_color0_) {
        bg_color0 = bg_color0_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetLegendBoxLineColor0(int legend_box_line_color0_) {
        legend_box_line_color0 = legend_box_line_color0_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetLegendBoxLineStyle(int legend_box_line_style_) {
        legend_box_line_style = legend_box_line_style_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetNumRows(const int n_row_) {
        n_row = n_row_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetNumCols(const int n_col_) {
        n_col = n_col_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetTextOffset(const double text_offset_) {
        text_offset = text_offset_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetTextScale(const double text_scale_) {
        text_scale = text_scale_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetTextSpacing(const double text_spacing_) {
        text_spacing = text_spacing_;
        return *this;
    }

    PlplotFig::LegendOpt &
    PlplotFig::LegendOpt::SetTextJustification(const double text_justification_) {
        text_justification = text_justification_;
        return *this;
    }

    PlplotFig &
    PlplotFig::Legend(double &width_out, double &height_out, const LegendOpt &opt) {

        m_pls_->legend(
            &width_out,
            &height_out,
            opt.box_style,
            opt.position,
            opt.position_offset_x,
            opt.position_offset_y,
            opt.width,
            opt.bg_color0,
            opt.legend_box_line_color0,
            opt.legend_box_line_style,
            opt.n_row,
            opt.n_col,
            opt.n_legend,
            opt.styles.data(),
            opt.text_offset,
            opt.text_scale,
            opt.text_spacing,
            opt.text_justification,
            opt.text_colors.data(),
            opt.texts.data(),
            opt.box_colors.data(),
            opt.box_fill_patterns.data(),
            opt.box_scales.data(),
            opt.box_fill_line_widths.data(),
            opt.line_colors.data(),
            opt.line_styles.data(),
            opt.line_widths.data(),
            opt.symbol_colors.data(),
            opt.symbol_scales.data(),
            opt.symbol_sizes.data(),
            opt.symbols.data());
        return *this;
    }

    PlplotFig &
    PlplotFig::Legend(const LegendOpt &opt) {
        double width_out, height_out;
        return Legend(width_out, height_out, opt);
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
    PlplotFig::DrawLine3(
        const int num_points,
        const double *xs,
        const double *ys,
        const double *zs) {
        m_pls_->line3(num_points, xs, ys, zs);  // 17.73
        return *this;
    }

    PlplotFig &
    PlplotFig::SetTitle(
        const char *title,
        const double displacement,
        const double pos,
        const double just) {
        m_pls_->mtex("t", displacement, pos, just, title);  // 17.84
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxisLabelX(
        const char *text,
        const bool top,
        const bool perpendicular,
        const double displacement,
        const double pos,
        const double just) {
        m_pls_->mtex(
            top ? (perpendicular ? "tv" : "t") : (perpendicular ? "bv" : "b"),
            displacement,
            pos,
            just,
            text);  // 17.84
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxisLabelY(
        const char *text,
        const bool right,
        const bool perpendicular,
        const double displacement,
        const double pos,
        const double just) {
        m_pls_->mtex(
            right ? (perpendicular ? "rv" : "r") : (perpendicular ? "lv" : "l"),
            displacement,
            pos,
            just,
            text);  // 17.84
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxis3LabelX(
        const char *text,
        const bool opposite,
        const bool perpendicular,
        const double displacement,
        const double pos,
        const double just) {
        m_pls_->mtex3(
            opposite ? (perpendicular ? "xsv" : "xs") : (perpendicular ? "xpv" : "xp"),
            displacement,
            pos,
            just,
            text);  // 17.85
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxis3LabelY(
        const char *text,
        const bool opposite,
        const bool perpendicular,
        const double displacement,
        const double pos,
        const double just) {
        m_pls_->mtex3(
            opposite ? (perpendicular ? "ysv" : "ys") : (perpendicular ? "ypv" : "yp"),
            displacement,
            pos,
            just,
            text);  // 17.85
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAxis3LabelZ(
        const char *text,
        const bool opposite,
        const bool perpendicular,
        const double displacement,
        const double pos,
        const double just) {
        m_pls_->mtex3(
            opposite ? (perpendicular ? "zsv" : "zs") : (perpendicular ? "zpv" : "zp"),
            displacement,
            pos,
            just,
            text);  // 17.85
        return *this;
    }

    PlplotFig &
    PlplotFig::SetAreaFillPattern(AreaFillPattern pattern) {
        m_pls_->psty(pattern);
        return *this;
    }

    PlplotFig &
    PlplotFig::PutText(
        const char *text,
        const double x,
        const double y,
        const double dx,
        const double dy,
        const double just) {
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
        const Color0 color) {
        m_pls_->col0(color);                                            // 17.13
        m_pls_->ptex3(wx, wy, wz, dx, dy, dz, sx, sy, sz, just, text);  // 17.98
        return *this;
    }

    PlplotFig &
    PlplotFig::SetColorMap(const int map_index, const ColorMap cmap) {
        int num_colors = 16;  // default 0
        if (cmap == Default1) { num_colors = 6; }
        if (cmap == Jet) { num_colors = 128; }

        static const std::array<std::vector<int>, 3> cmap_default0 = {
            // clang-format off
            std::vector{0, 255, 255,   0, 127, 255, 245, 190, 165,   0, 138,   0,  64, 255, 250, 255},
            std::vector{0,   0, 255, 255, 255, 192, 222, 190,  42,   0,  43, 255, 224,   0, 128, 255},
            std::vector{0,   0,   0,   0, 212, 203, 179, 190,  42, 255, 226, 255, 208, 255, 114, 255},
            // clang-format on
        };

        static const std::array<std::vector<int>, 3> cmap_default1 = {
            // clang-format off
            std::vector{ 85, 17, 1, 5, 51, 255},
            std::vector{  0,  0, 0, 0,  0,   0},
            std::vector{255, 51, 5, 0,  0,   0},
            // clang-format on
        };
        static const std::array<std::vector<int>, 3> cmap_jet_r = {
            std::vector{128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240,
                        248, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 254, 246, 238, 230, 222, 214, 206, 198, 190, 182, 174, 166,
                        158, 150, 142, 134, 126, 118, 110, 102, 94,  86,  78,  70,  62,  54,  46,
                        38,  30,  22,  14,  6,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                        0,   0,   0,   0,   0,   0,   0,   0},
            std::vector{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                        0,   0,   8,   16,  24,  32,  40,  48,  56,  64,  72,  80,  88,  96,  104,
                        112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224,
                        232, 240, 248, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 252, 244, 236, 228, 220, 212, 204, 196, 188, 180,
                        172, 164, 156, 148, 140, 132, 124, 116, 108, 100, 92,  84,  76,  68,  60,
                        52,  44,  36,  28,  20,  12,  4,   0,   0,   0,   0,   0,   0,   0,   0,
                        0,   0,   0,   0,   0,   0,   0,   0},
            std::vector{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                        0,   0,   0,   2,   10,  18,  26,  34,  42,  50,  58,  66,  74,  82,  90,
                        98,  106, 114, 122, 130, 138, 146, 154, 162, 170, 178, 186, 194, 202, 210,
                        218, 226, 234, 242, 250, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                        255, 255, 255, 255, 255, 255, 255, 252, 244, 236, 228, 220, 212, 204, 196,
                        188, 180, 172, 164, 156, 148, 140, 128}};
        static const std::array<std::vector<int>, 3> cmap_jet = {
            std::vector(cmap_jet_r[0].rbegin(), cmap_jet_r[0].rend()),
            std::vector(cmap_jet_r[1].rbegin(), cmap_jet_r[1].rend()),
            std::vector(cmap_jet_r[2].rbegin(), cmap_jet_r[2].rend())};

        const std::vector<int> *rgb = cmap_default0.data();
        if (cmap == Default1) { rgb = cmap_default1.data(); }
        if (cmap == Jet) { rgb = cmap_jet.data(); }
        if (cmap == JetReversed) { rgb = cmap_jet_r.data(); }

        if (map_index == 0) {
            m_pls_->scmap0n(num_colors);
            m_pls_->scmap0(rgb[0].data(), rgb[1].data(), rgb[2].data(), num_colors);
        } else {
            m_pls_->scmap1n(num_colors);
            m_pls_->scmap1(rgb[0].data(), rgb[1].data(), rgb[2].data(), num_colors);
        }
        return *this;
    }

    PlplotFig &
    PlplotFig::SetFontSize(const double height_mm, const double scale) {
        m_pls_->schr(height_mm, scale);  // 17.102
        return *this;
    }

    PlplotFig &
    PlplotFig::SetCurrentColor0(
        const int index,
        const int r,
        const int g,
        const int b,
        const double alpha) {
        m_pls_->scol0a(index, r, g, b, alpha);  // 17.113
        return *this;
    }

    PlplotFig &
    PlplotFig::SetFont(const int family, const int style, const int weight) {
        m_pls_->sfont(family, style, weight);  // 17.131
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetDefinedCallback(PLDEFINED_callback const defined_callback_) {
        defined_callback = defined_callback_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetXMin(const double x_min_) {
        x_min = x_min_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetXMax(const double x_max_) {
        x_max = x_max_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetYMin(const double y_min_) {
        y_min = y_min_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetYMax(const double y_max_) {
        y_max = y_max_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetColorLevels(const std::vector<double> &color_levels_) {
        color_levels = color_levels_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetColorLevels(
        const double *z,
        const int n_rows,
        const int n_cols,
        const int n_levels) {
        double min_z = z[0], max_z = z[0];
        const int n = n_rows * n_cols;
        for (int i = 1; i < n; ++i) {
            min_z = std::min(min_z, z[i]);
            max_z = std::max(max_z, z[i]);
        }
        color_levels.clear();
        color_levels.reserve(n_levels);
        color_levels.push_back(min_z);
        for (int i = 1; i < n_levels; ++i) {
            color_levels.push_back(
                min_z +
                (max_z - min_z) * static_cast<double>(i) / static_cast<double>(n_levels - 1));
        }
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetFillWidth(const double fill_width_) {
        fill_width = fill_width_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetContourColor0(const int contour_color0_) {
        contour_color0 = contour_color0_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetContourLineWidth(const double contour_line_width_) {
        contour_line_width = contour_line_width_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetFillCallback(PLFILL_callback const fill_callback_) {
        fill_callback = fill_callback_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetRectangular(const bool rectangular_) {
        rectangular = rectangular_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetTransformCallback(PLTRANSFORM_callback const transform_callback_) {
        transform_callback = transform_callback_;
        return *this;
    }

    PlplotFig::ShadesOpt &
    PlplotFig::ShadesOpt::SetTransformData(PLPointer transform_data_) {
        transform_data = transform_data_;
        return *this;
    }

    PlplotFig &
    PlplotFig::Shades(
        const double *z,
        const int n_rows,
        const int n_cols,
        const bool col_major,
        const ShadesOpt &opt) {
        std::vector<double *> z_ptrs;
        if (col_major) {
            z_ptrs.resize(n_cols);
            for (int i = 0; i < n_cols; ++i) { z_ptrs[i] = const_cast<double *>(z + i * n_rows); }
        } else {
            z_ptrs.resize(n_rows);
            for (int i = 0; i < n_rows; ++i) { z_ptrs[i] = const_cast<double *>(z + i * n_cols); }
        }
        m_pls_->shades(
            z_ptrs.data(),
            n_rows,
            n_cols,
            opt.defined_callback,
            opt.x_min,
            opt.x_max,
            opt.y_min,
            opt.y_max,
            opt.color_levels.data(),
            static_cast<int>(opt.color_levels.size()),
            opt.fill_width,
            opt.contour_color0,
            opt.contour_line_width,
            opt.fill_callback,
            opt.rectangular,
            opt.transform_callback,
            opt.transform_data);
        return *this;
    }

    PlplotFig &
    PlplotFig::Subplots(const int num_cols, const int num_rows) {
        m_pls_->ssub(num_cols, num_rows);
        return *this;
    }

    PlplotFig &
    PlplotFig::Scatter(
        const int num_points,
        const double *xs,
        const double *ys,
        const char *marker_utf8) {
        m_pls_->string(num_points, xs, ys, marker_utf8);
        return *this;
    }

    PlplotFig &
    PlplotFig::Scatter3D(
        const int num_points,
        const double *xs,
        const double *ys,
        const double *zs,
        const char *marker_utf8) {
        m_pls_->string3(num_points, xs, ys, zs, marker_utf8);
        return *this;
    }

    PlplotFig &
    PlplotFig::SetMargin(
        const double left,
        const double right,
        const double bottom,
        const double top) {
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
    PlplotFig::SetAxisLimits(
        const double x_min,
        const double x_max,
        const double y_min,
        const double y_max) {
        m_pls_->wind(x_min, x_max, y_min, y_max);  // 17.173
        return *this;
    }
}  // namespace erl::common
#endif
