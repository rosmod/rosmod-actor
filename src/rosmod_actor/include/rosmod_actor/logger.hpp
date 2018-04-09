/** @brief   Logger.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    April 01 2015
 *  @brief   This file declares the Component Logger class
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <mutex>
#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <string>
#include <iostream>
#include <stdarg.h>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <typeinfo>

/**
 * @brief Logger class
 */
class Logger 
{

public:
  /**
   * @brief Logger Constructor.
   */
  Logger() {
    logs_to_file_ = false;
    is_periodic_ = true;
    logging_enabled_ = false;
    max_log_unit_ = 1;
    log_content_ = "=================================================================\n";
  }

  /**
   * @brief Makes Logging Periodic.
   * @param[in] is_periodic boolean indicating whether logging is periodic.
   */
  void set_is_periodic(bool is_periodic) {
    {
      std::lock_guard<std::recursive_mutex> lk(settings_mutex);
      is_periodic_ = is_periodic;
    }
  }

  /**
   * @brief Set maximum log unit in bytes during periodic logging.
   * @param[in] max_log_unit maximum periodic logging unit in bytes.
   */
  void set_max_log_unit(int max_log_unit) {
    {
      std::lock_guard<std::recursive_mutex> lk(settings_mutex);
      max_log_unit_ = max_log_unit;
    }
  }

  /**
   * @brief Log to file instead of stdout.
   * @param[in] logs_to_file boolean requiring logging on file.
   */
  void set_logs_to_file(bool logs_to_file) {
    {
      std::lock_guard<std::recursive_mutex> lk(settings_mutex);
      logs_to_file_ = logs_to_file;
    }
  }

  /**
   * @brief Enable this logger
   */
  void enable_logging() {
    {
      std::lock_guard<std::recursive_mutex> lk(settings_mutex);
      logging_enabled_ = true;
    }
  }

  /**
   * @brief Disable this logger
   */
  void disable_logging() {
    {
      std::lock_guard<std::recursive_mutex> lk(settings_mutex);
      logging_enabled_ = false;
    }
  }

  /**
   * @brief Writes out the remainder of the logs and closes logfile.
   */
  ~Logger() {
    write();
    {
      std::lock_guard<std::recursive_mutex> lk(io_mutex);
      log_stream_.close();
    }
  }

  /**
   * @brief Create log file
   * @param[in] log_path path to log file.
   */
  bool create_file(std::string log_path) {
    {
      std::lock_guard<std::recursive_mutex> lk0(settings_mutex);
      std::lock_guard<std::recursive_mutex> lk1(io_mutex);
      if (logging_enabled_) {
	log_path_ = log_path;
	log_stream_.open(log_path_, std::ios::out | std::ios::app);
	logs_to_file_ = true;
	return true;
      }
    }
    return false;
  }

  /**
   * @brief Write logged bytes to file
   */  
  bool write() {
    {
      std::lock_guard<std::recursive_mutex> lk0(settings_mutex);
      std::lock_guard<std::recursive_mutex> lk1(io_mutex);
      if (logging_enabled_) {
	if (logs_to_file_) {
	  log_stream_ << log_content_;
	  log_stream_.flush();
	}
	else
	  printf("%s", log_content_.c_str());
	return true;
      }
      else
	return false;
    }
  }

  /**
   * @brief Flush out to file.
   */  
  bool flush() {
    {
      std::lock_guard<std::recursive_mutex> lk(settings_mutex);
      if (is_periodic_ && size() > max_log_unit_) {
	write();
	log_content_ = "";
	return true;
      }
    }
    return false;
  }

  /**
   * @brief Log to file with specific log_level. '\n' will be appended.
   *
   * @param[in] log_level string indicating logging level.
   * @param[in] format varargs input to logger.
   *
   * @detailed Log data will be formmatted according to
   *           ROSMOD::<log_level>::<timestamp>::<log data> formatting
   *
   */  
  bool log(std::string log_level, const char * format, ...) {
    {
      std::lock_guard<std::recursive_mutex> lk0(settings_mutex);
      std::lock_guard<std::recursive_mutex> lk1(io_mutex);
      if (logging_enabled_) {
	va_list args;
	va_start (args, format);
	char log_entry[1024];
	vsprintf (log_entry, format, args);
	std::string log_entry_string(log_entry);
	va_end (args);
	log_content_ += "ROSMOD::" + log_level  + "::" + clock() + 
	  "::" + log_entry_string + "\n";
	flush();
      }
    }
  }

  /**
   * @brief Log to file without formatting. '\n' will be appended.
   * @param[in] format varargs input to logger.
   */  
  bool raw_log(const char * format, ...) {
    {
      std::lock_guard<std::recursive_mutex> lk0(settings_mutex);
      std::lock_guard<std::recursive_mutex> lk1(io_mutex);
      if (logging_enabled_) {
	va_list args;
	va_start (args, format);
	char log_entry[1024];
	vsprintf (log_entry, format, args);
	std::string log_entry_string(log_entry);
	va_end (args);
	log_content_ += log_entry_string + "\n";
	flush();
      }
    }
  }

  /**
   * @brief Return the current size of the log in bytes.
   */  
  int size() {
    {
      std::lock_guard<std::recursive_mutex> lk(io_mutex);
      return log_content_.size();
    }
  }

  /**
   * @brief Return the current clock value.
   */  
  std::string clock() {
    std::stringstream clock_string;
    clock_string << clock_.now().time_since_epoch().count();
    return clock_string.str();
  }

private:
  std::recursive_mutex io_mutex;               /*!< Mutex for writing to the log */
  std::recursive_mutex settings_mutex;         /*!< Mutex for controlling the settings */
  std::ofstream log_stream_;                   /*!< Output log stream */
  std::string log_content_;                    /*!< Log contents */
  std::string log_path_;                       /*!< Log file path */
  bool is_periodic_;                           /*!< Is logging periodic? */
  bool logs_to_file_;                          /*!< Is logging to file? */
  bool logging_enabled_;                       /*!< Is this logger enabled? */
  int max_log_unit_;                           /*!< Maximum log unit in bytes */
  std::chrono::high_resolution_clock clock_;   /*!< High resolution clock */
};

#endif
