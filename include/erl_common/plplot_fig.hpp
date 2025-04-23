#pragma once

#ifdef ERL_USE_PLPLOT
    #include "opencv.hpp"

    #include <cairomm/cairomm.h>
    #include <plplot/plstream.h>

    #include <memory>

namespace erl::common {
    /**
     * A class to handle PLplot operations to draw plots into an OpenCV image.
     * The API is designed to be similar to Matplotlib.
     * @note https://plplot.sourceforge.net/docbook-manual/plplot-5.15.0.pdf
     */
    class PlplotFig {
    public:
        struct AxisOpt {
            bool draw_axis = false;                       // a: draw x-axis or y-axis
            bool draw_bottom_left_edge = true;            // b: draw bottom (x) or left (y) edge
            bool draw_top_right_edge = false;             // c: draw top (x) or right (y) edge
            bool draw_datetime_label = false;             // d: draw labels as datetime
            bool use_fixed_point_numeric = false;         // f: use fixed point numeric labels
            bool draw_grid_major = false;                 // g: draw major grid
            bool draw_grid_minor = false;                 // h: draw minor grid
            bool invert_tick_markers = false;             // i: draw tick markers outwards
            bool log_scale = false;                       // l: label axis logarithmically but don't affect the data
            bool draw_top_right_tick_labels = false;      // m: draw tick labels on top (x) or right (y)
            bool draw_bottom_left_tick_labels = true;     // n: draw tick labels on the bottom (x) or left (y)
            bool use_custom_labeling_function = false;    // o: use custom labeling function
            bool draw_tick_minor = true;                  // s: draw minor tick marks
            bool draw_tick_major = true;                  // t: draw major tick marks
            bool draw_perpendicular_tick_labels = false;  // v: draw tick labels perpendicular to the axis

            // u: like "b" but don't draw edge line
            // w: like "c" but don't draw edge line
            // x: like "t" but don't draw tick marks

            AxisOpt() = default;  // "bnst"

            static AxisOpt
            Off();

            [[nodiscard]] explicit
            operator std::string() const;

            AxisOpt&
            DrawAxis(bool draw = true);

            AxisOpt&
            DrawBottomLeftEdge(bool draw = true);

            AxisOpt&
            DrawTopRightEdge(bool draw = true);

            AxisOpt&
            DrawDatetimeLabel(bool draw = true);

            AxisOpt&
            UseFixedPointNumeric(bool fixed_point = true);

            AxisOpt&
            DrawGridMajor(bool draw = true);

            AxisOpt&
            DrawGridMinor(bool draw = true);

            AxisOpt&
            InvertTickMarkers(bool invert = true);

            AxisOpt&
            LogScale(bool log = true);

            AxisOpt&
            DrawTopRightTickLabels(bool draw = true);

            AxisOpt&
            DrawBottomLeftTickLabels(bool draw = true);

            AxisOpt&
            UseCustomLabelingFunction(bool use = true);

            AxisOpt&
            DrawTickMinor(bool draw = true);

            AxisOpt&
            DrawTickMajor(bool draw = true);

            AxisOpt&
            DrawPerpendicularTickLabels(bool draw = true);
        };

    private:
        static const std::string kDefaultAxisOpt;
        bool m_alpha_enabled_ = false;
        std::shared_ptr<plstream> m_pls_ = nullptr;
        Cairo::RefPtr<Cairo::ImageSurface> m_cairo_img_ = {};
        Cairo::RefPtr<Cairo::Context> m_cairo_ctx_ = {};

    public:
        PlplotFig(int width, int height, bool alpha_enabled);

        [[nodiscard]] cv::Mat
        ToCvMat() const;

        [[nodiscard]] int
        Width() const;

        [[nodiscard]] int
        Height() const;

        PlplotFig&
        Clear(double r = 0, double g = 0, double b = 0, double a = 1);

        PlplotFig&
        DrawArc(double center_x, double center_y, double radius_x, double radius_y, double angle_start, double angle_end, double rotate, bool fill);

