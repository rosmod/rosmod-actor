/** @file    Component.cpp 
 *  @author  William Emfinger
 *  @author  Pranav Srinivas Kumar
 *  @date    April 6 2018
 *  @brief   This file contains definitions for the base Component class
 */

#include "rosmod_actor/component.hpp"
#include <unistd.h>

// Constructor
Component::Component(Json::Value& _config) {
  logger.reset(new Logger());
  trace.reset(new Logger());
  config = _config;

  // Identify the pwd of Node Executable
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    workingDir = cwd;
  }
  else {
    ROS_ERROR_STREAM("Couldn't get CWD!");
  }
}

// Destructor
Component::~Component() {
  std::cout << "~Component() for " <<
    config["Name"].asString() <<
    " - writing out logs!" << "\n";
  comp_queue.disable();
  // make sure all user logs are written
  logger->write();
  // make sure all trace logs are written
  trace->write();
}

// Component Operation Queue Handler
void Component::process_queue() {  
  while (nh_.ok())
    this->comp_queue.callAvailable(ros::WallDuration(0.01));
}
