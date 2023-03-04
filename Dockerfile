FROM debian:bullseye
RUN ["apt-get install git"]
RUN ["git clone https://github.com/ativarsoft/pollen-lang.git"]
RUN ["make clean"]
RUN ["make dependencies"]
RUN ["make"]
RUN ["make test"]
RUN ["make deb"]
RUN ["make install"]
EXPOSE 80
CMD [“apache2ctl”, “-D”, “FOREGROUND”]
