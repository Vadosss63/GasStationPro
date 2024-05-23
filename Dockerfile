FROM i386/ubuntu:18.04

RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential:i386 \
    ca-certificates:i386 \
    cmake:i386 \
    g++:i386 \
    git:i386 \
    qt5-qmake:i386 \
    qt5-default:i386 \
    qtbase5-dev:i386 \
    qtchooser:i386 \
    qtbase5-dev-tools:i386 \
    libqt5serialport5:i386 \
    libqt5serialport5-dev:i386 \
    pkg-config:i386 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /var/work

RUN git clone --depth 1 --branch v4.0 \
    https://github.com/cpputest/cpputest.git

WORKDIR /var/work/cpputest

RUN mkdir buildcpput && \
    cd buildcpput && \
    cmake .. && \
    make install

WORKDIR /var/work/GasStationPro

RUN rm -rf /var/work/cpputest

CMD ["bash"]

# Build Docker image:
# sudo docker build -t t-azs .

# Build GasStation:
# docker run -it --rm -v $(pwd):/var/work/GasStationPro t-azs /bin/bash -c "rm -rf /var/work/GasStationPro/GasTeminal/gas_station/build && \
#     mkdir -p /var/work/GasStationPro/GasTeminal/gas_station/build && \
#     cd /var/work/GasStationPro/GasTeminal/gas_station/build && \
#     qmake ../  && \
#     make -j4"

# Build and Run UTs:
# docker run -it --volume /home/makarov/proj/GasStationPro/:/var/work/GasStationPro \
# --entrypoint /var/work/GasStationPro/GasTeminal/gas_station/UnitTests/build_run_ut.sh  t-azs
