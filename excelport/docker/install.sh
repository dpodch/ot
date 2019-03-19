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

echo "Remove docker image ${DOCKER_IMAGE_NAME}"
docker images -a | grep "${DOCKER_IMAGE_NAME}" | awk '{print $3}' | xargs docker rmi
docker load < ${DOCKER_IMAGE} || ERROR "Not load docker image ${DOCKER_IMAGE}"

/bin/bash ${RUN_SCRIPT} ${DOCKER_IMAGE_NAME}