        /**
         *
         * @param origin_x x coordinate of the origin
         * @param origin_y y coordinate of the origin
         * @param tick_interval_x interval between ticks on x-axis, 0.0 for auto
         * @param tick_interval_y interval between ticks on y-axis, 0.0 for auto
         * @param num_subticks_x number of sub-ticks on x-axis, 0 for auto
         * @param num_subticks_y number of sub-ticks on y-axis, 0 for auto
         * @param axis_opt_x x-axis options
         * @param axis_opt_y y-axis options
         * @return
         */
        PlplotFig&
        DrawAxes(
            double origin_x,
            double origin_y,
            double tick_interval_x = 0.0,
            double tick_interval_y = 0.0,
            int num_subticks_x = 0,
            int num_subticks_y = 0,
            const std::optional<AxisOpt>& axis_opt_x = std::nullopt,
            const std::optional<AxisOpt>& axis_opt_y = std::nullopt);

        /**
         * Draw axes with box in 2D
         * @param axis_opt_x x-axis options
         * @param axis_opt_y y-axis options
         * @param tick_interval_x interval between ticks on x-axis, 0.0 for auto
         * @param tick_interval_y interval between ticks on y-axis, 0.0 for auto
         * @param num_subticks_x number of sub-ticks on x-axis, 0 for auto
         * @param num_subticks_y number of sub-ticks on y-axis, 0 for auto
         * @return
         */
        PlplotFig&
        DrawAxesBox(
            const std::optional<AxisOpt>& axis_opt_x = std::nullopt,
            const std::optional<AxisOpt>& axis_opt_y = std::nullopt,
            double tick_interval_x = 0.0,
            double tick_interval_y = 0.0,
            int num_subticks_x = 0,
            int num_subticks_y = 0);

        PlplotFig&
        DrawAxesBox3(
            const std::optional<AxisOpt>& axis_opt_x = std::nullopt,
            const std::optional<AxisOpt>& axis_opt_y = std::nullopt,
            const std::optional<AxisOpt>& axis_opt_z = std::nullopt,
            const char* axis_label_x = "",
            const char* axis_label_y = "",
            const char* axis_label_z = "",
            double tick_interval_x = 0.0,
            double tick_interval_y = 0.0,
            double tick_interval_z = 0.0,
            int num_subticks_x = 0,
            int num_subticks_y = 0,
            int num_subticks_z = 0);

        /**
         * Code of color map 0.
         */
        enum class Color0 {
            Black = 0,       // 0x000000
            Red = 1,         // 0xFF0000
            Yellow = 2,      // 0xFFFF00
            Green = 3,       // 0x00FF00
            Aquamarine = 4,  // 0x7FFFD4
            Pink = 5,        // ffc0cb
            Wheat = 6,       // f5deb3
            Gray = 7,        // bebebe
            Brown = 8,       // a52a2a
            Blue = 9,        // 0000ff
            Violet = 10,     // 8a2be2
            Cyan = 11,       // 00ffff
            Turquoise = 12,  // 40e0d0
            Magenta = 13,    // ff00ff
            Salmon = 14,     // fa8072
            White = 15,      // ffffff
        };

        /**
         *
         * @param color the color code
         * @return
         * @note 17.13
         */
        PlplotFig&
        SetCurrentColor(Color0 color);

        /**
         *
         * @param line_style 1: solid, 2: short-dash-short-gap, 3: long-dash-log-gap, 4: long-dash-short-gap, etc. max 8
         * @param line_nms the number of marks (dash) and spaces (gap) in a line, nms=0 for solid line
         * @param line_mark_size the size of marks (dash) in a line, unit is millimeter
         * @param line_space_size the size of spaces (gap) in a line, unit is millimeter
         * @return
         */
        PlplotFig&
        SetLineStyle(int line_style, std::optional<int> line_nms, std::optional<int> line_mark_size, std::optional<int> line_space_size);

        /**
         * Set labels quickly. Call SetAxisLabelX or SetAxisLabelY for more options.
         * @param x_label UTF-8 string of x-axis label
         * @param y_label UTF-8 string of y-axis label
         * @param title UTF-8 string of title
         * @return
         * @note 17.69
         */
        PlplotFig&
        SetLabels(const char* x_label, const char* y_label, const char* title);

