#ifdef ERL_USE_PLPLOT
    #include "erl_common/plplot_fig.hpp"
    #include "erl_common/test_helper.hpp"

TEST(PlplotFig, Demo04) {

    using namespace erl::common;
    PlplotFig fig(640, 480, true);
    fig.SetFont();

    // prepare data
    std::vector<double> freql(101);
    std::vector<double> ampl(101);
    std::vector<double> phase(101);
    for (int i = 0; i < 101; ++i) {
        constexpr double f0 = 1.0;
        freql[i] = -2.0 + i / 20.0;
        const double freq = std::pow(10.0, freql[i]);
        ampl[i] = 20.0 * log10(1.0 / sqrt(1.0 + pow((freq / f0), 2.)));
        phase[i] = -(180.0 / M_PI) * atan((PLFLT) (freq / f0));
    }

    // page 1

    fig.Clear()  //
        .SetMargin(0.15, 0.85, 0.15, 0.9)
        .SetAxisLimits(-2.0, 3.0, -80.0, 0.0)
        .SetCurrentColor(PlplotFig::Color0::Red)
        .DrawAxesBox(PlplotFig::AxisOpt().DrawTopRightEdge().LogScale() /*bclnst*/, PlplotFig::AxisOpt().DrawPerpendicularTickLabels() /*bnstv*/)
        .SetCurrentColor(PlplotFig::Color0::Yellow)
        .DrawLine(101, freql.data(), ampl.data())               // plot ampl vs freq
        .PutText("-20 dB/decade", 1.6, -30.0, 1.0, -20.0, 0.5)  // put text near the curve
        .SetCurrentColor(PlplotFig::Color0::Red)
        .SetAxisLabelX("Frequency")
        .SetTitle("Single Pole Low-Pass Filter")
        .SetCurrentColor(PlplotFig::Color0::Yellow)
        .SetAxisLabelY("Amplitude (dB)");

    fig.SetCurrentColor(PlplotFig::Color0::Red)  //
        .SetAxisLimits(-2.0, 3.0, -100.0, 0.0)
        .DrawAxesBox(
            PlplotFig::AxisOpt::Off(),
            PlplotFig::AxisOpt::Off().DrawTopRightEdge().DrawTopRightTickLabels().DrawTickMajor().DrawTickMinor().DrawPerpendicularTickLabels(),
            0.0,
            30.0,
            0,
            3)
        .SetCurrentColor(PlplotFig::Color0::Green)
        .DrawLine(101, freql.data(), phase.data())  // plot phase vs freq
        .Scatter(101, freql.data(), phase.data())
        .SetCurrentColor(PlplotFig::Color0::Green)
        .SetAxisLabelY("Phase shift (degrees)", true);

    double legend_width, legend_height;
    fig.SetCurrentColor0(15, 32, 32, 32, 0.70)  //
        .Legend(
            legend_width,
            legend_height,
            2,
            {"Amplitude", "Phase shift"},
            {PL_LEGEND_LINE, PL_LEGEND_LINE | PL_LEGEND_SYMBOL},
            {static_cast<int>(PlplotFig::Color0::Yellow), static_cast<int>(PlplotFig::Color0::Green)},
            {1, 3},
            {1, 1},
            {1.0, 1.0},
            {"", "#(728)"},
            {0, 3},
            {1.0, 1.0},
            {0, 4});
    cv::imshow("demo04", fig.ToCvMat());
    cv::waitKey(0);

    // page 2
    fig.Clear()  //
        .SetMargin(0.15, 0.85, 0.15, 0.9)
        .SetAxisLimits(-2.0, 3.0, -80.0, 0.0)
        .SetCurrentColor(PlplotFig::Color0::Red)
        .DrawAxesBox(
            PlplotFig::AxisOpt().DrawTopRightEdge().LogScale().UseFixedPointNumeric().DrawGridMajor().DrawGridMinor() /*bcfghlnst*/,
            PlplotFig::AxisOpt().DrawTopRightEdge().DrawGridMajor().DrawGridMinor().DrawPerpendicularTickLabels() /*bcghnstv*/)
        .SetCurrentColor(PlplotFig::Color0::Yellow)
        .DrawLine(101, freql.data(), ampl.data())               // plot ampl vs freq
        .PutText("-20 dB/decade", 1.6, -30.0, 1.0, -20.0, 0.5)  // put text near the curve
        .SetCurrentColor(PlplotFig::Color0::Red)
        .SetAxisLabelX("Frequency")
        .SetTitle("Single Pole Low-Pass Filter")
        .SetCurrentColor(PlplotFig::Color0::Yellow)
        .SetAxisLabelY("Amplitude (dB)");
    fig.SetCurrentColor0(15, 32, 32, 32, 0.70)  //
        .Legend(
            legend_width,
            legend_height,
            1,  // draw the first legend entry only
            {"Amplitude", "Phase shift"},
            {PL_LEGEND_LINE, PL_LEGEND_LINE | PL_LEGEND_SYMBOL},
            {static_cast<int>(PlplotFig::Color0::Yellow), static_cast<int>(PlplotFig::Color0::Green)},
            {1, 3},
            {1, 1},
            {1.0, 1.0},
            {"", "#(728)"},
            {0, 3},
            {1.0, 1.0},
            {0, 4});
    cv::imshow("demo04", fig.ToCvMat());
    cv::waitKey(0);
}
#else
    #include "erl_common/test_helper.hpp"

TEST(PlplotFig, Demo04) { ERL_INFO("plplot is not enabled, skip test. Please configure cmake with -DERL_USE_PLPLOT=ON to run this test."); }
#endif
