#!/bin/bash
install_dir="/opt/GasStationPro"
working_irectory=".GasStationPro"
#Add user to dialout(COM port)
sudo usermod -a -G dialout $USER

if [ -d ${install_dir} ]; then
sudo systemctl --user stop gas_station.service
sudo systemctl --user disable gas_station
sudo rm -rf ${install_dir}
sudo rm -rf ~/${working_irectory}
fi
sudo mkdir ${install_dir} 
mkdir ~/${working_irectory}

#Build GasTeminal
cd GasTeminal/gas_station
if [ -d build ]; then
rm -rf build
fi

cp -r settings/* ~/.GasStationPro

mkdir build
cd build/
qmake ../
make -j4

sudo mv gas_station ${install_dir}/

### create gas_station.service 
printf "[Unit]

Description=gas_station
After=network.target

[Service]
ExecStart=%s/gas_station
ExecReload=%s/gas_station
WorkingDirectory=/home/%s/%s
Restart=always

[Install]
WantedBy=graphical-session.target" ${install_dir} ${install_dir} ${USER} ${working_irectory} > gas_station.service
### end create gas_station.service 

sudo mv gas_station.service ${install_dir}/
sudo ln -s -f ${install_dir}/gas_station.service /etc/systemd/user/gas_station.service

#Run GUI demon
systemctl --user enable gas_station
systemctl --user daemon-reload
systemctl --user restart gas_station.service

#sudo reboot now
