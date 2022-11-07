#!/bin/bash
cd /sys/fs/cgroup
mkdir cpu/yarn
chown -R zhao:root cpu/yarn
mkdir blkio/yarn
chown -R zhao:root blkio/yarn
mkdir memory/yarn
chown -R zhao:root memory/yarn
mkdir net_cls/yarn
chown -R zhao:root net_cls/yarn
