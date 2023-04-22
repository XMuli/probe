#ifdef __linux__

#include "probe/defer.h"
#include "probe/graphics.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

namespace probe::graphics
{
    static double calculate_frequency(XRRScreenResources *res, RRMode mode_id)
    {
        for(auto k = 0; k < res->nmode; ++k) {
            auto mode = res->modes[k];
            if(mode.id == mode_id) {
                if(mode.hTotal != 0 && mode.vTotal != 0) {
                    return static_cast<double>((1000 * mode.dotClock) / (mode.hTotal * mode.vTotal)) /
                           1000.0;
                }
                break;
            }
        }

        return 0;
    }

    // A slightly unusual feature is that a display is defined as a workstation consisting
    // of a keyboard, a pointing device such as a mouse, and one or more screens.
    // Multiple screens can work together, with mouse movement allowed to cross physical
    // screen boundaries. As long as multiple screens are controlled by a single user with
    // a single keyboard and pointing device, they comprise only a single display.
    std::vector<display_t> displays()
    {
        std::vector<display_t> _displays;

        // display
        auto display = XOpenDisplay(nullptr);
        if(!display) return {};
        defer(XCloseDisplay(display));

        // display number
        int display_num = 0;
        auto monitors   = XRRGetMonitors(display, DefaultRootWindow(display), True, &display_num);
        defer(XRRFreeMonitors(monitors));

        auto screen_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
        defer(XRRFreeScreenResources(screen_res));

        for(auto i = 0; i < display_num; ++i) {
            // monitors[i]->noutput == 1
            auto output_info = XRRGetOutputInfo(display, screen_res, monitors[i].outputs[0]);
            defer(XRRFreeOutputInfo(output_info));

            if(output_info->connection == RR_Disconnected || !output_info->crtc) {
                continue;
            }

            auto crtc_info = XRRGetCrtcInfo(display, screen_res, output_info->crtc);
            defer(XRRFreeCrtcInfo(crtc_info));

            //
            _displays.push_back({
                XGetAtomName(display, monitors[i].name),
                geometry_t{ crtc_info->x, crtc_info->y, crtc_info->width, crtc_info->height },
                calculate_frequency(screen_res, crtc_info->mode),
                static_cast<uint32_t>(DefaultDepth(display, 0)), // global
                static_cast<uint32_t>((DisplayWidth(display, 0) * 25.4) /
                                      DisplayWidthMM(display, 0)), // global
                static_cast<orientation_t>((crtc_info->rotation & 0x000f) |
                                           static_cast<uint32_t>(!!(crtc_info->rotation & 0x00f0))),
                !!monitors[i].primary,
                1.0 // TODO:
            });
        }
        return _displays;
    }

    std::deque<window_t> windows(bool visible)
    {
        std::deque<window_t> ret;

        auto display = XOpenDisplay(nullptr);
        if(!display) return {};

        auto root_window = DefaultRootWindow(display);

        Window root_return, parent_return;
        Window *child_windows  = nullptr;
        unsigned int child_num = 0;
        // The XQueryTree() function returns the root ID, the parent window ID, a pointer to the list of
        // children windows (NULL when there are no children), and the number of children in the list for
        // the specified window. The children are listed in current stacking order, from bottommost (first)
        // to topmost (last). XQueryTree() returns zero if it fails and nonzero if it succeeds. To free a
        // non-NULL children list when it is no longer needed, use XFree().
        XQueryTree(display, root_window, &root_return, &parent_return, &child_windows, &child_num);

        for(unsigned int i = 0; i < child_num; ++i) {
            XWindowAttributes attrs{};

            XGetWindowAttributes(display, child_windows[i], &attrs);

            if(visible && (attrs.map_state < 2 || (attrs.width * attrs.height < 4))) {
                continue;
            }

            char *buffer = nullptr;
            XFetchName(display, child_windows[i], &buffer);
            defer(XFree(buffer));

            ret.push_front({ (buffer ? buffer : std::string{}),
                             {},
                             geometry_t{ attrs.x, attrs.y, static_cast<uint32_t>(attrs.width),
                                         static_cast<uint32_t>(attrs.height) },
                             static_cast<uint64_t>(child_windows[i]),
                             attrs.map_state >= 2 });
        }

        XFree(child_windows);
        XCloseDisplay(display);

        return ret;
    }
} // namespace probe::graphics

#endif // __linux__
