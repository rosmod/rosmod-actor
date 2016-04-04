/** @file    Component.hpp 
 *  @author  William Emfinger
 *  @author  Pranav Srinivas Kumar
 *  @date    <%- Date().toISOString().replace(/T/, ' ').replace(/\..+/, '') %>
 *  @brief   This file declares the Component class
 */

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <iostream>
#include <string>
#include "node/xmlParser.hpp"
#include "node/Logger.hpp"

#ifdef USE_ROSMOD
  #include "rosmod/rosmod_ros.h"
  #include "rosmod/rosmod_callback_queue.h"
#else
  #ifdef USE_ROSCPP
    #include "ros/ros.h"
    #include "ros/callback_queue.h"
  #endif
#endif

/**
 * @brief Component class
 */
class Component {
public:
  /**
   * @brief Component Constructor.
   * @param _config Component configuration parsed from deployment XML
   * @param[in] argc command-line argument count
   * @param[in] argv command-line arguments of the actor process
   */
  Component(ComponentConfig &_config, int argc, char **argv);

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
  virtual void init_timer_operation(const NAMESPACE::TimerEvent& event);

  /**
   * @brief Component Message Queue handler
   */
  void process_queue();

  /**
   * @brief Component Destructor
   */
  ~Component();

protected:
  ComponentConfig config;                /*!< Component Configuration */
  int node_argc;                         /*!< argc received by the actor process */
  char **node_argv;                      /*!< argv received by the actor process */
  NAMESPACE::Timer init_timer;           /*!< Initialization timer */
  NAMESPACE::CallbackQueue comp_queue;   /*!< Component Message Queue */
  std::unique_ptr<Logger> logger;        /*!< Component logger object */
};

#endif
