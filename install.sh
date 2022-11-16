#!/bin/bash
install_dir="/opt/GasStationPro/"

if ! [ -d ${install_dir} ]; then
sudo mkdir ${install_dir}
#sudo cp car_wash.service /opt/car_wash/
#sudo ln -s -f /opt/car_wash/car_wash.service /etc/systemd/system/car_wash.service
fi

#Build GasTeminal
pushd .
cd GasTeminal/gas_station
if [ -d build ]; then
rm -rf build
fi
sudo cp gas_station.service ${install_dir}
mkdir build
cd build/
qmake ../
make -j4
sudo mv gas_station ${install_dir}


sudo ln -s -f ${install_dir}gas_station.service /etc/systemd/user/gas_station.service

popd
popd


#Build Terminal 
#sudo apt install golang
go env -w GO111MODULE=off
go get github.com/jacobsa/go-serial/serial
pushd .
cd PayServer/
go build ./pay_server.go
sudo mv pay_server ${install_dir}
echo popd
echo popd

#Run GUI demon
systemctl --user daemon-reload
systemctl --user restart gas_station.service

#sudo reboot now
