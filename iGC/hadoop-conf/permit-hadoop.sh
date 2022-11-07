#!/bin/bash
echo "can't change the permission for hadoop to root:zhao, do it manually"
cd /opt
sudo chown -R root:zhao ./hadoop-2.9.2
cd hadoop-2.9.2
sudo chown -R zhao:zhao ./
sudo chown root:zhao ./etc
sudo chown root:zhao ./etc/hadoop
sudo chown root:zhao ./etc/hadoop/container-executor.cfg
sudo chown root:zhao ./bin/container-executor
sudo chmod +s  ./bin/container-executor
