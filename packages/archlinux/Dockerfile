FROM archlinux:base-devel

# install build dependencies
RUN pacman -Sy --needed base-devel

WORKDIR /buildroot

COPY . .

CMD ["/bin/bash"]
