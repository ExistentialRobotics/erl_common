#pragma once
// disable warning when using pybind11
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <pybind11/pybind11.h>

namespace py = pybind11;

// NUMPY
#include <pybind11/numpy.h>

// EIGEN
#include <pybind11/eigen.h>

/**
 * NumPy to Eigen behavior
 * Type                                    | Accept c-style | Accept f-style | require writable | Posted to compatible different Ref types |
 * py::EigenDRef<const Eigen::MatrixXd>    | ref            | ref            | no               | temp copy                                |
 * py::EigenDRef<Eigen::MatrixXd>          | ref            | ref            | yes              | compile time error                       |
 * Eigen::Ref<const Eigen::MatrixXd>       | copy           | ref            | no               | temp copy                                |
 * Eigen::Ref<const ERMatrix<double>>      | copy           | ref            | no               | temp copy                                |
 * Eigen::Ref<Eigen::MatrixXd>             | no             | ref            | yes              | compile time error                       |
 * Eigen::Ref<ERMatrix<double>>            | ref            | no             | yes              | compile time error                       |
 *
 * Eigen::MatrixXd::conservativeResize can be used to replace std::vector<gpis::Point<T, Dim>>
 */

#include "pybind11_opencv.hpp"

#include <pybind11/functional.h>
#include <pybind11/stl.h>

#define ERL_PYBIND_WRAP_NAME_PROPERTY_AS_READONLY(py_cls, cls, name, property) py_cls.def_property_readonly(name, [](const cls& obj) { return obj.property; })
#define ERL_PYBIND_WRAP_PROPERTY_AS_READONLY(py_cls, cls, property)            ERL_PYBIND_WRAP_NAME_PROPERTY_AS_READONLY(py_cls, cls, #property, property)

namespace PYBIND11_NAMESPACE {
    template<typename T>
    using SupportedByNumpy = detail::any_of<detail::is_pod_struct<T>, std::is_arithmetic<T>>;

    template<typename T>
    class RawPtrWrapper {
        T* m_ptr_ = nullptr;

    public:
        RawPtrWrapper() = default;

        explicit RawPtrWrapper(T* ptr)
            : m_ptr_(ptr) {}

        T&
        operator*() const {
            return *m_ptr_;
        }

        T*
        operator->() const {
            return m_ptr_;
        }

        T&
        operator[](std::size_t i) const {
            return m_ptr_[i];
        }

        T*
        get() const {
            return m_ptr_;
        }
    };

    namespace detail {
        template<typename Iterator>
        struct iterator_self_access {
            using result_type = Iterator&;

            result_type
            operator()(Iterator& it) const {
                return it;
            }
        };
    }  // namespace detail

    template<
        return_value_policy Policy = return_value_policy::reference_internal,
        typename Iterator,
        typename Sentinel,
        typename ValueType = typename detail::iterator_self_access<Iterator>::result_type,
        typename... Extra>
    typing::Iterator<ValueType>
    wrap_iterator(Iterator first, Sentinel last, Extra&&... extra) {
        return detail::make_iterator_impl<detail::iterator_self_access<Iterator>, Policy, Iterator, Sentinel, ValueType, Extra...>(
            std::forward<Iterator>(first),
            std::forward<Sentinel>(last),
            std::forward<Extra>(extra)...);
    }
}  // namespace PYBIND11_NAMESPACE

PYBIND11_DECLARE_HOLDER_TYPE(T, RawPtrWrapper<T>);

#pragma GCC diagnostic pop
