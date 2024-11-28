#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <string>

class TradingError : public std::runtime_error {
 public:
  explicit TradingError(const std::string& message)
      : std::runtime_error(message) {}
};

class NetworkError : public TradingError {
 public:
  explicit NetworkError(const std::string& message)
      : TradingError("Network error: " + message) {}
};

class APIError : public TradingError {
 public:
  explicit APIError(const std::string& message)
      : TradingError("API error: " + message) {}
};

class JSONParseError : public TradingError {
 public:
  explicit JSONParseError(const std::string& message)
      : TradingError("JSON parse error: " + message) {}
};

class Logger {
 private:
  std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> loggers;

  Logger() = default;  // Private constructor

 public:
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  static Logger& getInstance() {
    static Logger instance;
    return instance;
  }

  std::shared_ptr<spdlog::logger> getLogger(const std::string& log_name) {
    auto it = loggers.find(log_name);
    if (it == loggers.end()) {
      std::string log_path = "logs/" + log_name + ".log";
      auto logger = spdlog::basic_logger_mt(log_name, log_path, true);
      loggers[log_name] = logger;
      return logger;
    }
    return it->second;
  }

  template <typename... Args>
  void log(const std::string& logger_name, spdlog::level::level_enum level,
           spdlog::format_string_t<Args...> fmt, Args&&... args) {
    auto logger = getLogger(logger_name);
    logger->log(level, fmt, std::forward<Args>(args)...);
    logger->flush();
  }

  void flush(const std::string& logger_name) {
    auto logger = getLogger(logger_name);
    logger->flush();
  }
};

#define LOG_INFO(logger_name, ...) \
  Logger::getInstance().log(logger_name, spdlog::level::info, __VA_ARGS__)
#define LOG_WARNING(logger_name, ...) \
  Logger::getInstance().log(logger_name, spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(logger_name, ...) \
  Logger::getInstance().log(logger_name, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(logger_name, ...) \
  Logger::getInstance().log(logger_name, spdlog::level::critical, __VA_ARGS__)