        /**
         * @param width_out returned value of the normalized legend width.
         * @param height_out returned value of the normalized legend height.
         * @param n_legend number of legend entries.
         * @param texts legend text of each entry.
         * @param styles legend style of each entry, PL_LEGEND_COLOR_BOX | PL_LEGEND_LINE | PL_LEGEND_SYMBOL.
         * @param text_colors legend text color of each entry.
         * @param line_colors legend line color of each entry.
         * @param line_styles legend line style of each entry.
         * @param line_widths legend line width of each entry.
         * @param symbols legend symbol of each entry.
         * @param symbol_colors legend symbol color of each entry.
         * @param symbol_scales legend symbol scale of each entry.
         * @param symbol_numbers number of symbols to draw for each entry.
         * @param box_colors legend box color of each entry.
         * @param box_fill_patterns legend box fill-pattern of each entry. [0, 8].
         * @param box_scales legend box scale of each entry.
         * @param box_fill_line_widths legend box fill line width of each entry.
         * @param box_style legend box style.
         * @param position legend box position.
         * @param position_offset_x normalized position offset in x direction.
         * @param position_offset_y normalized position offset in y direction.
         * @param width normalized width of the legend box.
         * @param bg_color0 background color0 index of the legend box.
         * @param legend_box_line_color0 bounding box color of cmap0.
         * @param legend_box_line_style bounding box line style.
         * @param n_row number of rows in the legend box.
         * @param n_col number of columns in the legend box.
         * @param text_offset text offset from the legend box in units of character width.
         * @param text_scale character height scale for text annotation.
         * @param text_spacing vertical spacing in units of character height between text annotations.
         * @param text_justification text justification, 0: left, 0.5: center, 1: right.
         * @return
         * @note 17.70
         */
        PlplotFig&
        Legend(
            double& width_out,
            double& height_out,
            int n_legend,
            const std::vector<const char*>& texts,
            const std::vector<int>& styles = {},
            const std::vector<int>& text_colors = {},
            const std::vector<int>& line_colors = {},
            const std::vector<int>& line_styles = {},
            const std::vector<double>& line_widths = {},
            const std::vector<const char*>& symbols = {},
            const std::vector<int>& symbol_colors = {},
            const std::vector<double>& symbol_scales = {},
            const std::vector<int>& symbol_numbers = {},
            const std::vector<int>& box_colors = {},
            const std::vector<int>& box_fill_patterns = {},
            const std::vector<double>& box_scales = {},
            const std::vector<double>& box_fill_line_widths = {},
            int box_style = PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
            int position = PL_POSITION_RIGHT | PL_POSITION_TOP | PL_POSITION_INSIDE | PL_POSITION_VIEWPORT,
            double position_offset_x = 0.0,
            double position_offset_y = 0.0,
            double width = 0.1,
            int bg_color0 = 15,
            int legend_box_line_color0 = 1,
            int legend_box_line_style = 1,
            int n_row = 0,
            int n_col = 0,
            double text_offset = 1.0,
            double text_scale = 1.0,
            double text_spacing = 2.0,
            double text_justification = 1.0);

        /**
         * Draw a line in 2D.
         * @param num_points
         * @param xs x coordinates of the points
         * @param ys y coordinates of the points
         * @return
         * @note 17.72
         */
        PlplotFig&
        DrawLine(int num_points, const double* xs, const double* ys);

        /**
         * Draw a line in 3D.
         * @param num_points
         * @param xs x coordinates of the points
         * @param ys y coordinates of the points
         * @param zs z coordinates of the points
         * @return
         * @note 17.73
         */
        PlplotFig&
        DrawLine3(int num_points, const double* xs, const double* ys, const double* zs);

        PlplotFig&
        SetTitle(const char* title, double displacement = 2.0, double pos = 0.5, double just = 0.5);

