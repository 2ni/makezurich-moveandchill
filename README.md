# Goal of the challenge [#moveandchill](https://makezurich.ch/start/2/) of [makezurich.ch](https://makezurich.ch) 2020
Collect data on the usage of public chairs provided by the city of Zurich to optimize the amount and placements of the offer (eg time, duration is used, if in th sun or shadow, ...)

# Resources and ideas
- wifi scanner was used for location detection on this [bachelor thesis about location of working tools](https://www.fhnw.ch/plattformen/bachelor20/IMVS29/index.html)
- [google api to get location from wifi ssids](https://developers.google.com/maps/documentation/geolocation/overview#wifi_access_point_object)
- relative positioning by ble directional localisation with the help of a [ble gateway](https://thingoo.en.alibaba.com/product/62091463696-233065917/ble_5_1_asset_tracking_bluetooth_gateway_for_AOA_AOE_directional_location.html)
- esp32 with wifi module to scan wifi
- focus on usage of chair first/as separate task?
- flex bent sensor to detect if chair used
- IMU sensor to detect movement which shows chair in use


# Inital setup ([platformio](https://docs.platformio.org/en/latest/core/quickstart.html))
```
pio project init --board nano33ble
pio lib install 6590 # hts221 (temperature, humidity) 
pio lib install 6591 # lps22hb (barometric pressure)
pio lib install 6589 # lsm9ds3 (9-axis imu)
pyenv virtualenv 3.7.8 chair
pyenv local chair
```

# Things to do
- [X] 1st connection of arduino nano 33
- [ ] occupancy detection of a chair
  - [ ] infrared
  - [ ] flexbend
  - [ ] strain gauge / load cell with HX711
- [ ] function to check if chair in the sun or not
- [ ] measure duration of occupancy
- [ ] send data to ttn
- [ ] pass data to some db which is publicly available
