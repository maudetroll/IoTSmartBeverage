# Container which should be run via balena 

Docker compose contains multiple container needed

- Code that handles RFID sensors
- Update Server (Node.js)
- Mosquitto Server
- MySQL (MariaDB)
- Database fill service
- Backend

To push all services use the balena cli.

```shell
$ balena push <yourAppName>
# Or
$ balena push <local IP>
```
