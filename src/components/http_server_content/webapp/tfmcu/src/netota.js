'use strict';
import { dbLog } from './misc.js';
import * as a_http from './fetch.js';
import * as misc from './misc.js';


export function netota_cbHtml() {
  document.getElementById("netota_master").onclick = () => netFirmwareOTA(otaName_master);
  document.getElementById("netota_beta").onclick = () => netFirmwareOTA(otaName_beta);
}

// ----------------- firmware div ---------------
var netota_intervalID = 0;
var netota_isInProgress = false;
var netota_progressCounter = 0;

function netota_FetchFeedback() {
  var netmcu = { to: "tfmcu" };
  netmcu.mcu = {
    "ota": "?"
  };
  let url = '/cmd.json';
  dbLog("url: " + url);
  a_http.http_postRequest(url, netmcu);
}

export const otaName_master = 'github-master';
export const otaName_beta = 'github-beta';
export function netFirmwareOTA(ota_name) {
  if (netota_isInProgress)
    return;
  var netmcu = { to: "tfmcu" };
  netmcu.mcu = {
    ota: ota_name
  };
  let url = '/cmd.json';
  dbLog("url: " + url);
  a_http.http_postRequest(url, netmcu);
  document.getElementById("netota_progress_div").innerHTML = "<strong>Firmware is updating...<br></strong>" + '<progress id="netota_progress_bar" value="0" max="30">70 %</progress>';
  netota_intervalID = setInterval(netota_FetchFeedback, 1000);
  netota_isInProgress = true;
  document.getElementById("netota_controls").style.display = "none";
}

function gitTags_netota() {
  const git_tag = document.getElementById('gitTags_select').value;
  netFirmwareOTA("tag:" + git_tag);
}


function gitTags_genHtml(json) {
  let html = '<button id="gitTag_netota" type="button">Do flash selected firmware version: </button>' +
    '<select id="gitTags_select">';
  json.forEach(item => {
    const name = item.name;
    html += '<option value="' + name + '">' + name + '</option>';
  });

  html += '</select>';
  document.getElementById('gitTags_div').innerHTML = html;
  document.getElementById("gitTag_netota").onclick = () => gitTags_netota();
}

function gitTags_handleResponse(json) {
  console.log(json[0]);
  gitTags_genHtml(json);
}

export function gitTags_fetch() {
  const tag_url = "https://api.github.com/repos/zwiebert/tronferno-mcu-bin/tags";
  const fetch_data = {
    method: "GET",
    cache: "no-cache",
    headers: {
    },
    referrer: "no-referrer",
    //body: JSON.stringify(data),
  };
  return fetch(tag_url, fetch_data)
    .then(response => {
      if (!response.ok) {
        console.log("error");
        throw new Error("network repsonse failed");
      }
      return response.json();
    })

    .then((json) => gitTags_handleResponse(json))

    .catch((error) => {
      console.log("error: a_http.http_postRequest(): ", error);
    });

}


export function netota_handle_otaState(ota_state) {
  let e = document.getElementById("netota_progress_div");
  switch (ota_state) {
    case 0: // none

      break;
    case 1: // run
      document.getElementById("netota_progress_bar").value = (++netota_progressCounter).toString();
      break;
    case 2: // fail
      e.innerHTML += "<br><strong>Update failed<br><br></strong>";
      break;
    case 3: // done
      document.getElementById("netota_progress_bar").value = document.getElementById("netota_progress_bar").max;
      e.innerHTML += '<br><strong>Update succeeded <button id="ota_reboot" type="button">Reboot MCU</button><br><br></strong>';
      document.getElementById("ota_reboot").onclick = () => misc.req_mcuRestart();
      break;
  }
  if (netota_isInProgress && ota_state != 1) {
    clearInterval(netota_intervalID);
    netota_progressCounter = 0;
    netota_isInProgress = false;
    document.getElementById("netota_controls").style.display = "";
  }
}

