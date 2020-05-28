<div id="configdiv">
  <h3>Configuration</h3>
  <div  class="config" id="config-div">
    <p><strong style="animation: blink .75s linear 4;">...try to load configuration data from MCU...</strong></p>
    <!-- configuration widgets. javascript output goes here -->
  </div>

  <table id="gmu-table">
    <tr>
      <td><label class="config-label">Groups</label></td>
      <td><label>1</label></td>
      <td><label>2</label></td>
      <td><label>3</label></td>
      <td><label>4</label></td>
      <td><label>5</label></td>
      <td><label>6</label></td>
      <td><label>7</label></td>
    </tr>
    <tr>
      <td><label class="config-label">Members</label></td>
      <td><input id="gmu1" style="width:2em;" type="number" min="0" max="7" value="7"></td>
      <td><input id="gmu2" style="width:2em;" type="number" min="0" max="7" value="7"></td>
      <td><input id="gmu3" style="width:2em;" type="number" min="0" max="7" value="7"></td>
      <td><input id="gmu4" style="width:2em;" type="number" min="0" max="7" value="7"></td>
      <td><input id="gmu5" style="width:2em;" type="number" min="0" max="7" value="7"></td>
      <td><input id="gmu6" style="width:2em;" type="number" min="0" max="7" value="7"></td>
      <td><input id="gmu7" style="width:2em;" type="number" min="0" max="7" value="7"></td>
    </tr>
  </table>

  <br>
  <button id="crlb" type="button">Reload</button>
  <button id="csvb" type="button">Save</button>
  <br><br>
  <button id="mrtb" type="button">Restart MCU</button><br>
  <div id="config_restart_div"></div>

  <section>
    <h1>Configuration-Wizards</h1>
    <ul>
      <li><button id="id_cuasb" type="button">Scan Central-Unit-ID</button> <span id="id_cuasStatus"></span></li>
    </ul>
  </section>
</div>

<script context="module">
  'use strict';

  import * as appDebug from './app_debug.js';
  import * as appState from './app_state.js';
  import * as httpFetch from './fetch.js';


  export function mcuConfig_cbHtml() {
  document.getElementById("csvb").onclick = () => mcuConfig_fromHtml_toMcu();
  }


  export function mcuConfigTable_genHtml(cfg) {
  var html = '<table id="cfg_table_id" class="conf-table">';
    Object.keys(cfg).forEach(function(key, idx) {
    html += '<tr id="cfg_' + key + '_tr">' + configTr_genHtml(key, cfg[key]) + '</tr>' + "\n";
    });
    html += '</table>';
  return html;
  }

  export function mcuConfig_updHtml(cfg) {
  Object.keys(cfg).forEach(function(key, idx) {
    let el = document.getElementById('cfg_' + key);

    switch (el.type) {
      case 'checkbox':
        el.checked = cfg[key] != 0;
        break;
      default:
        el.value = cfg[key];
    }
  });
  }



export function mcuConfig_fromHtml_toMcu() {
  const cfg = appState.ast.tfmcu_config;

  var new_cfg = {};
  var has_changed = false;
  usedMembers_fromHtml_toHtml();
  Object.keys(cfg).forEach(function(key, idx) {
    let new_val = 0;
    let el = document.getElementById('cfg_' + key);
    appDebug.dbLog("key: " + key);

    switch (el.type) {
      case 'checkbox':
        new_val = el.checked ? 1 : 0;
        break;
      default:
        new_val = el.value;
    }
    let old_val = cfg[key];
    if (new_val != old_val) {
      new_cfg[key] = new_val;
      has_changed = true;
      appDebug.dbLog(key);
    }
  });

  if (has_changed) {
    new_cfg.all = "?";
    var url = '/cmd.json';
    httpFetch.http_postRequest(url, { config: new_cfg });
  }
}