        /**
         * Place x-axis label
         * @param text
         * @param top if true, place the label on the top side.
         * @param perpendicular if true, the string is perpendicular to the axis.
         * @param displacement unit is character height. positive value means outwards, negative value means inwards.
         * @param pos position of the reference point relative to the axis. 0: left (x) or bottom (y), 1: right (x) or top (y), others: interpolation.
         * @param just position of the string relative to the reference point. 0: left (x) or bottom (y), 1: right (x) or top (y), others: interpolation.
         * @return
         * @note 17.84
         */
        PlplotFig&
        SetAxisLabelX(const char* text, bool top = false, bool perpendicular = false, double displacement = 3.2, double pos = 0.5, double just = 0.5);

        /**
         * Place y-axis label
         * @param text
         * @param right if true, place the label on the right side.
         * @param perpendicular if true, the string is perpendicular to the axis.
         * @param displacement unit is character height. positive value means outwards, negative value means inwards.
         * @param pos position of the reference point relative to the axis. 0: left (x) or bottom (y), 1: right (x) or top (y), others: interpolation.
         * @param just position of the string relative to the reference point. 0: left (x) or bottom (y), 1: right (x) or top (y), others: interpolation.
         * @return
         * @note 17.84
         */
        PlplotFig&
        SetAxisLabelY(const char* text, bool right = false, bool perpendicular = false, double displacement = 5.0, double pos = 0.5, double just = 0.5);

        /**
         *
         * @param text
         * @param opposite if true, place the label on the opposite side.
         * @param perpendicular if true, the string is perpendicular to the axis.
         * @param displacement unit is character height. positive value means outwards, negative value means inwards.
         * @param pos the reference point relative to the axis. 0: min, 1: max, others: interpolation.
         * @param just the string position relative to the reference point. 0: left (x) or bottom (y), 1: right (x) or top (y), others: interpolation.
         * @return
         * @note 17.85
         */
        PlplotFig&
        SetAxis3LabelX(const char* text, bool opposite = false, bool perpendicular = false, double displacement = 5.0, double pos = 0.5, double just = 0.5);

        /**
         *
         * @param text
         * @param opposite if true, place the label on the opposite side.
         * @param perpendicular if true, the string is perpendicular to the axis.
         * @param displacement unit is character height. positive value means outwards, negative value means inwards.
         * @param pos the reference point relative to the axis. 0: left (x) or bottom (y), 1: right (x) or top (y), others: interpolation.
         * @param just the string position relative to the reference point. 0: left (x) or bottom (y), 1: right (x) or top (y), others: interpolation.
         * @return
         * @note 17.85
         */
        PlplotFig&
        SetAxis3LabelY(const char* text, bool opposite = false, bool perpendicular = false, double displacement = 5.0, double pos = 0.5, double just = 0.5);

        /**
         *
         * @param text
         * @param opposite if true, place the label on the opposite side.
         * @param perpendicular if true, the string is perpendicular to the axis.
         * @param displacement unit is character height. positive value means outwards, negative value means inwards.
         * @param pos the reference point relative to the axis. 0: min, 1: max, others: interpolation.
         * @param just the string position relative to the reference point. 0: left (x) or bottom (y), 1: right (x) or top (y), others: interpolation.
         * @return
         * @note 17.85
         */
        PlplotFig&
        SetAxis3LabelZ(const char* text, bool opposite = false, bool perpendicular = false, double displacement = 5.0, double pos = 0.5, double just = 0.5);

        /**
         * Place text inside the viewport. The string baseline is parallel to the line (x, y) -> (x+dx, y+dy).
         * @param text the text to be placed
         * @param x x coordinate of the reference point
         * @param y y coordinate of the reference point
         * @param dx dx coordinate of the incline vector
         * @param dy dy coordinate of the incline vector
         * @param just position of the string relative to (x, y). 0: (x, y) is the left of the string, 1: the right, others: interpolation.
         * @return
         * @note 17.97
         */
        PlplotFig&
        PutText(const char* text, double x, double y, double dx = 0, double dy = 0, double just = 0);

