FROM debian:trixie

# Install packaging tools
RUN apt -q -y update
RUN apt -q -y upgrade
RUN apt -q -y install devscripts
RUN apt -q -y install wget

# Install dependencies
RUN apt -q -y install build-essential autoconf-archive pkg-config libgtkmm-3.0-dev

WORKDIR /tmp

# Install extra dependencies.
RUN wget -q https://github.com/dkosmari/libevdevxx/releases/download/v0.5.3/libevdevxx-static-dev_0.5.3-1_amd64.deb
RUN wget -q https://github.com/dkosmari/libgudevxx/releases/download/v0.3/libgudevxx-static-dev_0.3-1_amd64.deb
RUN apt install -qy ./libevdevxx-static-dev_0.5.3-1_amd64.deb ./libgudevxx-static-dev_0.3-1_amd64.deb


WORKDIR /buildroot

COPY . .

CMD ["/bin/bash"]
