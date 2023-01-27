//
// Created by Danila Belous on 26.01.2023 at 20:40.
//

#pragma once
#include <chrono>
#include <string>
#include <functional>

class perf_helper {
public:
  perf_helper();

  void profile(std::function<void()> const& f);

private:
  constexpr static std::size_t NRUNS = 3;
  std::string commit_info;
};
