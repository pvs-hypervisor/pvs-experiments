#!/bin/bash

rmmod drv1
rmmod drv2
sleep 1
echo > /var/log/syslog
insmod drv1/drv1.ko
insmod drv2/drv2.ko
echo "sucesso!"
