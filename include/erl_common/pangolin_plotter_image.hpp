#pragma once

#include "pangolin_window.hpp"

#include <opencv2/opencv.hpp>

namespace erl::common {
    /**
     * A class to plot images using Pangolin.
     */
    class PangolinPlotterImage {
        std::shared_ptr<PangolinWindow> m_window_;
        pangolin::View &m_display_;
        std::shared_ptr<pangolin::GlTexture> m_image_texture_;

    public:
        /**
         *
         * @param window
         * @param image_height
         * @param image_width
         * @param format
         * @param dtype
         */
        PangolinPlotterImage(std::shared_ptr<PangolinWindow> window, int image_height, int image_width, int format = GL_RGBA, int dtype = GL_UNSIGNED_BYTE);

        void
        Update(const cv::Mat &image) const;

        void
        Render() const;
    };
}  // namespace erl::common
