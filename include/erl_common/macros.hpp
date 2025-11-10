#pragma once

#ifndef NDEBUG
    #define VEC_ACCESS(vec, index) vec.at(index)
#else
    #define VEC_ACCESS(vec, index) vec[index]
#endif
