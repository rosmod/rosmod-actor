/** @file    Component.hpp 
 *  @author  William Emfinger
 *  @author  Pranav Srinivas Kumar
 *  @date    2017-04-17
 *  @brief   This file declares the Component class
 */

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <iostream>
#include <string>
#include <std_msgs/Bool.h>
#include "rosmod_actor/logger.hpp"
#include "rosmod_actor/json.hpp"

#include "ros/ros.h"

/**
 * @brief Component class
 */
class Component {
public:
  /**
   * @brief Component Constructor.
   * @param[in] _config Component configuration parsed from deployment JSON
   */
  Component(Json::Value& _config);

  /**
   * @brief Component Destructor
   */
  virtual ~Component();

  /**
   * @brief Component startup function
   *
   * This function configures all the component ports and timers
   */ 
  virtual void startUp() = 0;

  /**
   * @brief Component Initializer
   * This operation is executed immediately after startup.
   * @param[in] event a oneshot timer event
   * @see startUp()
   */
  virtual void init_timer_operation(const ros::TimerEvent& event) = 0;

  /**
   * @brief Component Message Queue handler
   */
  virtual void process_queue();

protected:
  Json::Value              config;      /*!< Component Configuration */
  ros::Timer               init_timer;  /*!< Initialization timer */
  ros::CallbackQueue       comp_queue;  /*!< Component Message Queue */
  std::unique_ptr<Logger>  logger;      /*!< Component logger object */
  std::unique_ptr<Logger>  trace;       /*!< Component trace logger object */
  std::string              workingDir;  /*!< Working directory of the process */
};

#endif
