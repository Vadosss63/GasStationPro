# GasStationPro

## Description
GasStationPro is an application developed using the Qt 5 framework. This graphical service interface is designed for a fuel dispensing terminal, which interacts with the user to manage the fuel pump. The application displays the amount of funds entered, allows the operation of the fuel pump, and sends terminal status statistics to a server while also receiving remote management commands from the server.

## Features
The application consists of 3 systemd services:
- `gas-station` - GUI for managing the fuel pump.
- `app-updater` - Service that allows remote updates of gas-station.
- `log-uploader` - Service that enables remote uploading of logs to the server.

## Requirements
To run the application, you will need:
- Qt 5.x

## Installation and Running
To build and run GasStationPro, follow these steps:

1. Clone the repository:
    ```
    git clone git@github.com:Vadosss63/GasStationPro.git
    ```

2. Navigate to the project directory:
    ```
    cd GasStationPro
    ```

3. Install all applications:
    ```
    ./install_all.sh
    ```

4. Install only gas-station:
    ```
    ./install_t_azs.sh
    ```

5. Install only app-updater:
    ```
    ./install_updater.sh
    ```

6. Install only log-uploader:
    ```
    ./install_log-uploader.sh
    ```

7. Running the application:
   After installation, all services start automatically. You can check the status of the services using systemctl commands.

## Using Custom Qt Libraries
This application uses dynamic linking to Qt libraries. You can replace the Qt libraries used by the application with your own versions if necessary. To do this, change the system environment variables or library path settings to point to different Qt library files.

## Unit Testing with CppUTest
This project uses CppUTest for unit testing, licensed under the BSD-3-Clause License. CppUTest provides a lightweight framework for creating unit tests for the application's C++ code.

### Building and Running Tests
To compile and run the unit tests included with this project, use the following steps:

Run form a root dir project:
./GasTeminal/gas_station/UnitTests/build_run_ut.sh

More details about CppUTest and its license can be found in the `LICENSE-CppUTest` file included in this repository.

## Licensing
This project is licensed under the GNU Lesser General Public License v3.0. You can find a copy of the license in the `LICENSE` file at the root of the repository. For more detailed information, see <https://www.gnu.org/licenses/lgpl-3.0.html>.

## Authors
- Vadim Parusov - QT/C++ Software Engineer
- Nikita Makarov - C++ Software Engineer

## Contact
If you have any questions or suggestions, please contact us at: parusovvadim@gmail.com
