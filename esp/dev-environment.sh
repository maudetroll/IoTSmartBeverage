#!/bin/bash
#!/bin/bash
CONTAINER_TOOL="podman"
if [ ! -d ESP8266_RTOS_SDK ]; then
    git clone https://github.com/espressif/ESP8266_RTOS_SDK/
fi
git submodule update --init --recursive
${CONTAINER_TOOL} build -t esp_dev .
${CONTAINER_TOOL} run -v ./ESP8266_RTOS_SDK:/work/ESP8266_RTOS_SDK -i -t esp_dev bash

