var base = '';
var base = 'http://10.0.0.20'; //dev-delete-line//

var cfg_out = {};

function config_item2(name,value) {
    return '<label class="config-label">'+name+'<input class="config-input" type="text" id="cfg.'+name+'" name="'+name+'" value="'+value+'"></label>';
}


function config_item(name,value) {
    return '<td><label class="config-label">'+name+'</label></td><td><input class="config-input" type="text" id="cfg.'+name+'" name="'+name+'" value="'+value+'"></td>';
}


function json2cli(data) {
    result = "";
    Object.keys(data).forEach (function (key, idx) {
        val = data[key];
        result = result+" "+key+"="+val;
    });
    return result;
}

function jsonUpdateHtml(cfg) {
    Object.keys(cfg).forEach (function (key, idx) {
        document.getElementById('cfg.'+key).value = cfg[key];
    });
}

function postData(url = '', data = {}) {
  // Default options are marked with *
    return fetch(url, {
        method: "POST", // *GET, POST, PUT, DELETE, etc.
        //mode: "cors", // no-cors, cors, *same-origin
        cache: "no-cache", // *default, no-cache, reload, force-cache, only-if-cached
       // credentials: "same-origin", // include, *same-origin, omit
        headers: {
            "Content-Type": "application/json",
        },
        redirect: "follow", // manual, *follow, error
        referrer: "no-referrer", // no-referrer, *client
        body: JSON.stringify(data), // body data type must match "Content-Type" header
    })
    .then(response => response.json()); // parses JSON response into native Javascript objects 
}

function mcuRestart() {
    var json = { name:"tfmcu", config: { restart:"1" } };
    var url = base+'/cmd.json';
    postData(url, json);
}
function inputConfig2mcu() {
    var tfmcu = JSON.parse(sessionStorage.getItem("tfmcu.config"));
    var cfg = tfmcu.config;
    var new_cfg = {}
    var has_changed = false;
    Object.keys(cfg).forEach (function (key, idx) {
        var new_val = document.getElementById('cfg.'+key).value;
        var old_val = cfg[key];
        if (new_val != old_val) {
            new_cfg[key] = new_val;
            has_changed = true;
            console.log(key);
        }
    });

    if (has_changed) {
        tfmcu.config = new_cfg;
        console.log(JSON.stringify(tfmcu));
        var url = base+'/cmd.json';
        console.log("url: "+url);
        postData(url, tfmcu);
        fetch_json(true);
    }
}

function cInputSend2mcu(c) {
    var tfmcu = {name:"tfmcu"};
    var send = {
        g: document.getElementById('send-g').value,
        m: document.getElementById('send-m').value,
        c: c,
    };
    tfmcu['send'] = send;
    console.log(JSON.stringify(tfmcu));
    var url = base+'/cmd.json';
    console.log("url: "+url);
    postData(url, tfmcu);
}

function inputSend2mcu() {
    cInputSend2mcu(document.getElementById('send-c').value);
}


function inputConfigReset() {
    var tfmcu = JSON.parse(sessionStorage.getItem("tfmcu.config"));
    var cfg = tfmcu.config;
    Object.keys(cfg).forEach (function (key, idx) {
        var new_val = document.getElementById('cfg.'+key).value = cfg[key];
    });
}

function json2html(cfg) {
    // document.writeln('<!DOCTYPE html><meta charset="UTF-8"><script src="hp.js"></script>');
    var html = "<h3>MCU Configuration</h3><table>"
    Object.keys(cfg).forEach (function (key, idx) {
        html += '<tr>'+config_item(key, cfg[key])+'</tr>'+"\n";
    });
    html +='</table>'
    html += '<button type="button" onclick="fetch_json(true)">Reload</button>';
    html += '<button type="button" onclick="inputConfigReset()">Reset</button>';
    html += '<button type="button" onclick="inputConfig2mcu()">Save</button>';
    return html;
}



function fetch_json(u=false) {
    var url = base+'/config.json';
    var result = {name: "error"};
    fetch (url, {
        method: 'get',
        headers: {
            Accept: 'application/json'
        },
    },
          ).then(response => {
              if(response.ok) {
                  response.json().then(json => {
                      //document.writeln("ok"+JSON.stringify(json));
                      var cfg = json.config;
                      if (u) {
                          jsonUpdateHtml(cfg);
                      } else {
                          document.getElementById("config-div").innerHTML = json2html(cfg);
                      }
                      sessionStorage.setItem("tfmcu.config", JSON.stringify(json));
                  });
              }
          });
}
