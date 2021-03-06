### Achievements
- implemented a library to communicate with the lorawan module
- implemented a poc sending duration of occupancy along with a summary of environmental data
- very basic "visualisation"
- met new people!

<img src="media/result.jpg" width="400" />

### How we built it
- started with simple stuff, such as detection of occupancy
- trying to get an end-to-end breakthrough as fast as possible
- iteratively extend / enhance what we've got

### Learnings
- great format, aka small junks during multiple days
- forming teams was challenging
- serial communication is a pain
- datasheets are key!
- 1st time hackathon, it's fun!
- arduino ide seems to suck big time. Luckily I used platformio :-)

### Next steps / missing items
- [ ] geolocalization
- [ ] low power mode / power mgmt
- [ ] waterproof box
- [ ] sync with representatives
- [ ] make data to opendata
- [ ] test it in real
- [ ] meaningful representation

### Sensor comparison for occupancy

| Sensor                   | Advantage                                                              | Disadvantage                              |
| ------                   | ---------                                                              | ------------                              |
| force sensing resistor   | cheap                                                                  | must be mounted on the chair (top side)   |
|                          | best for low power (interrupt based GPIO with high impedance resistor) |                                           |
| IR sensor                | can be mounted on the back of the chair                                | data must be pulled at regular intervalls |
|                          | sensitivity can be adapted                                             |                                           |
| proximity sensor Nano 33 | on board                                                               | low sensitivity (~5-10cm only)            |
|                          | no additional pin used                                                 | data mus be pulled at regular intervalls  |
