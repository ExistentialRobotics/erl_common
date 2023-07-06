#pragma once

#include <iostream>
#include <pybind11/pybind11.h>
#include <opencv2/core.hpp>

namespace pybind11::detail {
    template<>
    struct type_caster<cv::Scalar> {
    public:
        PYBIND11_TYPE_CASTER(cv::Scalar, const_name("Scalar"));

        // Python -> C++
        bool
        load(handle src, bool convert) {
            // If we are in no-convert mode, only load if given an array of the correct type
            if (!convert && !isinstance<array_t<double>>(src)) { return false; }

            // get an array of the expected type without Python errors
            auto buf = array_t<double>::ensure(src);
            if (!buf) { return false; }

            auto dims = buf.ndim();
            if (dims < 1 || dims > 2) { return false; }
            buf = buf.squeeze();  // make buf flattened

            if (buf.size() < 1 || buf.size() > 4) {  // cv::Scalar's capacity is 4
                return false;
            }

            auto a = array_t<double>(buf);
            for (long i = 0; i < buf.size(); ++i) { value[i] = double(buf.at(i)); }
            for (long i = buf.size(); i < 4; ++i) { value[i] = 255; }  // default value

            return true;
        }

        // C++ -> Python
        static handle
        cast(cv::Scalar scalar, return_value_policy, handle) {
            array_t<double> out(4);
            memcpy(out.mutable_data(), scalar.val, 4 * sizeof(double));
            return out.release();
        }
    };

    template<>
    struct type_caster<cv::Mat> {
    public:
        PYBIND11_TYPE_CASTER(cv::Mat, const_name("Mat"));

        // Python -> C++
        bool
        load(handle src, bool) {
            // make sure src is an array without Python errors
            auto buf = array::ensure(src);

            auto ndim = buf.ndim();
            if (ndim != 2 && ndim != 3) { throw std::runtime_error("Buffer dimension should be 2 or 3."); }
            int num_channels = ndim == 2 ? 1 : int(buf.shape(2));

            if (buf.dtype().equal(dtype::of<uint8_t>())) {
                value = cv::Mat(
                    int(buf.shape(0)),
                    int(buf.shape(1)),
                    CV_MAKETYPE(CV_8U, num_channels),
                    static_cast<uint8_t *>(buf.mutable_data()));  // NO COPY!
            } else if (buf.dtype().equal(dtype::of<int8_t>())) {
                value = cv::Mat(int(buf.shape(0)), int(buf.shape(1)), CV_MAKETYPE(CV_8S, num_channels), static_cast<int8_t *>(buf.mutable_data()));
            } else if (buf.dtype().equal(dtype::of<uint16_t>())) {
                value = cv::Mat(int(buf.shape(0)), int(buf.shape(1)), CV_MAKETYPE(CV_16U, num_channels), static_cast<uint16_t *>(buf.mutable_data()));
            } else if (buf.dtype().equal(dtype::of<int16_t>())) {
                value = cv::Mat(int(buf.shape(0)), int(buf.shape(1)), CV_MAKETYPE(CV_16S, num_channels), static_cast<int16_t *>(buf.mutable_data()));
            } else if (buf.dtype().equal(dtype::of<int32_t>())) {
                value = cv::Mat(int(buf.shape(0)), int(buf.shape(1)), CV_MAKETYPE(CV_32S, num_channels), static_cast<int32_t *>(buf.mutable_data()));
            } else if (buf.dtype().equal(dtype::of<float>())) {
                value = cv::Mat(int(buf.shape(0)), int(buf.shape(1)), CV_MAKETYPE(CV_32F, num_channels), static_cast<float *>(buf.mutable_data()));
            } else if (buf.dtype().equal(dtype::of<double>())) {
                value = cv::Mat(int(buf.shape(0)), int(buf.shape(1)), CV_MAKETYPE(CV_64F, num_channels), static_cast<double *>(buf.mutable_data()));
            } else {
                throw std::runtime_error("Unsupported format: " + str(buf.dtype()).operator std::string());
            }

            return true;
        }

        // C++ -> Python
        static handle
        cast(const cv::Mat &mat, return_value_policy, handle) {
            auto num_channels = mat.channels();
            auto shape = std::vector<py::ssize_t>{mat.size[0], mat.size[1]};
            if (num_channels > 1) { shape.push_back(num_channels); }

            int depth = mat.depth();

            switch (depth) {
                case CV_8U:
                    return array_t<uint8_t>(shape, py::detail::c_strides(shape, py::ssize_t(sizeof(uint8_t))), reinterpret_cast<uint8_t *>(mat.data)).release();
                case CV_8S:
                    return array_t<int8_t>(shape, py::detail::c_strides(shape, py::ssize_t(sizeof(int8_t))), reinterpret_cast<int8_t *>(mat.data)).release();
                case CV_32S:
                    return array_t<int32_t>(shape, py::detail::c_strides(shape, py::ssize_t(sizeof(int32_t))), reinterpret_cast<int32_t *>(mat.data)).release();
                case CV_32F:
                    return array_t<float>(shape, py::detail::c_strides(shape, py::ssize_t(sizeof(float))), reinterpret_cast<float *>(mat.data)).release();
                case CV_64F:
                    return array_t<double>(shape, py::detail::c_strides(shape, py::ssize_t(sizeof(double))), reinterpret_cast<double *>(mat.data)).release();
#if CV_MAJOR_VERSION >= 4
                case CV_16U:
                    return array_t<uint16_t>(shape, py::detail::c_strides(shape, py::ssize_t(sizeof(uint16_t))), reinterpret_cast<uint16_t *>(mat.data))
                        .release();
                case CV_16S:
                    return array_t<int16_t>(shape, py::detail::c_strides(shape, py::ssize_t(sizeof(int16_t))), reinterpret_cast<int16_t *>(mat.data)).release();
                case CV_16F:
                    throw cast_error("Does not support CV_16F.");
#endif
                default:
                    throw cast_error("Unknown dtype.");
            }
        }
    };
}  // namespace pybind11::detail
