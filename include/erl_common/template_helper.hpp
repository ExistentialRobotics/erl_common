#pragma once

#include "logging.hpp"

#include <memory>

/// Check if T is an instantiation of the template `Class`. For example:
/// `is_instantiation<shared_ptr, T>` is true if `T == shared_ptr<U>` where U can be anything.
template<template<typename...> class Class, typename T>
struct is_instantiation : std::false_type {};

template<template<typename...> class Class, typename... Us>
struct is_instantiation<Class, Class<Us...>> : std::true_type {};

/// Check if T is std::shared_ptr<U> where U can be anything
template<typename T>
using IsSharedPtr = is_instantiation<std::shared_ptr, T>;

/// Check if T is std::unique_ptr<U> where U can be anything
template<typename T>
using IsUniquePtr = is_instantiation<std::unique_ptr, T>;

/// Check if T is std::weak_ptr<U> where U can be anything
template<typename T>
using IsWeakPtr = is_instantiation<std::weak_ptr, T>;

/// Check if T is smart pointer (std::shared_ptr, std::unique_ptr, std::weak_ptr)
template<typename T>
using IsSmartPtr = std::disjunction<IsSharedPtr<T>, IsUniquePtr<T>, IsWeakPtr<T>>;

/// assert if pointer is null
template<typename T, typename... Args>
T
NotNull(T ptr, const bool fatal, const std::string &msg, Args &&...args) {
    if (ptr == nullptr) {
        if (fatal) { erl::common::Logging::Fatal(msg, std::forward<Args>(args)...); }
        erl::common::Logging::Error(msg, std::forward<Args>(args)...);
    }
    return ptr;
}
