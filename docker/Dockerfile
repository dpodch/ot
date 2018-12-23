FROM centos:7
ENV container docker

USER root

WORKDIR /tmp
RUN yum install -y wget
RUN rpm -Uhv https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
RUN yum -y install centos-release-scl
RUN yum-config-manager --enable rhel-server-rhscl-7-rpms
RUN yum -y install devtoolset-6
 
#RUN yum install -y https://download1.rpmfusion.org/free/el/rpmfusion-free-release-7.noarch.rpm

#RUN yum --disablerepo=nux-dextop install -y vlc-devel vlc-core vlc
RUN yum install -y rpm-build qt-devel cmake3 qtwebkit-devel qt-mobility-devel zlib-devel gcc gcc-c++ redhat-lsb
RUN yum install -y libharu-devel php-devel php-cli libxml2-devel vlc-devel subversion cyrus-sasl-plain cyrus-sasl-md5 
RUN yum install -y make curl-devel git

RUN mkdir /src
WORKDIR /src
RUN git clone https://github.com/tfussell/xlnt.git

RUN mkdir /src/xlnt/_build
WORKDIR /src/xlnt/_build
RUN scl enable devtoolset-6 bash && g++ --version && cmake3 ../ && make -j3

CMD ["bash"]
