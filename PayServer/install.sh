#!/bin/bash

if ! [ -d /opt/car_wash/ ]; then
sudo mkdir /opt/car_wash/
sudo cp car_wash.service /opt/car_wash/
sudo ln -s -f /opt/car_wash/car_wash.service /etc/systemd/system/car_wash.service
fi

go build ./main.go
sudo mv main /opt/car_wash/car_wash
systemctl enable car_wash
sudo systemctl restart car_wash.service

#sudo reboot now