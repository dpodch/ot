FROM ot-boost

USER root
COPY repo /repo
WORKDIR /repo
RUN yum install -y *.rpm
RUN rm -r /usr/share/qt5/qtlogging.ini

WORKDIR /opt/excelport/

CMD ["./excelport_srv", "-p", "12100", "-t", "10", "-d", "2"]
