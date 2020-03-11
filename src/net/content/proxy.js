#!/usr/bin/env node

'use strict';

const path = require('path');
const express = require('express');
const httpProxy = require('http-proxy');

let proj_dir=path.dirname(path.dirname(path.dirname(__dirname)));
let tff_dir=path.dirname(proj_dir)+"/tronferno-fhem";
let build_dir=proj_dir+"/src/esp32/build";
let cont_dir=__dirname
let mcu = 'http://192.168.1.69:80';

let server = express();

// forward any requests to MCU
let proxy = httpProxy.createProxyServer();

server.all("/*.json", (req, res) => {
    proxy.web(req, res, { target: mcu });
});
server.all("/doc", (req, res) => {
    proxy.web(req, res, { target: mcu });
});


// serve some files



// static files of MCU HTTP server
server.get("/index.html", (req, res) => {
    res.sendFile(cont_dir + '/tfmcu_dev.html');
});
server.get("/tfmcu_dev.html", (req, res) => {
    res.sendFile(cont_dir + '/tfmcu_dev.html');
});
server.get("/tfmcu_dev.js", (req, res) => {
    res.sendFile(cont_dir + '/tfmcu_dev.js');
});
server.get("/tfmcu.html", (req, res) => {
  res.sendFile(cont_dir + '/tfmcu.html');
});
server.get("/tfmcu.js", (req, res) => {
  res.sendFile(cont_dir + '/tfmcu.js');
});


// ESP32 firware image for OTA update
server.get("/tronferno-mcu.bin", (req, res) => {
    res.sendFile(build_dir+'/tronferno-mcu.bin');
});


// files for updating FHEM modules
server.get("/tronferno-fhem/controls_tronferno.txt", (req, res) => {
    res.sendFile(tff_dir + '/modules/tronferno/controls_tronferno.txt');
});
server.get("/tronferno-fhem/CHANGED", (req, res) => {
    res.sendFile(tff_dir + '/modules/tronferno/CHANGED');
});
server.get("/tronferno-fhem/FHEM/00_TronfernoMCU.pm", (req, res) => {
    res.sendFile(tff_dir + '/modules/tronferno/FHEM/00_TronfernoMCU.pm');
});
server.get("/tronferno-fhem/FHEM/10_Tronferno.pm", (req, res) => {
    res.sendFile(tff_dir + '/modules/tronferno/FHEM/10_Tronferno.pm');
});
server.get("/fernotron-fhem/controls_fernotron.txt", (req, res) => {
    res.sendFile(tff_dir + '/modules/sduino/controls_fernotron.txt');
});
server.get("/fernotron-fhem/CHANGED", (req, res) => {
    res.sendFile(tff_dir + '/modules/sduino/CHANGED');
});
server.get("/fernotron-fhem/FHEM/10_Fernotron.pm", (req, res) => {
    res.sendFile(tff_dir + '/modules/sduino/FHEM/10_Fernotron.pm');
});


server.listen(3000);


/*
Local Variables:
compile-command: "jshint proxy.js"
End:
*/
