/** @brief   Logger.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    <%- Date().toISOString().replace(/T/, ' ').replace(/\..+/, '') %>
 *  @brief   This file declares the Component Logger class
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

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
    max_log_unit_ = 1;
    log_content_ = "=================================================================\n";
  }

  /**
   * @brief Makes Logging Periodic.
   * @param[in] is_periodic boolean indicating whether logging is periodic.
   */
  void set_is_periodic(bool is_periodic) {
    is_periodic_ = is_periodic;
  }

  /**
   * @brief Set maximum log unit in bytes during periodic logging.
   * @param[in] max_log_unit maximum periodic logging unit in bytes.
   */
  void set_max_log_unit(int max_log_unit) {
    max_log_unit_ = max_log_unit;
  }

  /**
   * @brief Log to file instead of stdout.
   * @param[in] logs_to_file boolean requiring logging on file.
   */
  void set_logs_to_file(bool logs_to_file) {
    logs_to_file_ = logs_to_file;
  }

  /**
   * @brief Writes out the remainder of the logs and closes logfile.
   */
  ~Logger() {
    write();
    log_stream_.close();
  }

  /**
   * @brief Create log file
   * @param[in] log_path path to log file.
   */
  bool create_file(std::string log_path) {
    log_path_ = log_path;
    log_stream_.open(log_path_, std::ios::out | std::ios::app);
    logs_to_file_ = true;
    return true;
  }

  /**
   * @brief Write logged bytes to file
   */  
  bool write() {
    if (logs_to_file_) {
      log_stream_ << log_content_;
      log_stream_.flush();
    }
    else
      printf("%s", log_content_.c_str());
    return true;
  }

  /**
   * @brief Flush out to file.
   */  
  bool flush() {
    if (is_periodic_ && size() > max_log_unit_) {
      write();
      log_content_ = "";
      return true;
    }
    return false;
  }

  /**
   * @brief Log to file with specific log_level.
   * @param[in] log_level string indicating logging level.
   * @param[in] format varargs input to logger.
   */  
  bool log(std::string log_level, const char * format, ...) {
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

  /**
   * @brief Return the current size of the log in bytes.
   */  
  int size() {
    return log_content_.size();
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
  std::ofstream log_stream_;                   /*!< Output log stream */
  std::string log_content_;                    /*!< Log contents */
  std::string log_path_;                       /*!< Log file path */
  bool is_periodic_;                           /*!< Is logging periodic? */
  bool logs_to_file_;                          /*!< Is logging to file? */
  int max_log_unit_;                           /*!< Maximum log unit in bytes */
  std::chrono::high_resolution_clock clock_;   /*!< High resolution clock */
};

#endif
