# Beverage Monitoring

## Submodules

We use two submodules, to load them if the repo has already been cloned:

```shell
git submodule update --init
```


## Current Structure

```
 .
├──  esp
│  ├──  data
│  ├──  esp.ino
│  ├──  keys
│  ├──  Makefile
│  ├──  private.key -> keys/private.key // MISSING
│  ├──  public.key -> keys/public.key
│  ├──  readme.txt 
│  ├──  secret.h // MISSING
│  ├──  self-signed-cert
│  └──  tools
├──  README.md
└──  update_server
   ├──  Cargo.lock
   ├──  Cargo.toml
   ├──  readme.txt
   ├──  self-signed-cert -> ../esp/self-signed-cert // MISSES PRIVATE KEY
   ├──  src
   └──  test-request.sh
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


