A Generic ROSMOD node
======================

This is a generic ROSMOD node package that maps to an actor process and can parse a ROSMOD deployment XML to dynamically load and spawn component instance executor threads. The node needs to be run with administrative permissions in order to execute at real-time priority.

Build Library:
-------------

```bash
git clone http://github.com/rosmod/rosmod-actor
cd rosmod-actor
catkin_make -DNAMESPACE=rosmod
```

