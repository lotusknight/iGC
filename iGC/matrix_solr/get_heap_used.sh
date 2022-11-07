#!/bin/bash

# cassandra port:7199

#java -jar exe-jmxterm-1.1.0-uber.jar

#$>open localhost:7199
##Connection to host:jmxport is opened
#$>bean java.lang:type=Memory
##bean is set to java.lang:type=Memory
#$>run gc
##calling operation gc of mbean java.lang:type=Memory
##operation returns: 
#null
#$>quit
##bye


echo get -b java.lang:type=Memory HeapMemoryUsage | java -jar exe-jmxterm-1.1.0-uber.jar -n -l localhost:9010
