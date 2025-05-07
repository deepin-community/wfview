# How to install wfview

### 1. Install prerequisites:
(Note, some packages may have slightly different version numbers, this should be ok for minor differences.)
~~~
sudo apt-get install build-essential
sudo apt-get install qt5-qmake
sudo apt-get install qt5-default
sudo apt-get install libqt5core5a
sudo apt-get install qtbase5-dev
sudo apt-get install libqt5serialport5 libqt5serialport5-dev
sudo apt-get install libqt5multimedia5
sudo apt-get install libqt5multimedia5-plugins
sudo apt-get install qtmultimedia5-dev
sudo apt-get install git 
sudo apt-get install libopus-dev
sudo apt-get install libeigen3-dev
sudo apt-get install portaudio19-dev
sudo apt-get install librtaudio-dev
sudo apt-get install libhidapi-dev libqt5gamepad5-dev
sudo apt-get install libudev-dev
~~~
Now you need to install qcustomplot. There are two versions that are commonly found in linux distros: 1.3 and 2.0. Either will work fine. If you are not sure which version your linux install comes with, simply run both commands. One will work and the other will fail, and that's fine!

qcustomplot1.3 for older linux versions (Linux Mint 19.x, Ubuntu 18.04): 

~~~
sudo apt-get install libqcustomplot1.3 libqcustomplot-doc libqcustomplot-dev
~~~

qcustomplot2 for newer linux versions (Linux Mint 20, Ubuntu 19, Rasbian V?, Debian 10):

~~~
sudo apt-get install libqcustomplot2.0 libqcustomplot-doc libqcustomplot-dev
~~~

optional for those that want to work on the code using the QT Creator IDE: 
~~~
sudo apt-get install qtcreator qtcreator-doc
~~~

### 2. Clone wfview to a local directory on your computer:
~~~ 
cd ~/Documents
git clone https://gitlab.com/eliggett/wfview.git
~~~

### 3. Create a build directory, compile, and install:
If you want to change the default install path from `/usr/local` to a different prefix (e.g. `/opt`), you must call `qmake ../wfview/wfview.pro PREFIX=/opt`

~~~
mkdir build
cd build
qmake ../wfview/wfview.pro
make -j
sudo make install
~~~

### 4. You can now launch wfview, either from the terminal or from your desktop environment. If you encounter issues using the serial port, run the following command: 
~~~

if you are using the wireless 705 or any networked rig like the 7610, 7800, 785x, there is no need to use USB so below is not needed.

sudo chown $USER /dev/ttyUSB*

Note, on most linux systems, you just need to add your user to the dialout group, which is persistent and more secure:

