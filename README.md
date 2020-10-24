# Goal of the challenge [#moveandchill](https://makezurich.ch/start/2/) of [makezurich.ch](https://makezurich.ch) 2020
Collect data on the usage of public chairs provided by the city of Zurich to optimize the amount and placements of the offer (eg time, duration is used, if in th sun or shadow, ...)

# Resources and ideas
- wifi scanner was used for location detection on this [bachelor thesis about location of working tools](https://www.fhnw.ch/plattformen/bachelor20/IMVS29/index.html)
- [google api to get location from wifi ssids](https://developers.google.com/maps/documentation/geolocation/overview#wifi_access_point_object}
- relative positioning by ble directional localisation with the help of a [ble gateway](https://thingoo.en.alibaba.com/product/62091463696-233065917/ble_5_1_asset_tracking_bluetooth_gateway_for_AOA_AOE_directional_location.html)
- esp32 with wifi module to scan wifi
- focus on usage of chair first/as separate task?
- flex bent sensor to detect if chair used
- IMU sensor to detect movement which shows chair in use
