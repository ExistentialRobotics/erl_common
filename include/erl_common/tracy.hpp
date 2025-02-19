#pragma once

#ifdef TRACY_ENABLE
    #include <tracy/Tracy.hpp>
    #define ERL_TRACY_FRAME_MARK_START() FrameMarkStart(__PRETTY_FUNCTION__)
    #define ERL_TRACY_FRAME_MARK_END()   FrameMarkEnd(__PRETTY_FUNCTION__)

    #ifdef ERL_TRACY_PROFILE_MEMORY
        #define ERL_TRACY_RECORD_ALLOC(ptr, size) TracyAlloc((ptr), (size))
        #define ERL_TRACY_RECORD_FREE(ptr)        TracyFree((ptr))
    #else
        #define ERL_TRACY_RECORD_ALLOC(ptr, size) ((void) 0)
        #define ERL_TRACY_RECORD_FREE(ptr)        ((void) 0)
    #endif

    #define ERL_TRACY_SET_THREAD_NAME(name)                             tracy::SetThreadName(name)
    #define ERL_TRACY_PLOT(name, value)                                 TracyPlot(name, value)
    #define ERL_TRACY_PLOT_CONFIG(name, format_type, step, fill, color) TracyPlotConfig(name, format_type, step, fill, color)
#else
    #define ERL_TRACY_FRAME_MARK_START()                                ((void) 0)
    #define ERL_TRACY_FRAME_MARK_END()                                  ((void) 0)
    #define ERL_TRACY_RECORD_ALLOC(ptr, size)                           ((void) 0)
    #define ERL_TRACY_RECORD_FREE(ptr)                                  ((void) 0)
    #define ERL_TRACY_SET_THREAD_NAME(name)                             ((void) 0)
    #define ERL_TRACY_PLOT(name, value)                                 ((void) (value))
    #define ERL_TRACY_PLOT_CONFIG(name, format_type, step, fill, color) ((void) 0)
#endif
