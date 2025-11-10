#pragma once

#include <opencv2/core.hpp>
#include <pybind11/pybind11.h>

// include after pybind11/pybind11.h
#include <pybind11/numpy.h>

namespace pybind11::detail {

    namespace compatible {  // not available when < 2.9.2

        template<size_t N>
        constexpr descr<N - 1>
        const_name(char const (&text)[N]) {
            return descr<N - 1>(text);
        }

        constexpr descr<0>
        const_name(char const (&)[1]) {
            return {};
        }
    }  // namespace compatible

    template<>
    class type_caster<cv::Scalar> {
    public:
        PYBIND11_TYPE_CASTER(cv::Scalar, compatible::const_name("Scalar"));

        // Python -> C++
        bool
        load(const handle &src, const bool convert) {
            // If we are in no-convert mode, only load if given an array of the correct type
            if (!convert && !isinstance<array_t<double>>(src)) { return false; }

            // get an array of the expected type without Python errors
            auto buf = array_t<double>::ensure(src);
            if (!buf) { return false; }

            if (const auto dims = buf.ndim(); dims < 1 || dims > 2) { return false; }
            buf = buf.squeeze();  // make buf flattened

            if (buf.size() < 1 || buf.size() > 4) {  // cv::Scalar's capacity is 4
                return false;
            }

            for (ssize_t i = 0; i < buf.size(); ++i) {
                value[static_cast<int>(i)] = static_cast<double>(buf.at(i));
            }
            for (ssize_t i = buf.size(); i < 4; ++i) {
                value[static_cast<int>(i)] = 255;  // default value
            }

            return true;
        }

        // C++ -> Python
        static handle
        cast(const cv::Scalar &scalar, return_value_policy, handle) {
            array_t<double> out(4);
            memcpy(out.mutable_data(), scalar.val, 4 * sizeof(double));
            return out.release();
        }
    };

    template<>
    class type_caster<cv::Mat> {
    public:
        PYBIND11_TYPE_CASTER(cv::Mat, compatible::const_name("Mat"));

        // Python -> C++
        bool
        load(handle src, bool) {
            // make sure src is an array without Python errors
            auto buf = array::ensure(src);

            const auto ndim = buf.ndim();
            if (ndim != 2 && ndim != 3) { ERL_FATAL("Buffer dimension should be 2 or 3."); }
            const int num_channels = ndim == 2 ? 1 : static_cast<int>(buf.shape(2));

            if (buf.dtype().equal(dtype::of<uint8_t>())) {
                value = cv::Mat(
                    static_cast<int>(buf.shape(0)),
                    static_cast<int>(buf.shape(1)),
                    CV_MAKETYPE(CV_8U, num_channels),
                    buf.mutable_data());  // NO COPY!
            } else if (buf.dtype().equal(dtype::of<int8_t>())) {
                value = cv::Mat(
                    static_cast<int>(buf.shape(0)),
                    static_cast<int>(buf.shape(1)),
                    CV_MAKETYPE(CV_8S, num_channels),
                    buf.mutable_data());
            } else if (buf.dtype().equal(dtype::of<uint16_t>())) {
                value = cv::Mat(
                    static_cast<int>(buf.shape(0)),
                    static_cast<int>(buf.shape(1)),
                    CV_MAKETYPE(CV_16U, num_channels),
                    buf.mutable_data());
            } else if (buf.dtype().equal(dtype::of<int16_t>())) {
                value = cv::Mat(
                    static_cast<int>(buf.shape(0)),
                    static_cast<int>(buf.shape(1)),
                    CV_MAKETYPE(CV_16S, num_channels),
                    buf.mutable_data());
            } else if (buf.dtype().equal(dtype::of<int32_t>())) {
                value = cv::Mat(
                    static_cast<int>(buf.shape(0)),
                    static_cast<int>(buf.shape(1)),
                    CV_MAKETYPE(CV_32S, num_channels),
                    buf.mutable_data());
            } else if (buf.dtype().equal(dtype::of<float>())) {
                value = cv::Mat(
                    static_cast<int>(buf.shape(0)),
                    static_cast<int>(buf.shape(1)),
                    CV_MAKETYPE(CV_32F, num_channels),
                    buf.mutable_data());
            } else if (buf.dtype().equal(dtype::of<double>())) {
                value = cv::Mat(
                    static_cast<int>(buf.shape(0)),
                    static_cast<int>(buf.shape(1)),
                    CV_MAKETYPE(CV_64F, num_channels),
                    buf.mutable_data());
            } else {
                ERL_FATAL("Unsupported format: {}", str(buf.dtype()).operator std::string());
            }

            return true;
        }

        // C++ -> Python
        static handle
        cast(const cv::Mat &mat, return_value_policy, handle) {
            const int num_channels = mat.channels();
            auto shape = std::vector<ssize_t>{mat.size[0], mat.size[1]};
            if (num_channels > 1) { shape.push_back(num_channels); }
            switch (mat.depth()) {
                case CV_8U:  // NOLINT(*-branch-clone)
                    return array_t<uint8_t>(shape, reinterpret_cast<uint8_t *>(mat.data)).release();
                case CV_8S:
                    return array_t<int8_t>(shape, reinterpret_cast<int8_t *>(mat.data)).release();
                case CV_32S:
                    return array_t<int32_t>(shape, reinterpret_cast<int32_t *>(mat.data)).release();
                case CV_32F:
                    return array_t<float>(shape, reinterpret_cast<float *>(mat.data)).release();
                case CV_64F:
                    return array_t<double>(shape, reinterpret_cast<double *>(mat.data)).release();
#if CV_MAJOR_VERSION >= 4
                case CV_16U:
                    return array_t<uint16_t>(shape, reinterpret_cast<uint16_t *>(mat.data))
                        .release();
                case CV_16S:
                    return array_t<int16_t>(shape, reinterpret_cast<int16_t *>(mat.data)).release();
                case CV_16F:
                    throw cast_error("Does not support CV_16F.");
#endif
                default:
                    throw cast_error("Unknown dtype.");
            }
        }
    };
}  // namespace pybind11::detail
