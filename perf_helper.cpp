#include "perf_helper.h"
#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>

perf_helper::perf_helper() : commit_info("") {
  std::string filename = ".last_commit.info";
#ifdef WIN32
  // see https://learn.microsoft.com/en-us/windows/win32/procthread/creating-processes for more information
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  std::string cmdtext = "cmd /c \"git log -1 --pretty=%h-%f > " + filename + " && echo I hate Windows > NUL \"";
  wchar_t wtext[100];
  mbstowcs(wtext, cmdtext.c_str(), cmdtext.length());
  LPWSTR cmd = wtext;

  if (!CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    printf("CreateProcess failed (%d).\n", GetLastError());
    return;
  }

  WaitForSingleObject(pi.hProcess, INFINITE);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
#endif // WIN32
  std::ifstream ifs(filename);
  if (ifs) {
    std::getline(ifs, commit_info);
  }
}

void perf_helper::profile(std::function<void()> const& f) {
  using namespace std::chrono_literals;
  using std::chrono::steady_clock;
  using std::chrono::system_clock;
  std::time_t const sys_time = system_clock::to_time_t(system_clock::now());
  std::stringstream timestamp;
  timestamp << std::put_time(std::localtime(&sys_time), "%Y-%m-%d--%H-%M-%S--");
  std::filesystem::path fpath("logs");
  fpath.append(timestamp.str()).concat(commit_info).concat(".log");
  std::ofstream file(fpath);
  if (!file) {
    std::cerr << "Log file " << fpath.string() << "was not open" << std::endl;
    return;
  }
  long long sum = 0;
  for (std::size_t run = 0; run < NRUNS; ++run) {
    auto start = steady_clock::now();
    f();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now() - start);
    file << "run #" << run << ": " << elapsed.count() << " ms\n";
    sum += elapsed.count();
  }
  file << "mean time: " << std::fixed << std::setprecision(3) << static_cast<double>(sum) / NRUNS << " ms\n";
}
