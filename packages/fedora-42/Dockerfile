FROM fedora:42

# install packaging tools
RUN dnf -qy group install rpm-development-tools
RUN dnf -qy install fedora-rpm-macros wget

# install build dependencies
RUN dnf -qy group install c-development
RUN dnf -qy install gtkmm3.0-devel

RUN rpmdev-setuptree

WORKDIR /root/rpmbuild

COPY . .

CMD ["/bin/bash"]
