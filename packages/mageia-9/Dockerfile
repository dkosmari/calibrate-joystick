FROM mageia:9

RUN urpmi -q --auto-update --auto

# install packaging tools
RUN urpmi -q --auto rpmdevtools bm wget gnupg2

# install build dependencies
RUN urpmi -q --auto task-c++-devel
RUN urpmi -q --auto lib64gtkmm3.0-devel

WORKDIR /tmp

# Import the key used to sign the packages
RUN gpg --import-key 2A8BF440D60CE9D60EC314B2E2FBBA1C0CFCF830
RUN gpg --output dkosmari.asc --export 2A8BF440D60CE9D60EC314B2E2FBBA1C0CFCF830
RUN rpm --import dkosmari.asc

RUN wget -q https://github.com/dkosmari/libevdevxx/releases/download/v0.5.3/lib64evdevxx-static-devel-0.5.3-1.mga9.x86_64.rpm
RUN wget -q https://github.com/dkosmari/libgudevxx/releases/download/v0.3/lib64gudevxx-static-devel-0.3-1.mga9.x86_64.rpm

RUN urpmi -q --auto ./lib64evdevxx-static-devel-0.5.3-1.mga9.x86_64.rpm ./lib64gudevxx-static-devel-0.3-1.mga9.x86_64.rpm


WORKDIR /buildroot

COPY . .

CMD ["/bin/bash"]
