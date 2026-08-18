FROM archlinux:base-devel

# install build dependencies
RUN pacman -Sy --needed base-devel sudo

RUN useradd -m user
RUN echo "user ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

USER user
WORKDIR /buildroot

COPY . .

CMD ["/bin/bash"]
