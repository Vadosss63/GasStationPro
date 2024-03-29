#!/bin/bash
install_dir="/opt/GasStationPro"
working_irectory=".GasStationPro"
#Add user to dialout(COM port)
sudo usermod -a -G dialout $USER

if [ -d ${install_dir} ]; then
sudo systemctl --user stop gas_station.service
sudo systemctl --user disable gas_station
sudo rm -rf ${install_dir}/gas_station
sudo rm -rf ${install_dir}/gas_station.service
fi
sudo mkdir -p ${install_dir} 
mkdir -p ~/${working_irectory}

#Build GasTeminal
cd GasTeminal/gas_station
if [ -d build ]; then
rm -rf build
fi

if [ ! -f ~/.GasStationPro/settings.json ]; then
cp settings/settings.json ~/.GasStationPro/settings.json
fi

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

echo "systemctl --user start gas_station.service" > ~/.config/lxsession/Lubuntu/autostart
#sudo reboot now
