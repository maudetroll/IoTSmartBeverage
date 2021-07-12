const fs = require('fs');
const https = require('https');
const express = require('express');
const path = require('path');

/*
 * Generic authentication method
 */ 
const clientAuthMiddleware = () => (req, res, next) => {
  if (!req.client.authorized) {
    return res.status(401).send('Invalid client certificate authentication.');
  }
  return next();
};
  
const app = express();
// Ever Request has to be authenticated before further processing
app.use(clientAuthMiddleware());

var PORT = 8081;

/*
 * HTTP Endpoint for the firmware
 */
app.get('/firmware.bin.signed', function(_req, res, next){

    var options = {
        root: path.join(__dirname)
    };
      
    var fileName = 'firmware.bin.signed';

    res.sendFile(fileName, options, function (err) {
        if (err) {
            next(err);
        } else {
            console.log('Sent:', fileName);
        }
    });
});

/*
 * HTTP Endpoint for the bar firmware
 */
app.get('/bar-firmware.bin.signed', function(_req, res, next){

    var options = {
        root: path.join(__dirname)
    };
      
    var fileName = 'bar-firmware.bin.signed';

    res.sendFile(fileName, options, function (err) {
        if (err) {
            next(err);
        } else {
            console.log('Sent:', fileName);
        }
    });
});
  
https
  .createServer(
    {
      // ...
      cert: fs.readFileSync('./secrets/server.crt'),
      key: fs.readFileSync('./secrets/server.key'),
      ca: fs.readFileSync('./secrets/ca.crt'),
      requestCert: true,
      rejectUnauthorized: false,
      // ...
    },
    app
  )
  .listen(PORT);

