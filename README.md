### Goal of the challenge [#moveandchill](https://makezurich.ch/start/2/) of [makezurich.ch](https://makezurich.ch) 2020
Proof of concept based on the [Arduino Nano BLE 33](https://github.com/yene/Nano-33-BLE-Sense) to measure how public chairs are used in the city of Zurich during the [#makezurich](https://makezurich.ch/) hackathon.

It is an implementation of the [#moveandchill](https://makezurich.ch/start/2/) challenge.

See the [RESULTS](RESULTS.md) for a summary of the achievements.

### Resources
- [lorawan library](https://github.com/2ni/lorawan_modem) to communicate with the lorawan module Murata ST B-L072Z-LRWAN1 or the Miromico FMLR-72-U-STL0Z
- [tech specs](https://makezurich.ch/techspecs/) and [description](https://github.com/make-zurich/makezurich-hardware-intro#arduino-nano-33-ble-sense)
- [schematic](https://easyeda.com/vkbs/makezurich-moveandchill) [pdf](schematic/Schematic_makezurich-moveandchill_2020-10-31_12-12-48.pdf)
- [output examples](examples/)
- [data output on thingspeak](https://thingspeak.com/channels/1213808)

### Setup
```
git submodule update --init --recursive
```

### Resources and ideas
- wifi scanner was used for location detection on this [bachelor thesis about location of working tools](https://www.fhnw.ch/plattformen/bachelor20/IMVS29/index.html)
- [google api to get location from wifi ssids](https://developers.google.com/maps/documentation/geolocation/overview#wifi_access_point_object)
- relative positioning by ble directional localisation with the help of a [ble gateway](https://thingoo.en.alibaba.com/product/62091463696-233065917/ble_5_1_asset_tracking_bluetooth_gateway_for_AOA_AOE_directional_location.html)
- esp32 with wifi module to scan wifi
- focus on usage of chair first/as separate task?
- flex bent sensor to detect if chair used
- IMU sensor to detect movement which shows chair in use

### Things to do
- [X] 1st connection of arduino nano 33
- [X] build arduino library to commuincate with murata module
- [X] occupancy detection of a chair
  - [X] infrared
  - [X] flexbend (quadratic)
  - [X] strain gauge / load cell with HX711
- [X] function to check if chair in the sun or not
- [X] measure duration of occupancy
- [X] send data to ttn
- [X] pass data to some db which is publicly available
- [ ] detect position of chair

### Manual fix about attachInterrupt only calling one
- see this [patch](https://github.com/arduino/ArduinoCore-nRF528x-mbedos/pull/69/commits/af5d3d43ae435e392a985e1b7b18491b74b33a5b)
- fixed in this file: ~/.platformio/packages/framework-arduino-nrf52-mbedos/cores/arduino/Interrupts.cpp

### Inital setup calls I did to set things up ([platformio](https://docs.platformio.org/en/latest/core/quickstart.html))
```
pio project init --board nano33ble
pio lib install 6590 # hts221 (temperature, humidity) 
pio lib install 6591 # lps22hb (barometric pressure)
pio lib install 6589 # lsm9ds3 (9-axis imu)
pio lib install 6616 # light APDS9960
pyenv virtualenv 3.7.8 chair
pyenv local chair
cd lib; git submodule  add git@github.com:2ni/lorawan_modem.git
```
