Update Server
===

Update Server for the ESPs. Should support some form of HTTPS or similar
communication. Actix supports HTTP/2.0, which is hopefully sufficient.

Certificates
---

Self signed cert for `localhost` is contained in simlinked directory
`./self-signed-cert`.
TODO:
- private part isn't contained in the repo yet, so this code won't run
