#!/bin/bash
install_dir="/opt/GasStationPro/"
#Add user to dialout(COM port)
sudo usermod -a -G dialout $USER

if ! [ -d ${install_dir} ]; then
sudo mkdir ${install_dir}
fi

#Build GasTeminal
pushd .
cd GasTeminal/gas_station
if [ -d build ]; then
rm -rf build
fi

cp -r settings/* ~/.GasStationPro
mkdir build
cd build/
qmake ../
make -j4
sudo mv gas_station ${install_dir}

### create gas_station.service 
printf "[Unit]

Description=gas_station
After=network.target

[Service]
ExecStart=/opt/GasStationPro/gas_station
ExecReload=/opt/GasStationPro/gas_station
WorkingDirectory=/home/%s/.GasStationPro/
Restart=always

[Install]
WantedBy=graphical-session.target" ${USER} >> gas_station.service
### end create gas_station.service 

sudo cp gas_station.service ${install_dir}
sudo ln -s -f ${install_dir}gas_station.service /etc/systemd/user/gas_station.service

popd
popd

#Run GUI demon
systemctl --user enable gas_station
systemctl --user daemon-reload
systemctl --user restart gas_station.service

#sudo reboot now
