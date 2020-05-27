'use strict';
import * as as from './app_state.js';
import * as a_http from './fetch.js';
import * as misc from './misc.js';
import * as ota from './netota.js';

export function testing_netota() {
    document.getElementById("netota").onclick = () => ota.netFirmwareOTA(document.getElementById("id-esp32FirmwareURL").value);
}

export function testing_genHtml() {
    misc.tabs.push({ 'text': 'Tests', 'div_id': ['testsdiv'], });
    test_random_periodic_movement_genHtml();
}

function test_random_periodic_movement_genHtml() {
    let html = '<br><br>Random Periodic Movement Commands: <button id="test_rpm_start" type="button">Test Start</button> <button id="test_rpm_stop" type="button">Test Stop</button>';

    document.getElementById("testsdiv").innerHTML += html;
    document.getElementById("test_rpm_start").onclick = () => testPressed(true);
    document.getElementById("test_rpm_stop").onclick = () => testPressed(false);
}

const test_cmds = ['up', 'down', 'stop'];
function get_randomCmd() {
    const idx = Math.floor(Math.random() * (test_cmds.length - 0.01));
    return test_cmds[idx];
}

function test_randomCmd() {
    const cmd = get_randomCmd();
    console.log(cmd);
    a_http.http_postShutterCommand(cmd);
}

let test_randomCmd_interval = 0;
function testPressed(enable) {

    if (test_randomCmd_interval) {
        clearInterval(test_randomCmd_interval);
        test_randomCmd_interval = 0;
    }

    if (enable) {
        as.ast.g = 7;
        as.ast.m = 1;
        test_randomCmd_interval = setInterval(test_randomCmd, 1000);
    }
}



