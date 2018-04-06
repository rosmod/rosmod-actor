## ROSMOD Actor - A Generic ROSMOD Node

This is a generic ROSMOD node package that maps to an actor process
and can parse a ROSMOD deployment JSON to dynamically load and spawn
component instance executor threads. ROSMOD-Actor has support for
running with real-time priority in linux and can set its real-time
priority that is defined in the configuration json.  It will need to
be run with administrative permissions in order to execute at
real-time priority.

### Dependencies

Install these dependencies according to their install instructions.

* [ROS](http://www.ros.org)
* [Catkin Tools](https://github.com/catkin/catkin_tools)

### Downloading

```bash
# clone
git clone https://github.com/rosmod/rosmod-actor
# enter repository
cd rosmod-actor
```

### Configuration

```bash
# chown /opt for the user
sudo chown -R $USER /opt
# set rosmod-actor to extend your ROS workspace
catkin config --extend /opt/ros/kinetic
# set the install location for rosmod-actor (you can change this to be wherever you like)
catkin config -i /opt/rosmod-actor
# configure rosmod-actor to actually install
catkin config --install
```

### Build and Install

```bash
# Remove any previous build files that may exist
catkin clean -b --yes
# build and install according to config
catkin build
```
