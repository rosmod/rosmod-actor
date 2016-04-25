/** @file    node_main.cpp 
 *  @author  William Emfinger
 *  @date    <%- Date().toISOString().replace(/T/, ' ').replace(/\..+/, '') %>
 *  @brief   This file contains the main function for a ROSMOD actor/node.
 */

#include <cstdlib>
#include <string.h>
#include <dlfcn.h>
#include <exception>      
#include <cstdlib>        
#include <signal.h>
#include <boost/thread.hpp>
#include "node/Component.hpp"
#include "json/json.h"
#include "pthread.h"
#include "sched.h"
#include <iostream>
#include <fstream>

#ifdef USE_ROSMOD
  #include "rosmod/rosmod_ros.h"
#else
  #ifdef USE_ROSCPP
    #include "ros/ros.h"
  #endif
#endif

void componentThreadFunc(Component* compPtr)
{
  compPtr->startUp();
  compPtr->process_queue();
}

std::vector<boost::thread*> compThreads;	

/**
 * @brief Parses node configuration and spawns component executor threads.
 *
 */
int main(int argc, char **argv)
{
  std::string nodeName = "node";
  std::string hostName = "localhost";
  std::string configFile = "";

  for(int i = 0; i < argc; i++)
  {
    if(!strcmp(argv[i], "--nodename"))
      nodeName = argv[i+1];
    if(!strcmp(argv[i], "--hostname"))
      hostName = argv[i+1];
    if(!strcmp(argv[i], "--config"))
      configFile = argv[i+1];
  }

  Json::Value root;
  std::ifstream configuration(configFile, std::ifstream::binary);
  configuration >> root;

  std::cout << "Root Node name: " << root["Name"].asString() << std::endl;
  std::cout << "Root Node priority: " << root["Priority"].asInt() << std::endl;
  for (unsigned int i = 0; i < root["Component Instances"].size(); i++) {
    std::cout << root["Component Instances"][i]["Logging"]["Enabled"].asBool() << std::endl;
  }

  int ret;
  pthread_t this_thread = pthread_self(); 

  // struct sched_param is used to store the scheduling priority
  struct sched_param params;     
  // We'll set the priority to the maximum.
  params.sched_priority = root["Priority"].asInt();  
  if (params.sched_priority < 0)
    params.sched_priority = sched_get_priority_max(SCHED_RR);

  std::cout << "Trying to set thread realtime prio = " << 
    params.sched_priority << std::endl; 
     
  // Attempt to set thread real-time priority to the SCHED_FIFO policy     
  ret = pthread_setschedparam(this_thread, SCHED_RR, &params);     
  if (ret != 0)
    std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;

  // Now verify the change in thread priority     
  int policy = 0;     
  ret = pthread_getschedparam(this_thread, &policy, &params);     
  if (ret != 0)          
    std::cout << "Couldn't retrieve real-time scheduling paramers" << std::endl;      
  // Check the correct policy was applied     
  if(policy != SCHED_RR)         
    std::cout << "Scheduling is NOT SCHED_RR!" << std::endl;     
  else
    std::cout << "SCHED_RR OK" << std::endl;     
  // Print thread scheduling priority     
  std::cout << "Thread priority is " << params.sched_priority << std::endl;

  nodeName = root["Name"].asString();
  NAMESPACE::init(argc, argv, nodeName.c_str());

  // Create Node Handle
  NAMESPACE::NodeHandle n;

  ROS_INFO_STREAM(nodeName << " thread id = " << boost::this_thread::get_id());
    
  for (unsigned int i = 0; i < root["Component Instances"].size(); i++) {
    std::string libraryLocation = root["Component Instances"][i]["Definition"].asString();
    void *hndl = dlopen(libraryLocation.c_str(), RTLD_NOW);
    if(hndl == NULL) {
      std::cerr << dlerror() << std::endl;
      exit(-1);
    }
    void *mkr = dlsym(hndl, "maker");
    Component *comp_inst = ((Component *(*)(Json::Value&))(mkr))
      (root["Component Instances"][i]);
    
    // Create Component Threads
    boost::thread *comp_thread = new boost::thread(componentThreadFunc, comp_inst);
    compThreads.push_back(comp_thread);
    ROS_INFO_STREAM(nodeName << " has started " << root["Component Instances"][i]["Name"]);
  }
  for (int i=0;i<compThreads.size();i++) {
    compThreads[i]->join();
  }
  return 0; 
}

