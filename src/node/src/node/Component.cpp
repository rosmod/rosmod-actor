/** @file    Component.cpp 
 *  @author  William Emfinger
 *  @author  Pranav Srinivas Kumar
 *  @date    <%- Date().toISOString().replace(/T/, ' ').replace(/\..+/, '') %>
 *  @brief   This file contains definitions for the base Component class
 */

#include "node/Component.hpp"

// Constructor
Component::Component(Json::Value& _config) {
  logger.reset(new Logger());
  config = _config;
}

// Destructor
Component::~Component() {
  comp_queue.disable();
  init_timer.stop();
}

// Initialization
void Component::init_timer_operation(const NAMESPACE::TimerEvent& event) {}

// Component Operation Queue Handler
void Component::process_queue() {  
  NAMESPACE::NodeHandle nh;
  while (nh.ok())
    this->comp_queue.callAvailable(ros::WallDuration(0.01));
}