        /**
         * Place text inside the viewport of a 3D plot. The string baseline is parallel to the line (wx, wy, wz) -> (wx+dx, wy+dy, wz+dz).
         * The string is vertically parallel to the line (wx, wy, wz) -> (wx+sx, wy+sy, wz+sz).
         * @param text the text to be placed
         * @param wx world x coordinate of the reference point
         * @param wy world y coordinate of the reference point
         * @param wz world z coordinate of the reference point
         * @param dx x coordinate of the incline vector
         * @param dy y coordinate of the incline vector
         * @param dz z coordinate of the incline vector
         * @param sx x coordinate of the shear vector
         * @param sy y coordinate of the shear vector
         * @param sz z coordinate of the shear vector
         * @param just position of the string relative to (x, y). 0: (x, y) is the left of the string, 1: the right, others: interpolation.
         * @param color default is red
         * @return
         * @note 17.98
         */
        PlplotFig&
        PutText3(
            const char* text,
            double wx,
            double wy,
            double wz,
            double dx = 0,
            double dy = 0,
            double dz = 0,
            double sx = 0,
            double sy = 0,
            double sz = 0,
            double just = 0,
            Color0 color = Color0::Red);

        /**
         *
         * @param index index of the color in cmap0.
         * @param r red component, [0, 255].
         * @param g green component, [0, 255].
         * @param b blue component, [0, 255].
         * @param alpha alpha component, [0, 1].
         * @return
         * @note 17.113
         */
        PlplotFig&
        SetCurrentColor0(int index, int r, int g, int b, double alpha);

        /**
         *
         * @param family PL_FCI_SANS, PL_FCI_SERIF, PL_FCI_MONO, PL_FCI_SCRIPT, PL_FCI_SYMBOL
         * @param style PL_FCI_UPRIGHT, PL_FCI_ITALIC, PL_FCI_OBLIQUE
         * @param weight PL_FCI_MEDIUM, PL_FCI_BOLD
         * @return
         * @note 17.131
         */
        PlplotFig&
        SetFont(int family = PL_FCI_SANS, int style = PL_FCI_UPRIGHT, int weight = PL_FCI_MEDIUM);

        /**
         * Split the window into subplots (subpages).
         * @param num_cols number of columns (x-axis)
         * @param num_rows number of rows (y-axis)
         * @return
         * @note 17.145
         */
        PlplotFig&
        Subplots(int num_cols, int num_rows);

        /**
         * Draw a scatter plot (glyph) in 2D.
         * @param num_points number of points
         * @param xs x coordinates of the points
         * @param ys y coordinates of the points
         * @param marker_utf8 UTF-8 string of the marker, default is "#(728)" (cross)
         * @return
         * @note 17.150
         */
        PlplotFig&
        Scatter(int num_points, const double* xs, const double* ys, const char* marker_utf8 = "#(728)");

        /**
         * Draw a scatter plot (glyph) in 3D.
         * @param num_points number of points
         * @param xs x coordinates of the points
         * @param ys y coordinates of the points
         * @param zs z coordinates of the points
         * @param marker_utf8 UTF-8 string of the marker, default is "#(728)" (cross)
         * @return
         * @note 17.151
         */
        PlplotFig&
        Scatter3D(int num_points, const double* xs, const double* ys, const double* zs, const char* marker_utf8 = "#(728)");

        /**
         * Set the margin of the plot.
         * @param left the left margin, [0, 1].
         * @param right the right margin, [0, 1].
         * @param bottom the bottom margin, [0, 1].
         * @param top the top margin, [0, 1].
         * @return
         * @note 17.169
         */
        PlplotFig&
        SetMargin(double left, double right, double bottom, double top);

        /**
         *
         * @return
         * @note 17.170
         */
        PlplotFig&
        SetDefaultMargin();

        /**
         *
         * @param width set the pen width
         * @return
         * @note 17.172
         */
        PlplotFig&
        SetPenWidth(int width);

        /**
         * Set the axis limits.
         * @param x_min the minimum x value.
         * @param x_max the maximum x value.
         * @param y_min the minimum y value.
         * @param y_max the maximum y value.
         * @return
         * @note 17.173
         */
        PlplotFig&
        SetAxisLimits(double x_min, double x_max, double y_min, double y_max);
    };
}  // namespace erl::common
#endif
