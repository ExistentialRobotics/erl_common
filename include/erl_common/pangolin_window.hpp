#pragma once

#include <pangolin/pangolin.h>
#include <pangolin/display/display.h>
#include <map>

namespace erl::common {

    class PangolinWindow {
        std::string m_window_name_;
        pangolin::WindowInterface& m_window_;
        std::map<std::string, pangolin::View*> m_displays_;

    public:
        PangolinWindow(const std::string& window_name, int window_width, int window_height, int window_x = 0, int window_y = 0)
            : m_window_name_(window_name),
              m_window_(pangolin::CreateWindowAndBind(window_name, window_width, window_height)) {
            m_window_.MakeCurrent();
            m_window_.Move(window_x, window_y);
            glEnable(GL_DEPTH_TEST);
            m_displays_["main"] = &pangolin::DisplayBase();
        }

        void
        Activate(bool clear = false) {
            m_window_.MakeCurrent();
            pangolin::BindToContext(m_window_name_);
            m_window_.ProcessEvents();
            m_displays_["main"]->Activate();
            if (clear) { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
        }

        void
        Deactivate() {
            m_window_.RemoveCurrent();
        }

        pangolin::WindowInterface&
        GetWindow() {
            return m_window_;
        }

        pangolin::View&
        AddDisplay(const std::string& display_name, pangolin::View& display) {
            m_displays_[display_name] = &m_displays_["main"]->AddDisplay(display);
            return *m_displays_[display_name];
        }

        pangolin::View&
        AddDisplay(const std::string& display_name, pangolin::View& display, pangolin::View& display_parent) {
            m_displays_[display_name] = &display_parent.AddDisplay(display);
            return *m_displays_[display_name];
        }

        pangolin::View&
        GetDisplay(const std::string& display_name) {
            return *m_displays_.at(display_name);
        }
    };

}  // namespace erl::common
