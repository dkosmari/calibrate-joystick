FROM archlinux:base-devel

# install build dependencies
RUN pacman -S --needed base-devel

WORKDIR /buildroot

COPY . .

CMD ["/bin/bash"]
