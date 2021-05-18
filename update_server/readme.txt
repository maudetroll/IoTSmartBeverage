Update Server
===

Update Server for the ESPs. Should support some form of HTTPS or similar
communication. Actix supports HTTP/2.0, which is hopefully sufficient.

Certificates
---

Self signed cert for `localhost` is contained in simlinked directory
`./self-signed-cert`. Secret which authorizes the requesting client is now
within a separate module not contained in the repo. Path to the binary which
should be send to requesting ESPs is hardcoded 
(at the moment: `./srv/esp-builds/latest.ino.bin.signed`)

Send test request
---
Use `./test-request.sh` to test the running update server. Server should listen
on `localhost:8080`, the self signed certification is needed and the base64
encoded version and secret should be passed to the script via an environment
variable.

TODO:
- private part isn't contained in the repo yet, so this code won't run
