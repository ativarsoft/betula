FROM debian:bookworm
RUN apt-get update
RUN apt-get install apt-utils
RUN apt-get install -y git build-essential
RUN git clone https://github.com/ativarsoft/betula.git
WORKDIR betula
RUN make clean
RUN apt-get install -y $(cat dependencies.list)
RUN make
RUN make test
RUN make deb
RUN dpkg -i pollen-$(cat VERSION).deb
EXPOSE 80
CMD ["apache2ctl", "-D", "FOREGROUND"]
