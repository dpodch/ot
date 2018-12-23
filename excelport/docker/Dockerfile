FROM centos/devtoolset-6-toolchain-centos7

USER root
RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
COPY repo /repo
WORKDIR /repo
RUN yum install -y *.rpm

WORKDIR /opt/excelport/

CMD ["./excelport_srv", "--config", "excelport_srv.xml"]
