#!/bin/bash

ERROR ()
{
	echo "!!!!! ERROR: $* !!!!!"
	exit 1
}

RSYSLOG=rsyslog
DOCKER=docker
DOCKER_IMAGE_NAME="ot-srv"
DOCKER_IMAGE="${DOCKER_IMAGE_NAME}.tar"
RUN_SCRIPT=run_docker.sh

SERVICES_LIST=(${RSYSLOG} ${DOCKER})
PKG_LIST=(${RSYSLOG} ${DOCKER})

[ ! -f ${DOCKER_IMAGE} ] && ERROR "Docker image not exists ${DOCKER_IMAGE}"

for i in "${PKG_LIST[@]}"
do
	yum -y install ${i} || ERROR "install ${i}"
	echo "###installed ${i}"
done

for i in "${SERVICES_LIST[@]}"
do
	systemctl is-enabled --quiet ${i} || systemctl enable ${i} || ERROR "enable ${i}"
	systemctl is-active --quiet ${i} || systemctl restart ${i} || ERROR "restart ${i}"
	echo "###activated service ${i}"
done


echo "##Stopping and removing docker container ${DOCKER_IMAGE_NAME}..."
#docker rm $(docker stop $(docker ps -a -q --filter ancestor=${DOCKER_IMAGE_NAME} --format="{{.ID}}"))
docker ps -a -q --filter ancestor=${DOCKER_IMAGE_NAME} --format="{{.ID}}" \
	| xargs --no-run-if-empty docker stop \
	| xargs --no-run-if-empty docker rm \
		|| ERROR "stop docker ${DOCKER_IMAGE_NAME}"
echo "##Stopping and removing docker container ${DOCKER_IMAGE_NAME} Done"


echo "### Remove docker image ${DOCKER_IMAGE_NAME} ..."
docker images -a | grep "${DOCKER_IMAGE_NAME}" | awk '{print $3}' | xargs --no-run-if-empty docker rmi \
	 || ERROR "remove docker image ${DOCKER_IMAGE_NAME}"
echo "### Remove docker image ${DOCKER_IMAGE_NAME} Done"

docker load < ${DOCKER_IMAGE} || ERROR "Not load docker image ${DOCKER_IMAGE}"

echo "##Starting docker container ${DOCKER_IMAGE_NAME} Done"
docker run --restart=always --log-driver=syslog --log-opt tag="EXCELPORT" -d -p 12100:12100 ${DOCKER_IMAGE_NAME}
echo "##Starting docker container ${DOCKER_IMAGE_NAME} Done"

echo "DONE"