FROM i386/ubuntu:18.04

RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential:i386 \
    g++:i386 \
    qt5-qmake:i386 \
    qt5-default:i386 \
    qtbase5-dev:i386 \
    qtchooser:i386 \
    qtbase5-dev-tools:i386 \
    libqt5serialport5:i386 \
    libqt5serialport5-dev:i386 \
    cpputest:i386 \
    libcpputest-dev:i386 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /var/work/GasStationPro

CMD ["bash"]

# docker run -it --rm -v $(pwd):/var/work/GasStationPro t-azs /bin/bash -c "rm -rf /var/work/GasStationPro/GasTeminal/gas_station/build && \
#     mkdir -p /var/work/GasStationPro/GasTeminal/gas_station/build && \
#     cd /var/work/GasStationPro/GasTeminal/gas_station/build && \
#     qmake ../  && \
#     make -j4"