FROM fedora:42

# install packaging tools
RUN dnf -qy group install rpm-development-tools
RUN dnf -qy install fedora-rpm-macros wget gnupg2

# install build dependencies
RUN dnf -qy group install c-development
RUN dnf -qy install gtkmm3.0-devel

RUN rpmdev-setuptree

WORKDIR /tmp

# Import the key used to sign the packages
RUN gpg2 --receive-keys 2A8BF440D60CE9D60EC314B2E2FBBA1C0CFCF830
RUN gpg2 --output dkosmari.asc --armor --export 2A8BF440D60CE9D60EC314B2E2FBBA1C0CFCF830
RUN rpm --import dkosmari.asc

RUN wget -q https://github.com/dkosmari/libevdevxx/releases/download/v0.5.3/libevdevxx-static-devel-0.5.3-1.fc42.x86_64.rpm
RUN wget -q https://github.com/dkosmari/libgudevxx/releases/download/v0.3/libgudevxx-static-devel-0.3-1.fc42.x86_64.rpm

RUN dnf -qy install ./libevdevxx-static-devel-0.5.3-1.fc42.x86_64.rpm ./libgudevxx-static-devel-0.3-1.fc42.x86_64.rpm


WORKDIR /root/rpmbuild

COPY . .

CMD ["/bin/bash"]
