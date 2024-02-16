#pragma once

#include <memory>

// Wrapper for recursive std::variant
// (credit: https://www.foonathan.net/2022/05/recursive-variant-box/)
template <typename T> class box {
  std::unique_ptr<T> _impl;

public:
  box(T &&obj) : _impl(new T(std::move(obj))) {}
  box(const T &obj) : _impl(new T(obj)) {}

  box(const box &other) : box(*other._impl) {}
  box &operator=(const box &other) {
    *_impl = *other._impl;
    return *this;
  }

  ~box() = default;

  T &operator*() { return *_impl; }
  const T &operator*() const { return *_impl; }

  T *operator->() { return _impl.get(); }
  const T *operator->() const { return _impl.get(); }
};
