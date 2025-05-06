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
        .DrawAxesBox(
            PlplotFig::AxisOpt().DrawTopRightEdge().LogScale() /*bclnst*/,
            PlplotFig::AxisOpt().DrawPerpendicularTickLabels() /*bnstv*/)
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
            PlplotFig::AxisOpt::Off()
                .DrawTopRightEdge()
                .DrawTopRightTickLabels()
                .DrawTickMajor()
                .DrawTickMinor()
                .DrawPerpendicularTickLabels(),
            0.0,
            30.0,
            0,
            3)
        .SetCurrentColor(PlplotFig::Color0::Green)
        .DrawLine(101, freql.data(), phase.data())  // plot phase vs freq
        .Scatter(101, freql.data(), phase.data())
        .SetCurrentColor(PlplotFig::Color0::Green)
        .SetAxisLabelY("Phase shift (degrees)", true);

    PlplotFig::LegendOpt legend_opt(2, {"Amplitude", "Phase shift"});
    legend_opt
        .SetTextColors(
            {static_cast<int>(PlplotFig::Color0::Yellow),
             static_cast<int>(PlplotFig::Color0::Green)})
        .SetStyles({PL_LEGEND_LINE, PL_LEGEND_LINE | PL_LEGEND_SYMBOL})
        .SetLineColors({1, 3})
        .SetLineStyles({1, 1})
        .SetLineWidths({1.0, 1.0})
        .SetSymbols({"", "#(728)"})
        .SetSymbolColors({0, 3})
        .SetSymbolScales({1.0, 1.0})
        .SetSymbolSizes({0, 4});
    fig.SetCurrentColor0(15, 32, 32, 32, 0.70).Legend(legend_opt);
    cv::imshow("demo04", fig.ToCvMat());
    cv::waitKey(2000);

    // page 2
    fig.Clear()  //
        .SetMargin(0.15, 0.85, 0.15, 0.9)
        .SetAxisLimits(-2.0, 3.0, -80.0, 0.0)
        .SetCurrentColor(PlplotFig::Color0::Red)
        .DrawAxesBox(
            PlplotFig::AxisOpt()
                .DrawTopRightEdge()
                .LogScale()
                .UseFixedPointNumeric()
                .DrawGridMajor()
                .DrawGridMinor() /*bcfghlnst*/,
            PlplotFig::AxisOpt()
                .DrawTopRightEdge()
                .DrawGridMajor()
                .DrawGridMinor()
                .DrawPerpendicularTickLabels() /*bcghnstv*/)
        .SetCurrentColor(PlplotFig::Color0::Yellow)
        .DrawLine(101, freql.data(), ampl.data())               // plot ampl vs freq
        .PutText("-20 dB/decade", 1.6, -30.0, 1.0, -20.0, 0.5)  // put text near the curve
        .SetCurrentColor(PlplotFig::Color0::Red)
        .SetAxisLabelX("Frequency")
        .SetTitle("Single Pole Low-Pass Filter")
        .SetCurrentColor(PlplotFig::Color0::Yellow)
        .SetAxisLabelY("Amplitude (dB)");
    fig.SetCurrentColor0(15, 32, 32, 32, 0.70).Legend(legend_opt.SetNumLegend(1));
    cv::imshow("demo04", fig.ToCvMat());
    cv::waitKey(2000);
}

TEST(PlplotFig, Demo16) {
    using namespace erl::common;

    constexpr int nx = 35;
    constexpr int ny = 70;
    Eigen::MatrixXd z(nx, ny);
    Eigen::MatrixXd w(nx, ny);
    for (int j = 0; j < ny; j++) {
        double y = (j - static_cast<double>(ny) / 2) / (static_cast<double>(ny) / 2) - 1.0;
        for (int i = 0; i < nx; i++) {
            double x = (i - static_cast<double>(nx) / 2) / (static_cast<double>(nx) / 2);
            z(i, j) = -sin(7. * x) * cos(7. * y) + x * x - y * y;
            w(i, j) = -cos(7. * x) * sin(7. * y) + 2 * x * y;
        }
    }

    const double zmin = z.minCoeff();
    const double zmax = z.maxCoeff();

    constexpr int ns = 20;
    std::vector<double> shedge(ns + 1);
    for (int i = 0; i < ns + 1; i++) {
        shedge[i] = zmin + (zmax - zmin) * static_cast<double>(i) / static_cast<double>(ns);
    }

    PlplotFig fig(640, 480, true);
    PlplotFig::ShadesOpt shades_opt;
    shades_opt.SetColorLevels(z.data(), nx, ny, ns).SetContourColor0(15).SetContourLineWidth(1.0);
    fig.Clear()
        .SetMargin(0.15, 0.85, 0.15, 0.85)
        .SetAxisLimits(-1.0, 1.0, -1.0, 1.0)
        .SetCurrentColor(PlplotFig::Color0::White)
        .DrawAxesBox(
            PlplotFig::AxisOpt().DrawTopRightEdge(),
            PlplotFig::AxisOpt().DrawPerpendicularTickLabels())
        .SetAreaFillPattern(PlplotFig::AreaFillPattern::Solid)
        .SetColorMap(1, PlplotFig::ColorMap::Jet)
        .Shades(z.data(), nx, ny, true, shades_opt);
    cv::imshow("demo16", fig.ToCvMat());
    cv::waitKey(2000);
}
#else
    #include "erl_common/test_helper.hpp"

TEST(PlplotFig, Demo04) {
    ERL_INFO(
        "plplot is not enabled, skip test. Please configure cmake with -DERL_USE_PLPLOT=ON to run "
        "this test.");
}
#endif
