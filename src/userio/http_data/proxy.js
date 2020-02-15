#!/usr/bin/env node

'use strict';

// proxy server for testing html and js files

// address of the MCU
let mcu = 'http://192.168.1.69:80';


let express = require('express');
let httpProxy = require('http-proxy');

let server = express();

// serving static files from current directory
server.get("/index.html", (req, res) => {
    res.sendFile(__dirname + '/tfmcu_dev.html');
});
server.get("/tfmcu_dev.html", (req, res) => {
    res.sendFile(__dirname + '/tfmcu_dev.html');
});
server.get("/tfmcu_dev.js", (req, res) => {
    res.sendFile(__dirname + '/tfmcu_dev.js');
});

// forward any requests to MCU
let proxy = httpProxy.createProxyServer();

server.all("/*.json", (req, res) => {
    proxy.web(req, res, { target: mcu });
});


server.listen(3000);
