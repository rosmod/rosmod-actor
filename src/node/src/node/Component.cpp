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

  // Identify the pwd of Node Executable
  char szTmp[32];
  char pBuf[1024];
  memset(pBuf, 0, 1024);
  sprintf(szTmp, "/proc/%d/exe", getpid());
  readlink(szTmp, pBuf, 1024);

  std::string s = pBuf;
  std::string exec_path = s;
  std::string delimiter = "/";
  std::string exec;
  size_t pos = 0;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    exec = s.substr(0, pos);
    s.erase(0, pos + delimiter.length());
  }
  exec = s.substr(0, pos);
  // now copy the found directory to the component's workingDir variable
  workingDir = exec_path.erase(exec_path.find(exec), exec.length());
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
