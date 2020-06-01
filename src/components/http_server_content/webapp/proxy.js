#!/usr/bin/env node

'use strict';

const path = require('path');
const express = require('express');
const httpProxy = require('http-proxy');
var expressWs = require('express-ws')


let proj_dir=path.dirname(path.dirname(path.dirname(path.dirname(__dirname))));
let tff_dir=path.dirname(proj_dir)+"/tronferno-fhem";
let build_dir=proj_dir+"/src/esp32/build";
let cont_dir=__dirname
let mcu = 'http://192.168.1.69:80';
let mcu_ws = 'ws://192.168.1.69:80';

let app = express();
 expressWs(app);
let server = require('http').createServer(app);



// forward any requests to MCU
let proxy = httpProxy.createProxyServer({ ws: true });

app.all("/*.json", (req, res) => {
    proxy.web(req, res, { target: mcu });
});
app.all("/f/cli/*", (req, res) => {
    proxy.web(req, res, { target: mcu });
});

app.ws("/ws", (req, res) => {
    proxy.web(req, res, { target: mcu_ws });
});
server.on('upgrade', function (req, socket, head) { 
    console.log('head', JSON.stringify(head));
     proxy.ws(req, socket, head, { target: mcu_ws });
     });

// serve some files



// static files of MCU HTTP server
app.get("/index.html", (req, res) => {
    res.sendFile(cont_dir + '/tfmcu_dev.html');
});
app.get("/tfmcu_dev.html", (req, res) => {
    res.sendFile(cont_dir + '/tfmcu_dev.html');
});
app.get("/tfmcu.html", (req, res) => {
    res.sendFile(cont_dir + '/build/tfmcu.html');
});
app.get("/f/js/tfmcu.js", (req, res) => {
    //res.sendFile(cont_dir + '/tfmcu_dev.js');
    res.sendFile(cont_dir + '/tfmcu/build/bundle.js');
});
app.get("/f/js/bundle.js.map", (req, res) => {
    //res.sendFile(cont_dir + '/tfmcu_dev.js');
    res.sendFile(cont_dir + '/tfmcu/build/bundle.js.map');
});
app.get("/f/css/global.css", (req, res) => {
    //res.sendFile(cont_dir + '/tfmcu_dev.js');
    res.sendFile(cont_dir + '/tfmcu/src/app.css');
});
app.get("/f/css/tfmcu.css", (req, res) => {
    //res.sendFile(cont_dir + '/tfmcu_dev.js');
    res.sendFile(cont_dir + '/tfmcu/build/bundle.css');
});
app.get("/src/", (req, res) => {
    //res.sendFile(cont_dir + '/tfmcu_dev.js');
    res.sendFile(cont_dir + '/tfmcu/src/');
});

// ESP32 firware image for OTA update
app.get("/tronferno-mcu.bin", (req, res) => {
    res.sendFile(build_dir+'/tronferno-mcu.bin');
});


// files for updating FHEM modules
app.get("/tronferno-fhem/controls_tronferno.txt", (req, res) => {
    res.sendFile(tff_dir + '/modules/tronferno/controls_tronferno.txt');
});
app.get("/tronferno-fhem/CHANGED", (req, res) => {
    res.sendFile(tff_dir + '/modules/tronferno/CHANGED');
});
app.get("/tronferno-fhem/FHEM/00_TronfernoMCU.pm", (req, res) => {
    res.sendFile(tff_dir + '/modules/tronferno/FHEM/00_TronfernoMCU.pm');
});
app.get("/tronferno-fhem/FHEM/10_Tronferno.pm", (req, res) => {
    res.sendFile(tff_dir + '/modules/tronferno/FHEM/10_Tronferno.pm');
});
app.get("/fernotron-fhem/controls_fernotron.txt", (req, res) => {
    res.sendFile(tff_dir + '/modules/sduino/controls_fernotron.txt');
});
app.get("/fernotron-fhem/CHANGED", (req, res) => {
    res.sendFile(tff_dir + '/modules/sduino/CHANGED');
});
app.get("/fernotron-fhem/FHEM/10_Fernotron.pm", (req, res) => {
    res.sendFile(tff_dir + '/modules/sduino/FHEM/10_Fernotron.pm');
});


server.listen(3000);


/*
Local Variables:
compile-command: "jshint proxy.js"
End:
*/
