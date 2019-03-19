#!/bin/bash

CONTAINER_NAME=ot-srv

if [ ! -z "$1" ]; then
	CONTAINER_NAME=$1
fi

echo "##Stopping docker container ${CONTAINER_NAME}..."
docker rm $(docker stop $(docker ps -a -q --filter ancestor=${CONTAINER_NAME} --format="{{.ID}}"))
echo "##Stopping docker container ${CONTAINER_NAME} Done"
echo "##Starting docker container ${CONTAINER_NAME} Done"
docker run --restart=always --log-driver=syslog --log-opt tag="EXCELPORT" -d -p 12100:12100 ${CONTAINER_NAME}
echo "##Starting docker container ${CONTAINER_NAME} Done"