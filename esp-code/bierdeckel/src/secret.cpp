#include "secret.h"
#include <Arduino.h>

// Define CA cert
const char cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDYDCCAkgCFAby5WtzERmyqC5tDuNvHyzZnwV1MA0GCSqGSIb3DQEBCwUAMHwx
CzAJBgNVBAYTAkRFMQswCQYDVQQIDAJCVzELMAkGA1UEBwwCS0ExDDAKBgNVBAoM
A2lvdDEMMAoGA1UECwwDaW90MRkwFwYDVQQDDBBteWRvbWFpbm5hbWUueHl6MRww
GgYJKoZIhvcNAQkBFg15b3VyQG1haWwuY29tMB4XDTIxMDYwNzE2NDYyNVoXDTIy
MDYwMjE2NDYyNVowXTELMAkGA1UEBhMCREUxCzAJBgNVBAgMAkJXMQswCQYDVQQH
DAJLQTEMMAoGA1UECgwDaW90MQwwCgYDVQQLDANpb3QxGDAWBgNVBAMMDzE5Mi4x
NjguMTc4LjEyMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMV2CyW0
lxuv+CZFbKUp2Qzs0rjafUminGbeFw54+IRHhJJSm4ddLmObm29dUYErtUOLDK+i
j7reWbA4xKyyP/3mYyRqVQnmLzqe7rCPe1ah3R/JhF1O95P9jKDlZfxAYw6jhAEG
0EYCEhoOMfnNRjcXE/rgJMAYzyXxqudISFKBGA8ci0nyFOSlQd5WL9zUBGt1L967
zLhTdOOGsVuMZ3sV2fvYRflMFjwMVoO+B81h66THJXo28FQ3eQju5dL3c+uxu2Cm
l33/wAJI6Sus8iM1N2270FDhiRkkdiNiQnxGMVuTp+I7hhF8lAtPGKxZwR4DU/zR
Gw8AYejc+XV4OmMCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAC1TT5P6sLg2CmGlZ
8IG3AIBg/8Tnhon7nONiF/0pi/8uod093WHKGdHUW4KD+dyMk5vo32qwx7cT2oSz
LvtDibhF01Nivk633g2WHk31KKcvRZ9t72pxIdU/ZibnYNR4034m/3F050t6peI9
MfhIF3+QVkhjkyoLdX2gz1caeVo2u9aR0t7gnMbJByNYbvjXJu0qE94QfHnSSxMd
q6jVoZBd5eqYJKqZ2I+230CNbqJKsy0SEa0XnypkHK0MFOePjow70izN/345UreI
47CM224yaMJDMmjsbJRaPoOTeFSnkTl2DC2PDII60QSLk+1zRWwLhSpgqKz5ERw1
VhxzNA==
-----END CERTIFICATE-----
)EOF";

// Define private key for esp client, in order to authenticate ourself
const char private_key[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAxXYLJbSXG6/4JkVspSnZDOzSuNp9SaKcZt4XDnj4hEeEklKb
h10uY5ubb11RgSu1Q4sMr6KPut5ZsDjErLI//eZjJGpVCeYvOp7usI97VqHdH8mE
XU73k/2MoOVl/EBjDqOEAQbQRgISGg4x+c1GNxcT+uAkwBjPJfGq50hIUoEYDxyL
SfIU5KVB3lYv3NQEa3Uv3rvMuFN044axW4xnexXZ+9hF+UwWPAxWg74HzWHrpMcl
ejbwVDd5CO7l0vdz67G7YKaXff/AAkjpK6zyIzU3bbvQUOGJGSR2I2JCfEYxW5On
4juGEXyUC08YrFnBHgNT/NEbDwBh6Nz5dXg6YwIDAQABAoIBAQCc3go9ChqBhGo+
/SgyjexAuGMvN2QQ+34EuqrWdIP5kldfZwDnqL8Roqz468m0NGTsI6sQXLSSX9Ig
jPixuWnc6woYA+FK2/LpPxmgalwxlqV0e0MMXY+Rofv2VkeO/hezqXNU3XTYKQz4
Zg6NxOXjHhJhW2/Wq97k5fg/hMzvvPlT/sCvPLa2yDLzK5a14BO9TJRIoFwnFObH
I58OaoINqiP1ZUag0wHys8QiWQ9DV1aQfQZ4nD1U55nxj3ebZ4JEJwFPvbQfpmrc
OULfLgWXvBITBgHVucwOeusqFC0t6sNZeconyVAP4AKyqLRyphlO3TaPJIL4yqFu
AcyZgWEBAoGBAOhR3JC45dweWtfOnG10pvJPO7E/XDStcnXPQ1WpiKAxvVdkUsy7
0l8z8tnFixu3bexn11m8HnZwPTTf/eGVltHVOwhsRa2m8VLcd8uRGn0BlW0j9caT
wBh8H3SAZKvN8Nim9RYZFg9y1xV/WrGk7ZXQ02p/TGAeSSuvCHREoaZFAoGBANmW
lQAphxQsuj0VZbBOGiRgCePZLL9/3Azx6StNRF2/IxVlmQD/q5RjQLYQqVD2IxK1
1qpHcb126dRSRuG6UEffMKdbSXM3EENH8L8TpQAyIx/3hWySgvwjM0wJRJgWOHeu
fz1w8AGWsz/U6Bg42wnXYevKK205qCKYEFZ7ghyHAoGASSfF+biPgTH5zy0bawgp
rfGvTVAzW88mVNywSmA5SqB/C+7md9vJEfuMxHCFLnQjZCcK1BH90bPkQisqigeN
14N6tFtL0bPZGAuemXaWzbha8mQ5d499FPi4+vmXOdZ+uepREOVTYgf6nKVezMOv
oNaCTG3LghTnW58hXWNjN7UCgYAm6HVWZRahdeoLmDLp1t132bCLDL+isrNfoTZn
ptZtyQr1/YfhlNZSn3jn1YzhTFIzO07afhIJpiTj8Z55KL7IS1HA62Lz9kmzLj8P
e+zKXyzGv5UdOAmyGn1GwHWCmJ6aUBqymupf7lm5NVIXWrtYRCpfZnRjgKbfIL/z
Jvy6KwKBgQCB7qtuxIxf6hjINBnJnZ8J4HBr4eteuCUZcnyMpD+cRY1UlkEiiWM0
Ma5jbaqddUFkABTraAf8SMxZ3TLFhte29wp8yCVibtv/5rbN8VPCevRy/5BOX5gD
/X36xOFF78qAL42jtYpK90KOKAyfoJ5H7HIRFcoMwTeWyluSaDjIlw==
-----END RSA PRIVATE KEY-----
)EOF";