function configTr_genHtml(name, value) {
  if (name.endsWith("-enable")) {
    return '<td><label class="config-label">' + name +
      '</label></td><td><input class="config-input cb" type="checkbox" id="cfg_' + name +
      '" name="' + name + '"' + (value ? " checked" : "") + '></td>';
  } else if (name === 'rf-rx-pin' || name === 'set-button-pin') {
    return '<td><label class="config-label">' + name +
      '</label></td><td><input class="config-input" type="number" min="-1" max="39" id="cfg_' + name +
      '" name="' + name + '" value="' + value + '"></td>';
  } else if (name === 'rf-tx-pin') {
    return '<td><label class="config-label">' + name +
      '</label></td><td><input class="config-input" type="number" min="-1" max="33" id="cfg_' + name +
      '" name="' + name + '" value="' + value + '"></td>';
  } else if (name === 'verbose') {
    return '<td><label class="config-label">' + name +
      '</label></td><td><input class="config-input" type="number" min="0" max="5" id="cfg_' + name +
      '" name="' + name + '" value="' + value + '"></td>';
  } else if (name === 'network') {
    return '<td><label class="config-label">' + name +
      '</label></td><td><select  class="config-input" id="cfg_' + name + '">' +

      '<option value="wlan">Connect to existing WLAN</option>' +
      '<option value="ap">AP (192.168.4.1, ssid/pw=tronferno)</option>' +
      '<option value="lan">Connect to Ethernet</option>' + // dev-no-lan-delete-line
      '<option value="none">No Network</option>' +
      '</select></td>';
  } else if (name === 'lan-phy') {
    return '<td><label class="config-label">' + name +
      '</label></td><td><select  class="config-input" id="cfg_' + name + '">' +
      '<option value="lan8270">LAN8270</option>' +
      '<option value="rtl8201">RTL8201</option>' +
      '<option value="ip101">IP101</option>' +
      '</select></td>';
  } else if (name === 'lan-pwr-gpio') {
    return '<td><label class="config-label">' + name +
      '</label></td><td><input class="config-input" type="number" min="-1" max="36" id="cfg_' + name +
      '" name="' + name + '" value="' + value + '"></td>';
  } else if (name === 'astro-correction') {
    return '<td><label class="config-label">' + name +
      '</label></td><td><select  class="config-input" id="cfg_' + name + '">' +
      '<option value="0">average</option>' +
      '<option value="1">not too late or dark</option>' +
      '<option value="2">not too early or bright</option>' +
      '</select></td>';
  } else if (name.startsWith('gpio')) {
    return '<td><label class="config-label">' + name +
      '</label></td><td><select  class="config-input" id="cfg_' + name + '">' +
      '<option value="i">Input (Pull.FLoating)</option>' +
      '<option value="ih">Input (Pull.Up)</option>' +
      '<option value="il">Input (Pull.Down)</option>' +
      '<option value="O">Output</option>' +
      '<option value="Ol">Output (Level.Low)</option>' +
      '<option value="Oh">Output (Level.High)</option>' +
      '<option value="o">Output (OpenDrain)</option>' +
      '<option value="ol">Output (OpenDrain + Level.Low)</option>' +
      '<option value="oh">Output (OpenDrain + Level.High)</option>' +
      '<option value="Q">Input/Output</option>' +
      '<option value="Ql">Input/Output (Level.Low)</option>' +
      '<option value="Qh">Input/Output (Level.High)</option>' +
      '<option value="q">Input/Output (OpenDrain)</option>' +
      '<option value="ql">Input/Output (OpenDrain + Level.Low)</option>' +
      '<option value="qh">Input/Output (OpenDrain + Level.High)</option>' +
      '</select></td>';
  } else {
    return '<td><label class="config-label">' + name +
      '</label></td><td><input class="config-input text" type="text" id="cfg_' + name +
      '" name="' + name + '" value="'  + value + '"></td>';
  }
}


export function usedMembers_fromConfig() {

  let s = appState.ast.tfmcu_config["gm-used"];


  let sa = s ? s.split('').reverse() : [];

  let gmu = [0, 1, 2, 3, 4, 5, 6, 7];

  for (let g = 1; g < 8; ++g) {
    let um = sa[g] ? parseInt(sa[g]) : 0;
    gmu[g] = um;
  }

  appState.ast.gmu = gmu;
}

export function usedMembers_updHtml_fromHtml() {
  let val = document.getElementById("cfg_gm-used").value; // HEX string! not a number!

  while (val.length < 8)
    val = "0" + val;

  let g = 1;
  for (let i = 6; i >= 0; --i, ++g) {
    let id = "gmu" + (g.toString());
    document.getElementById(id).value = val[i];
  }
}

function usedMembers_fromHtml_toHtml() {
  let val = "";

  let written = false; // to skip leading zeros
  for (let i = 7; i >= 1; --i) {
    let id = "gmu" + (i.toString());
    let mct = document.getElementById(id).value;
    if (mct != '0' || written) {
      val += mct;
      written = true;
    }
  }
  let x = document.getElementById("cfg_gm-used").value;
  val += x[x.length - 1]; // push back unused lowest nibble

  document.getElementById("cfg_gm-used").value = val;
}
</script>
