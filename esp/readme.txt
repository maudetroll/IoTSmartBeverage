Boilerplate code for the ESP8266
===

Dependencies:
---

- Arduino IDE
- Arduino-CLI
- Arduino ESP8266 Core
- PubSubClient 2.8.0
    * make install-libraries // Requires Arduino IDE

Certificates and Keys
---

Only public keys should be contained in the repo at the moment.  with
`tools/certs-from-mozilla.py` we can fetch a list of certs from mozilla, using
`make refresh all certs` should fetch those and add the pub part of a cert for
the current `localhost`.

Compile
---

The `arduino-cli` should automatically produce a signed artifact, if private and
public key are contained within the directory.  But as the private part of the
signature key is not yet within this repo, the build will not be signed. 

If all dependencies and the arduino-cli are installed `make compile` should be
successful, the artifacts should be in the directory `./build`. When building
without the `--output-dir` flag a temp directory is created, which strangely
contains more artifacts (?tf).


Generating the file system image
---

We probably need the credentials file in the local file system, therefore we
need to create and upload a file system image. A possible tool can be found at:
https://raw.githubusercontent.com/earlephilhower/arduino-esp8266littlefs-plugin
This needs a few more tools like `mklittlefs`:
https://raw.githubusercontent.com/earlephilhower/mklittlefs
Those tools should create a file image from all the files found in the
directory `data` and automatically upload it to the ESP.:

TODO:
- To automate this process we could use the Makefile and just call the tools
  which would be called by the Arduino IDE Plugin ourselves with the right
  parameters, but to get the parameters we should run the plugin once and just
  copy the logged stuff.
- We could try to push file directory updates over the air similar to normal
  updates, there may be a suitable function. (See comment in ./esp.ino)

