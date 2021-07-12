# Beverage Monitoring

## Submodules

We use two submodules, to load them if the repo has already been cloned:

```shell
git submodule update --init
```


## Current Structure

```
.
├── balena-containers           <- Balena Project
│   ├── backend
│   ├── docker-compose.yml        <- Main docker-compose.yml
│   ├── mosquitto
│   ├── mysql
│   ├── mysql-data
│   ├── raspberrypirfid
│   └── update-server
├── beverage-backend-spring     <- Backend Submodule
├── blog                        <- Dir containing posts that are on Github
│   └── iot-blog-posts-mulc
├── esp-code                    <- Code for the esp8622 microcontroller
│   ├── barhardware
│   └── bierdeckel
├── README.md
└── tools                       <- Dir for potential tools
    └── keys
```

## TODO:
Download Balena Image
1. Register in Balena.io
2. Download Image for Raspberry PI 4B and install it to SD Card using Balena Etcher

Flash ESP Code
1. Download ESP8266_Code.cpp and 
2. Set Wifi SSID and Password
3. Flash it onto ESP8266 using Arduino IDE
4. Check with predefined weight if values are realistic

Install Containers to Raspberry PI using Balena
 --todo--


