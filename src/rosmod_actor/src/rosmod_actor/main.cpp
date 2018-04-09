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
#include "rosmod_actor/component.hpp"
#include "rosmod_actor/json.hpp"
#include "pthread.h"
#include "sched.h"
#include <iostream>
#include <fstream>

#include "ros/ros.h"

#include <signal.h>
#include <ros/ros.h>
#include <ros/xmlrpc_manager.h>

std::vector<Component*> comp_instances;

void rosmod_actor_SigInt_handler(int sig) {
  std::cout << "Received signal: " << sig << std::endl;
  std::cout << "Destroying " << comp_instances.size() << " components!" << std::endl;
  for (int i=0; i < comp_instances.size(); i++) {
    delete comp_instances[i];
  }
}

void componentThreadFunc(Component* compPtr)
{
  compPtr->startUp();
  compPtr->process_queue();
}

std::vector<boost::thread*> compThreads;	

void printHelp() {
  ROS_INFO_STREAM("\nUsage:  rosmod_actor\n" <<
		  "\t--config <json config file>\n" <<
		  "\t--help   (show this help and exit)");
}

/**
 * @brief Parses node configuration and spawns component executor threads.
 *
 */
int main(int argc, char **argv)
{
  std::string nodeName = "";
  std::string configFile = "";

  for(int i = 0; i < argc; i++)
  {
    if(!strcmp(argv[i], "--config"))
      configFile = argv[i+1];
    if(!strcmp(argv[i], "--help")) {
      printHelp();
      return 0;
    }
  }

  if (argc == 0 || !configFile.length()) {
    ROS_ERROR_STREAM("No config file provided!");
    printHelp();
    return 0;
  }


  Json::Value root;
  try {
    std::ifstream configuration(configFile, std::ifstream::binary);
    configuration >> root;
  } catch (std::exception& e) {
    ROS_ERROR_STREAM( std::string("Exception caught trying to open / parse config file: ") << e.what() );
  } catch ( ... ) {
    ROS_ERROR_STREAM("Unhandled exception caught trying to open / parse config file!");
  }

  nodeName = root["Name"].asString();
  ros::init(argc, argv, nodeName.c_str(), ros::init_options::NoSigintHandler);
  signal(SIGINT, rosmod_actor_SigInt_handler);

  ROS_INFO_STREAM( std::string("Root Node name: ") << root["Name"].asString() << std::endl);
  ROS_INFO_STREAM( std::string("Root Node priority: ") << root["Priority"].asInt() << std::endl);

  // Create Node Handle
  ros::NodeHandle n;
  ROS_INFO_STREAM(nodeName << " thread id = " << boost::this_thread::get_id());

  int ret;
  pthread_t this_thread = pthread_self(); 

  // struct sched_param is used to store the scheduling priority
  struct sched_param params;     
  // We'll set the priority to the maximum.
  params.sched_priority = root["Priority"].asInt();  
  if (params.sched_priority < 0)
    params.sched_priority = sched_get_priority_max(SCHED_RR);

  ROS_INFO_STREAM("Trying to set thread realtime prio = " << 
		  params.sched_priority << std::endl);
     
  // Attempt to set thread real-time priority to the SCHED_FIFO policy     
  ret = pthread_setschedparam(this_thread, SCHED_RR, &params);     
  if (ret != 0)
    ROS_ERROR_STREAM("Unsuccessful in setting thread realtime prio" << std::endl);

  // Now verify the change in thread priority     
  int policy = 0;     
  ret = pthread_getschedparam(this_thread, &policy, &params);
  if (ret != 0)          
    ROS_ERROR_STREAM("Couldn't retrieve real-time scheduling paramers" << std::endl);
  // Check the correct policy was applied     
  if(policy != SCHED_RR)
    ROS_ERROR_STREAM("Scheduling is NOT SCHED_RR!" << std::endl);
  else
    ROS_INFO_STREAM("SCHED_RR OK" << std::endl);
  // Print thread scheduling priority     
  ROS_INFO_STREAM("Thread priority is " << params.sched_priority << std::endl);
    
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

    comp_instances.push_back(comp_inst);
    
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

