FROM archlinux:latest

# install build dependencies
RUN pacman -Syu --noconfirm \
    autoconf \
    automake \
    base-devel \
    gcc \
    gettext \
    gtkmm3 \
    intltool \
    libevdev \
    libgudev \
    libtool \
    pkgconf \
    sudo

RUN useradd -m user
RUN echo "user ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

USER user
WORKDIR /buildroot

COPY . .

CMD ["/bin/bash"]
