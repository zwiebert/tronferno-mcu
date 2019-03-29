var base = '';
var base = 'http://10.0.0.20'; //dev-delete-line//

var gmu = [0,1,2,3,4,5,6,7];
var gu = [0,1,2,3,4,5,6,7];
var gu_idx = 0;

var cfg_out = {};

function config_item2(name,value) {
    return '<label class="config-label">'+name+'<input class="config-input" type="text" id="cfg_'+name+'" name="'+name+'" value="'+value+'"></label>';
}


function config_item(name,value) {
    return '<td><label class="config-label">'+name+'</label></td><td><input class="config-input" type="text" id="cfg_'+name+'" name="'+name+'" value="'+value+'"></td>';
}

function gmuUnpack() {
    let val = document.getElementById("cfg_gm-used").value; //HEX string! not a number!
    while(val.length < 8)
        val = "0"+val;
    console.log("val: "+val);
    
    let g=1;
    gmu = [0];
    gu = [0];
    for(let i=6; i >= 0; --i,++g) {
        let id = "gmu"+(g.toString());
        let mct = parseInt(val[i], 16);
        if (mct)
            gu.push(g);
        gmu.push(mct);
        document.getElementById(id).value = val[i];
    }
    console.log(gmu);
    console.log(gu);
}

function gmuPack() {
    let val = "";
    
    let written = false; // to skip leading zeros
    for(let i=7; i >= 1; --i) {
        let id = "gmu"+(i.toString());
        let mct = document.getElementById(id).value;
        if (mct != '0' || written) {
            val+= mct;
            written = true;
        }
    }
    let x =  document.getElementById("cfg_gm-used").value;
    val += x[x.length-1]; // push back unused lowest nibble
    
    document.getElementById("cfg_gm-used").value = val;
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
        document.getElementById('cfg_'+key).value = cfg[key];
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
    gmuPack();
    Object.keys(cfg).forEach (function (key, idx) {
        var new_val = document.getElementById('cfg_'+key).value;
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
    let g = document.getElementById('sgi').value;
    let m = document.getElementById('smi').value;
    if (g=="A") g = 0;
    if (m=="A") m = 0;
    var send = {
        g: g,
        m: m,
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
        var new_val = document.getElementById('cfg_'+key).value = cfg[key];
    });
}

function json2html(cfg) {
    // document.writeln('<!DOCTYPE html><meta charset="UTF-8"><script src="hp.js"></script>');
    var html = "<h3>MCU Configuration</h3><table>"
    Object.keys(cfg).forEach (function (key, idx) {
        html += '<tr id="cfg_'+key+'_tr">'+config_item(key, cfg[key])+'</tr>'+"\n";
    });
    html +='</table>'
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
                      gmuUnpack();
                      sessionStorage.setItem("tfmcu.config", JSON.stringify(json));
                  });
              }
          });
}

function getGuIdx() {
    let val = document.getElementById("sgi").value;   
    if (val == "A")
        val = 0;

    for(let i=0; i < gu.length; ++i)
        if (gu[i] == val)
            return i;
    return 0;
}

function gPressed() {
    let val = document.getElementById("sgi").value;   
    if (val == "A")
        val = 0;

    gu_idx = getGuIdx(); //FIXME

    ++gu_idx;
    if (gu_idx >= gu.length)
        gu_idx = 0;
    
    val = gu[gu_idx];

    if (val == 0)
        val = "A";
    document.getElementById("sgi").value = val;
    if (val == 1)
        document.getElementById("smi").value = 1;       
}


function mPressed() {
    let val = document.getElementById("smi").value;   
    if (val == "A")
        val = 0;

    ++val;
    gu_idx = getGuIdx(); //FIXME
    let g  = gu[gu_idx];
                
    if (val > gmu[g])
        val = 0;

    if (val == 0)
        val = "A";
    document.getElementById("smi").value = val;
}


