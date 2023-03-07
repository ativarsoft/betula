FROM debian:bullseye
RUN apt-get update
RUN apt-get install -y git build-essential
RUN git clone https://github.com/ativarsoft/pollen-lang.git
WORKDIR pollen-lang
RUN make clean
RUN apt-get install -y $(cat dependencies.list)
RUN make
RUN make test
RUN make deb
RUN dpkg -i pollen-$(cat VERSION).deb
RUN make install-site
EXPOSE 80
CMD ["apache2ctl", "-D", "FOREGROUND"]
