# robocar-vehicle-controller
This repository contains the Arduino code for the Robocar vehicle controller

## How it works

This code runs on an Arduino and accepts actuation commands via serial connection.

## Commands

### Reset
```
R
```

### Throttle
```
T<value>
```

Where <value> is a decimal floating point representation of the throttle value (in percent) between -100 and 100.

### Steering
```
S<value>
```
Where <value> is a decimal floating point representaton of the steering angle (in degrees) between -90.0 and 90.0.
