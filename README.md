# Project IDP
A university project.


### Prerequisites
to build this project you will need the following programs installed:
* gcc
* make
* opencv (3)
* opencv-dev
* wiringPi
* pkg-config


### Installing
to build this program you first need a local copy of it:
```bash
# Clone the github repository
$ git clone https://github.com/thefellowes/projectIDP.git

# Go inside the repository
$ cd projectIDP/
$ cd rpi_server/

# To actually install it run:
$ make
$ sudo make install
```
Before you can actually run our program you need to make sure you have added:
```
gpu_mem=128
start_file=start_x.elf
fixup_file=fixup_x.dat

# the following only if you have an raspberry pi 3
dtoverlay=pi3-miniuart-bt
```
to /boot/config.txt
and to /etc/rc.local
```
modprobe bcm2835-v4l2
```

To actually run the server
```bash
$ rpi_server
```
