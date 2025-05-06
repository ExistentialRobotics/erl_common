#ifdef ERL_USE_PANGOLIN
    #include "erl_common/pangolin_plotter_image.hpp"

namespace erl::common {
    PangolinPlotterImage::PangolinPlotterImage(
        std::shared_ptr<PangolinWindow> window,
        int image_height,
        int image_width,
        int format,
        int dtype)
        : m_window_(std::move(window)),
          m_display_(m_window_->AddDisplay(
              "image",
              pangolin::CreateDisplay().SetBounds(
                  0,
                  1.0,
                  0,
                  1.0,
                  static_cast<float>(image_width) / static_cast<float>(image_height)))),
          m_image_texture_(std::make_shared<pangolin::GlTexture>(
              image_width,
              image_height,
              format,
              false,
              0,
              format,
              dtype)) {}

    void
    PangolinPlotterImage::Update(const cv::Mat &image) const {
        m_window_->Activate();
        m_display_.Activate();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear entire window
        m_image_texture_->Upload(image.data, GL_BGRA, GL_UNSIGNED_BYTE);
        m_image_texture_->RenderToViewportFlipY();
        m_window_->Deactivate();
    }

    void
    PangolinPlotterImage::Render() const {
        m_window_->Activate();
        m_display_.Activate();
        m_image_texture_->RenderToViewportFlipY();
        m_window_->Deactivate();
    }

}  // namespace erl::common
#endif