~~~
sudo usermod -aG dialout $USER 
~~~
(don't forget to log out and log in)

~~~

### opensuse/sles/tumbleweed install
---

install wfview on suse 15.3 & up, sles 15.x or tumbleweed; this was done on a clean install/updated OS. 


There are two options, QT5 (out of the box)  and QT6 (almost out of the box)


for qt5 and 6:

~~~
sudo zypper in --type pattern devel_basis
~~~

for qt5 only:

~~~
sudo zypper in libQt5Widgets-devel libqt5-qtbase-common-devel libqt5-qtserialport-devel libQt5SerialPort5 qcustomplot-devel libqcustomplot2 libQt5PrintSupport-devel libqt5-qtmultimedia-devel lv2-devel libopus-devel eigen3-devel libQt5Xml-devel portaud
io-devel rtaudio-devel libqt5-qtgamepad-devel libQt5Gamepad5 libqt5-qtwebsockets-devel libqt5-qtgamepad-devel portaudio-devel rtaudio-devel libhidapi-devel

mkdir -p ~/src/build
cd ~/src
git clone https://gitlab.com/eliggett/wfview.git
cd build
qmake-qt5 ../wfview/wfview.pro
make
sudo make install
~~~


for qt6 only:
~~~
sudo zypper in qt6-base-common-devel qt6-gui-devel qt6-serialport-devel qt6-multimedia-devel qt6-printsupport-devel qt6-websockets-devel qt6-widgets-devel rtaudio-devel libhidapi-devel qt6-xml-devel
~~~

as there is no qt6 version so far for qcustomplot yes for suse:

download qnd rpm -ivh customplot-qt5 qcustomplot-qt5-devel qcustomplot-qt6 qcustomplot-qt6-devel rpm's from fedora41
(or better, add repo and install from there)

Now:

~~~

mkdir -p ~/src/build
cd ~/src
git clone https://gitlab.com/eliggett/wfview.git
cd build
qmake6 ../wfview/wfview.pro
make
~~~

the linking fails because of slightly different cusomplot names so:
~~~

g++ -O2 -s -Wl,-O1 -Wl,-rpath,/usr/lib64 -Wl,-rpath-link,/usr/lib64 -o wfview  main.o bandbuttons.o cachingqueue.o cwsender.o firsttimesetup.o freqctrl.o frequencyinputwidget.o cwsidetone.o debugwindow.o icomcommander.o loggingwindow.o scrolltest.o settingswidget.o memories.o rigcreator.o spectrumscope.o tablewidget.o tciaudiohandler.o tciserver.o wfmain.o commhandler.o rigcommander.o freqmemory.o rigidentities.o udpbase.o udphandler.o udpcivdata.o udpaudio.o logcategories.o pahandler.o rthandler.o audiohandler.o audioconverter.o calibrationwindow.o satellitesetup.o udpserver.o meter.o qledlabel.o pttyhandler.o resample.o repeatersetup.o rigctld.o usbcontroller.o controllersetup.o selectradio.o tcpserver.o cluster.o database.o aboutbox.o audiodevices.o qrc_style.o qrc_resources.o moc_wfmain.o moc_bandbuttons.o moc_cachingqueue.o moc_commhandler.o moc_cwsender.o moc_firsttimesetup.o moc_freqctrl.o moc_frequencyinputwidget.o moc_cwsidetone.o moc_debugwindow.o moc_icomcommander.o moc_loggingwindow.o moc_memories.o moc_rigcommander.o moc_rigcreator.o moc_scrolltest.o moc_settingswidget.o moc_spectrumscope.o moc_tablewidget.o moc_tciaudiohandler.o moc_tciserver.o moc_udphandler.o moc_udpcivdata.o moc_udpaudio.o moc_pahandler.o moc_rthandler.o moc_audiohandler.o moc_audioconverter.o moc_calibrationwindow.o moc_satellitesetup.o moc_udpserver.o moc_meter.o moc_qledlabel.o moc_pttyhandler.o moc_repeatersetup.o moc_rigctld.o moc_usbcontroller.o moc_controllersetup.o moc_selectradio.o moc_tcpserver.o moc_cluster.o moc_aboutbox.o moc_audiodevices.o   -lpulse -lpulse-simple -lrtaudio -ludev -lportaudio -L./ -lhidapi-libusb -lopus /usr/lib64/libQt6Multimedia.so /usr/lib64/libQt6PrintSupport.so /usr/lib64/libQt6Widgets.so /usr/lib64/libQt6Gui.so /usr/lib64/libGLX.so /usr/lib64/libOpenGL.so /usr/lib64/libQt6SerialPort.so /usr/lib64/libQt6WebSockets.so /usr/lib64/libQt6Network.so /usr/lib64/libQt6Xml.so /usr/lib64/libQt6Core.so /usr/lib64/libqcustomplot-qt6.so -lpthread -lGL

sudo make install
~~~



---

### Fedora install ###
---
Tested under Fedora 41 Workstation.

Install dependencies:

```bash
dnf install -y qt6-qtbase-devel qt6-qtmultimedia-devel qt6-serialport-devel \
  qt6-qtwebsocket-devel qcustomplot-qt6-devel opus-devel eigen3-devel \
  portaudio-devel rtaudio-devel hidapi-devel systemd-devel
```

When done, create a build area, clone the repo, build and install:

```bash
mkdir -p $HOME/src && cd $HOME/src
git clone https://gitlab.com/eliggett/wfview.git
cd wfview
mkdir -p build && cd build
qmake ../wfview.pro
sed -i 's/^LIBS.*/& -lqcustomplot-qt6/' Makefile
make -j4
sudo make install
```

wfview is now installed in /usr/local/bin and you can start it from launcher or from command line with command `wfview`.

# How to configure your RC-28 knob under Linux

To use RC-28 knob you need to add udev rules, please execute as root:

~~~
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="0c26", ATTRS{idProduct}=="001e", MODE="0666"' >> /etc/udev/rules.d/99-ham-wfview.rules
udevadm control --reload-rules && udevadm trigger
~~~
---
