#ifdef ERL_USE_PANGOLIN
    #include "erl_common/pangolin_window.hpp"

    #include "erl_common/random.hpp"

namespace erl::common {
    PangolinWindow::PangolinWindow(const std::string &window_name, const int window_width, const int window_height, int window_x, int window_y)
        : m_window_name_(window_name),
          m_window_(pangolin::CreateWindowAndBind(window_name, window_width, window_height)) {
        m_window_.MakeCurrent();
        if (window_x < 0) { window_x = std::uniform_int_distribution<>(0, 1920)(g_random_engine); }
        if (window_y < 0) { window_y = std::uniform_int_distribution<>(0, 1080)(g_random_engine); }
        m_window_.Move(window_x, window_y);
        glEnable(GL_DEPTH_TEST);
        m_displays_["main"] = &pangolin::CreateDisplay();
    }

}  // namespace erl::common
#endif
