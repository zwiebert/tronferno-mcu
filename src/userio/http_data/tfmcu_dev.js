var base = '';
var base = 'http://10.0.0.20'; //dev-delete-line//

var gmu = [0,1,2,3,4,5,6,7];
var gu = [0,1,2,3,4,5,6,7];
var gu_idx = 0;

var cfg_out = {};

function handle_json_reply(json) {
    console.log("reply-json: "+JSON.stringify(json));
    let g = document.getElementById('sgi').value.toString();
    let m = document.getElementById('smi').value.toString();

    if ("position" in json) {
        document.getElementById('spi').value = json.position.p;
    }
    if ("config" in json) {
        jsonUpdateHtml(json.config);
    }
    let auto_name = "auto"+g+m;
    console.log(auto_name);
    if (auto_name in json) {
        let auto = json[auto_name];
        document.getElementById('tfti').value = ("f" in auto) ? auto.f : "";
        document.getElementById('tdti').value = ("daily" in auto) ? auto.daily : "";
        document.getElementById('tati').value = ("astro" in auto) ? auto.astro : "";
        document.getElementById('twti').value = ("weekly" in auto) ? auto.weekly : "";

        if ("f" in auto) {
            let f = auto.f;
            document.getElementById('tdci').checked = f.indexOf("D") >= 0;
            document.getElementById('twci').checked = f.indexOf("W") >= 0;
            document.getElementById('taci').checked = f.indexOf("A") >= 0;
            document.getElementById('trci').checked = f.indexOf("R") >= 0;
            document.getElementById('tsci').checked = f.indexOf("S") >= 0;
            document.getElementById('tmci').checked = f.indexOf("M") >= 0;
        }

    }
}

function config_item(name,value) {
    if (name.endsWith("-enable")) {
        console.log("value: "+value);
        return '<td><label class="config-label">'+name+'</label></td><td><input class="config-input cb" type="checkbox" id="cfg_'
            +name+'" name="'+name +'"' + (value ? " checked" : "") +'></td>';
    } else {
        return '<td><label class="config-label">'+name+'</label></td><td><input class="config-input" type="text" id="cfg_'
            +name+'" name="'+name+'" value="'+value+'"></td>';
    }

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

function jsonUpdateHtml(cfg) {
    Object.keys(cfg).forEach (function (key, idx) {
        let el = document.getElementById('cfg_'+key);
        switch(el.type) {
        case 'checkbox':
            el.checked = cfg[key] != 0;
            break;
        default:
            el.value = cfg[key]
        }
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
        .then(response => {
            if(response.ok) {
                response.json().then(json => {
                    handle_json_reply(json);
                });
            }
        });
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
        let new_val = 0;
        let el = document.getElementById('cfg_'+key);
        switch(el.type) {
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
            console.log(key);
        }
    });

    if (has_changed) {
        new_cfg.all = "?";
        tfmcu.config = new_cfg;
        console.log(JSON.stringify(tfmcu));
        var url = base+'/cmd.json';
        console.log("url: "+url);
        postData(url, tfmcu);
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
    var html ="<table>"
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

    autoFetch();
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
    autoFetch();
}

function autoInput2G() {
    let gs = document.getElementById('sgi').value;
    let g = gs === 'A' ? 0 : Number.parseInt(gs);
    return g;
}
function autoInput2M() {
    let ms = document.getElementById('smi').value;
    let m = ms === 'A' ? 0 : Number.parseInt(ms);
    return m;
}

function autoSave() {
    let url = base+'/cmd.json';
    let tfmcu = { name: "tfmcu", timer: { }};
    let auto = tfmcu.timer;
    let has_daily = false, has_weekly = false, has_astro = false;

    auto.g = autoInput2G();
    auto.m = autoInput2M();

    let f = "i";
    f += document.getElementById('tmci').checked ? "M" : "m";
    f += (has_daily = document.getElementById('tdci').checked) ? "D" : "d";
    f += (has_weekly = document.getElementById('twci').checked) ? "W" : "w";
    f += (has_astro = document.getElementById('taci').checked) ? "A" : "a";
    f += document.getElementById('trci').checked ? "R" : "r";
    f += document.getElementById('tsci').checked ? "S" : "s";
    auto.f = f;

    if (has_daily) {
        let val =  document.getElementById('tdti').value;
        auto.daily = val;
    }

    if (has_weekly) {
        let val =  document.getElementById('twti').value;
        auto.weekly = val;
    }

    if (has_astro) {
        let val =  document.getElementById('tati').value;
        auto.astro = val ? Number.parseInt(val) : 0;
    }

    console.log(JSON.stringify(tfmcu));
    postData(url, tfmcu);

}

function autoClear() {
    document.getElementById('tfti').value = "";
    document.getElementById('tdti').value = "";
    document.getElementById('tati').value = "";
    document.getElementById('twti').value = "";
    document.getElementById('tdci').checked = false;
    document.getElementById('twci').checked = false;
    document.getElementById('taci').checked = false;
    document.getElementById('trci').checked = false;
    document.getElementById('tsci').checked = false;
    document.getElementById('tmci').checked = false;
}

function autoFetch() {
    autoClear();

    let tfmcu = {name:"tfmcu"};
    let g = document.getElementById('sgi').value.toString();
    let m = document.getElementById('smi').value.toString();

    tfmcu.timer = {
        g: g,
        m: m,
        f: "uki",
    };

    let url = base+'/cmd.json';
    postData(url, tfmcu);
}

function testPressed() {
    var tfmcu = {name:"tfmcu"};
    var xtfmcu = {};
    let g = document.getElementById('sgi').value.toString();
    let m = document.getElementById('smi').value.toString();

    xtfmcu.send = {
        g: "2",
        m: "2",
        p: "?",
    };

    tfmcu.timer = {
        g: g,
        m: m,
        f: "uki",
    };

    xtfmcu.config = {
        all: "?"
    };

    console.log(JSON.stringify(tfmcu));
    var url = base+'/cmd.json';
    console.log("url: "+url);
    postData(url, tfmcu);
}

function onContentLoaded() {
    fetch_json(false);

    document.getElementById("sgb").onclick = gPressed;
    document.getElementById("smb").onclick = mPressed;
    document.getElementById("sub").onclick = () => cInputSend2mcu('up');
    document.getElementById("ssb").onclick = () => cInputSend2mcu('stop');
    document.getElementById("sdb").onclick = () => cInputSend2mcu('down');

    document.getElementById("arlb").onclick = autoFetch;
    document.getElementById("asvb").onclick = autoSave;

    document.getElementById("crtb").onclick = inputConfigReset;
    document.getElementById("csvb").onclick = inputConfig2mcu;
    document.getElementById("crlb").onclick = () => fetch_json(true);

    document.getElementById("mrtb").onclick = mcuRestart;

}
