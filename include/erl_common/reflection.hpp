#pragma once

#include <string>

template<typename T, typename Member>
struct MemberInfo {
    const char *name = "";
    Member T::*ptr = nullptr;
    std::string T::*type_ptr = nullptr;  // field to get the type of the member
};

template<typename T, typename... Members>
constexpr auto
MakeSchema(MemberInfo<T, Members>... members) {
    return std::make_tuple(members...);
}

template<typename T>
struct EnumMemberInfo {
    const char *name = "";
    T value;
};

/**
 * Specialization should be provided for each enum type T.
 * @tparam T Enum type.
 * @tparam N Number of enum members.
 * @return array of EnumMemberInfo<T> of size N.
 */
template<typename T, int N>
constexpr std::array<EnumMemberInfo<T>, N>
MakeEnumSchema();

#define ERL_REFLECT_SCHEMA(T, ...) static constexpr auto Schema = MakeSchema<T>(__VA_ARGS__)
#define ERL_REFLECT_MEMBER(T, x) \
    MemberInfo<T, decltype(T::x)> { #x, &T::x, nullptr }
#define ERL_REFLECT_MEMBER_POLY(T, x, t) \
    MemberInfo<T, decltype(T::x)> { #x, &T::x, &T::t }
#define ERL_REFLECT_ENUM_SCHEMA(T, N, ...)                              \
    template<>                                                          \
    constexpr std::array<EnumMemberInfo<T>, N> MakeEnumSchema<T, N>() { \
        return {__VA_ARGS__};                                           \
    }
#define ERL_REFLECT_ENUM_MEMBER(name, val) \
    EnumMemberInfo<decltype(val)> { name, val }
