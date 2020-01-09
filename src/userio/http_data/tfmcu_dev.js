var base = '';
var base = 'http://192.168.1.69'; //dev-delete-line//

var gmu = [0,1,2,3,4,5,6,7];
var gu = [0,1,2,3,4,5,6,7];
var gu_idx = 0;

var tfmcu_config = {};
let config_fetched = false;
let cuas_Interval;
let cuas_State = 0;

class AppState {

    constructor() {
        this.mG = Number.parseInt((localStorage.getItem("group") || "0"), 10);
        this.mM = Number.parseInt((localStorage.getItem("member") || "0"), 10);
        this.mTabVisibility = Number.parseInt((localStorage.getItem("tab_vis") || "1"), 10);
        this.mAuto = {link:{}, auto:{}};
    }

    set tabVisibility(value) {
	this.mTabVisibility = value;
        localStorage.setItem("tab_vis", value.toString());
	tabSwitchVisibility(value);
    }

    get g() {
        return this.mG;
    }

    getAutoName() { return  "auto" + this.g.toString() + this.m.toString(); }
    getAutoObj() { let key = this.getAutoName(); return (key in this.mAuto.auto) ? this.mAuto.auto[key] : {}; }
    linkAutoObj() { this.mAuto.link = this.getAutoObj(); }
    gmChanged() {
        this.linkAutoObj();
        this.updateAutomaticHtml();
        console.log(JSON.stringify(this));
        this.fetchAutomatic();
    }

    set g(value) {
        this.mG = value;
        localStorage.setItem("group", value.toString());
        document.getElementById("sgi").value = value ? value : "A";
        this.gmChanged();
    }

    get m() {
        return this.mM;
    }

    set m(value) {
        this.mM = value;
        localStorage.setItem("member", value.toString());
        document.getElementById("smi").value = value ? value : "A";
        this.gmChanged();
    }

    get auto() {
        return this.mAuto.link;
    }

    set auto(obj) {
        this.mAuto.auto[this.getAutoName()] = obj;
        this.linkAutoObj();
        this.updateAutomaticHtml();
    }

    updateAutomaticHtml() {
        let auto = this.auto;
        document.getElementById('tfti').value = ("f" in auto) ? auto.f : "";

        document.getElementById('tati').value = ("astro" in auto) ? auto.astro : "";
        document.getElementById('twti').value = ("weekly" in auto) ? auto.weekly : "";

        let f = ("f" in auto) ? auto.f : "";
        document.getElementById('tdci').checked = f.indexOf("D") >= 0;
        document.getElementById('twci').checked = f.indexOf("W") >= 0;
        document.getElementById('taci').checked = f.indexOf("A") >= 0;
        document.getElementById('trci').checked = f.indexOf("R") >= 0;
        document.getElementById('tsci').checked = f.indexOf("S") >= 0;
        document.getElementById('tmci').checked = f.indexOf("M") >= 0;

        let up_elem = document.getElementById('tduti');
        let down_elem = document.getElementById('tddti');
        up_elem.value = "";
        down_elem.value = "";

        if ("daily" in auto) {
            console.log("------");
            let d = auto.daily;
            let l = auto.daily.length;
            up_elem.value = d.startsWith("-") ? "" : d.substring(0,2)+":"+d.substring(2,4);
            down_elem.value = d.endsWith("-") ? "" : d.substring(l-4,l-2)+":"+d.substring(l-2);
        }
    }

    handleFetchedData(obj) {
        console.log("reply-json: "+JSON.stringify(obj));

        if ("config" in obj) {
	    let config = obj.config;

	    if ("cuas" in config) {
		if (config.cuas != cuas_State) {
		    let s = "";
		    switch(config.cuas) {
		    case 0: s = ""; break;
		    case 1: s = '<strong style="animation: blink .75s linear 4;"> ...Scanning... </strong>'; break;
		    case 2: s = '<strong style="background-color:red;">Time-Out! (no STOP-command received)</strong>'; break;
		    case 3: s = '<strong style="background-color:green;">Success (cu-id has been saved)</strong>';
			this.fetchConfig();
			break;
		    }

		    if (config.cuas > 1) {
			window.clearInterval(cuas_Interval);
		    }
		    document.getElementById("id_cuasStatus").innerHTML = s;
		    config.cuas = cuas_State;
		}
            } else if (config_fetched) {
                updateHtmlByConfigData(obj.config);
		tfmcu_config = obj;
		gmuUnpack();
            } else {
                config_fetched = true;
                document.getElementById("config-div").innerHTML = buildHtmlByConfigData(obj.config);
		tfmcu_config = obj;
		gmuUnpack();
            }

        }

        if ("position" in obj) {
            document.getElementById('spi').value = obj.position.p;
        }

	if ("mcu" in obj) {
	    let mcu = obj.mcu;
	    if ("chip" in mcu) {
		document.getElementById("id_chip").innerHTML = mcu.chip;
	    }
	    if ("firmware" in mcu) {
		document.getElementById("id_firmware").innerHTML = mcu.firmware;
	    }
	    if ("build-time" in mcu) {
		document.getElementById("id_buildTime").innerHTML = mcu["build-time"];
	    }

	}

        if (this.getAutoName() in obj) {
            this.auto = obj[this.getAutoName()];
        }
    }


    fetchConfig() {
        var url = base+'/config.json';
        fetch (url, {
            method: 'get',
            headers: {
                Accept: 'application/json'
            },
        }).then(response => {
            if(response.ok) {
                response.json().then(obj => this.handleFetchedData(obj));
            }
        });
    }


    fetchAutomatic() {

        let tfmcu = {to:"tfmcu"};

        tfmcu.timer = {
            g: this.g,
            m: this.m,
            f: "uki",
        };

        let url = base+'/cmd.json';
        postData(url, tfmcu);
    }


    fetchVersion() {

        let tfmcu = {to:"tfmcu"};

        tfmcu.mcu = { version:"?" };

        let url = base+'/cmd.json';
        postData(url, tfmcu);
    }

    load() {
        this.g = this.mG;
        this.m = this.mM;
        this.tabIdx = this.mTabIdx;
	this.fetchVersion();
        this.fetchConfig(); //FIXME: needed here for group/member numbers
        this.tabSwitchVisibility = this.mTabVisibility;
    }

}

let app_state;


function buildConfigTableRowHtml(name,value) {
    if (name.endsWith("-enable")) {
        console.log("value: "+value);
        return '<td><label class="config-label">'+name+
            '</label></td><td><input class="config-input cb" type="checkbox" id="cfg_'+name+
            '" name="'+name +'"' + (value ? " checked" : "") +'></td>';
    } else if (name === 'verbose') {
	        return '<td><label class="config-label">'+name+
            '</label></td><td><input class="config-input" type="number" min="0" max="5" id="cfg_'+name+
            '" name="'+name+'" value="'+value+'"></td>';
    } else if (name === 'network') {
	        return '<td><label class="config-label">'+name+
            '</label></td><td><select  class="config-input" id="cfg_'+name+'">'+
               '<option value="wlan">Connect to existing WLAN</option>'+
               '<option value="ap">WLAN-AP at 192.168.1.4</option>'+
               '<option value="lan">Connect to Ethernet</option>'+
            '</select></td>';
    } else {
        return '<td><label class="config-label">'+name+
            '</label></td><td><input class="config-input text" type="text" id="cfg_'+name+
            '" name="'+name+'" value="'+value+'"></td>';
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

function buildHtmlByConfigData(cfg) {
    var html ="<table class=\"conf-table\";\">";
    Object.keys(cfg).forEach (function (key, idx) {
        html += '<tr id="cfg_'+key+'_tr">'+buildConfigTableRowHtml(key, cfg[key])+'</tr>'+"\n";
    });
    html +='</table>';
    return html;
}

function updateHtmlByConfigData(cfg) {
    Object.keys(cfg).forEach (function (key, idx) {
        let el = document.getElementById('cfg_'+key);

        switch(el.type) {
        case 'checkbox':
            el.checked = cfg[key] != 0;
            break;
        default:
            el.value = cfg[key];
        }
    });
}

function postData(url = '', data = {}) {
    // Default options are marked with *
    console.log("post-json: "+JSON.stringify(data));
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
                    app_state.handleFetchedData(json);
                });
            }
        });
}

function postMcuRestart() {
    var json = { to:"tfmcu", config: { restart:"1" } };
    var url = base+'/cmd.json';
    postData(url, json);
}


function postCuasStatus() {
    var json = { to:"tfmcu", config: { cuas:"?" } };
    var url = base+'/cmd.json';
    postData(url, json);
}

function postCuasStart() {
    var json = { to:"tfmcu", config: { cu:"auto" } };
    var url = base+'/cmd.json';
    postData(url, json);
    cuas_State = 0;
    cuas_Interval = window.setInterval(postCuasStatus, 1000);
}



function postConfig() {
    let tfmcu = Object.assign({},tfmcu_config);
    var cfg = tfmcu.config;
    var new_cfg = {};
    var has_changed = false;
    gmuPack();
    Object.keys(cfg).forEach (function (key, idx) {
        let new_val = 0;
        let el = document.getElementById('cfg_'+key);
        console.log("key: "+key);

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

function postSendCommand(c=document.getElementById('send-c').value) {
    var tfmcu = {to:"tfmcu"};
    let g = app_state.g.toString();
    let m = app_state.m.toString();

    var send = {
        g: g,
        m: m,
        c: c,
    };
    tfmcu.send = send;
    console.log(JSON.stringify(tfmcu));
    var url = base+'/cmd.json';
    console.log("url: "+url);
    postData(url, tfmcu);
}

function netFirmwareOTA(fwUrl) {
    // TODO: validate URL here
    var netmcu = {to:"tfmcu"};
    netmcu.mcu = {
	ota: fwUrl
    };
    let url = base+'/cmd.json';
    console.log("url: "+url);
    postData(url, netmcu);
}

function getGuIdx() {
    let val = app_state.g;

    for(let i=0; i < gu.length; ++i)
        if (gu[i] == val)
            return i;
    return 0;
}

function onGPressed() {
    let val = app_state.g;

    gu_idx = getGuIdx(); //FIXME

    ++gu_idx;
    if (gu_idx >= gu.length)
        gu_idx = 0;

    val = gu[gu_idx];

    app_state.g = val;
    if (val == 1)
        app_state.m = 1;
}


function onMPressed() {
    let val = app_state.m;

    ++val;
    gu_idx = getGuIdx(); //FIXME
    let g  = gu[gu_idx];

    if (val > gmu[g])
        val = 0;

    app_state.m = val;
}

function postAutomatic() {
    let url = base+'/cmd.json';
    let tfmcu = { to:"tfmcu", timer: { }};
    let auto = tfmcu.timer;
    let has_daily = false, has_weekly = false, has_astro = false;

    auto.g = app_state.g;
    auto.m = app_state.m;

    let f = "i";
    f += document.getElementById('tmci').checked ? "M" : "m";
    f += (has_daily = document.getElementById('tdci').checked) ? "D" : "d";
    f += (has_weekly = document.getElementById('twci').checked) ? "W" : "w";
    f += (has_astro = document.getElementById('taci').checked) ? "A" : "a";
    f += document.getElementById('trci').checked ? "R" : "r";
    f += document.getElementById('tsci').checked ? "S" : "s";
    auto.f = f;

    if (has_weekly)
        auto.weekly = document.getElementById('twti').value;
    if (has_astro)
        auto.astro = document.getElementById('tati').value;
    if (has_daily) {
        let tdu = document.getElementById('tduti').value;
        let tdd = document.getElementById('tddti').value;
        let td = tdu.length !== 5 ? "-" : tdu.substring(0,2) + tdu.substring(3,5);
        td +=  tdd.length !== 5 ? "-" : tdd.substring(0,2) + tdd.substring(3,5);
        auto.daily = td;
    }



    console.log(JSON.stringify(tfmcu));
    postData(url, tfmcu);

}

function clearAutomatic() {
    document.getElementById('tfti').value = "";
    document.getElementById('tduti').value = "";
    document.getElementById('tddti').value = "";
    document.getElementById('tati').value = "";
    document.getElementById('twti').value = "";
    document.getElementById('tdci').checked = false;
    document.getElementById('twci').checked = false;
    document.getElementById('taci').checked = false;
    document.getElementById('trci').checked = false;
    document.getElementById('tsci').checked = false;
    document.getElementById('tmci').checked = false;
}

const VIS_SEND = 0x01;
const VIS_AUTO  = 0x02;
const VIS_CONFIG = 0x04;
const VIS_FIRMWARE = 0x08;

let tabs = [
    { 'mask':1, 'button_id':'stb', 'div_id':'senddiv' },
    { 'mask':2, 'button_id':'atb', 'div_id':'autodiv' },
    { 'mask':4, 'button_id':'ctb', 'div_id':'configdiv' },
    { 'mask':8, 'button_id':'ftb', 'div_id':'id-fwDiv' }
];


function tabSwitchVisibility(mask) {
    const NONE = "none";
    const SHOW = "";
    const BGC1 = "hsl(220, 60%, 60%)";
    const BGC0 = "#eee";

    for (let i=0; i < tabs.length; ++i) {
        document.getElementById(tabs[i].div_id).style.display = (mask & tabs[i].mask) ? SHOW : NONE;
        document.getElementById(tabs[i].button_id).style.backgroundColor =  (mask & tabs[i].mask) ? BGC1 : BGC0;
    }
}

function tabMakeVisibleByIdx_OLD(idx) {
    let sendCont =  document.getElementById("senddiv");
    let autoCont =  document.getElementById("autodiv");
    let confCont =  document.getElementById("configdiv");
    const NONE = "none";
    const SHOW = "";
    const BGC1 = "hsl(220, 60%, 60%)";
    const BGC0 = "#eee";

    for (let i = 0; i < tabButtons.length; ++i) {
        document.getElementById(tabButtons[i]).style.backgroundColor = (i === idx ? BGC1 : BGC0);
    }

    switch(idx) {
    case 0:
        sendCont.style.display = SHOW;
        autoCont.style.display = NONE;
        confCont.style.display = NONE;
        break;
    case 1:
        sendCont.style.display = SHOW;
        autoCont.style.display = SHOW;
        confCont.style.display = NONE;
        app_state.fetchAutomatic();
        break;
    case 2:
        sendCont.style.display = NONE;
        autoCont.style.display = NONE;
        confCont.style.display = SHOW;
        app_state.fetchConfig();
        break;
    }
}

function onContentLoaded() {
    app_state = new AppState();
    app_state.load();

    document.getElementById("sgb").onclick = () => onGPressed();
    document.getElementById("smb").onclick = () => onMPressed();
    document.getElementById("sub").onclick = () => postSendCommand('up');
    document.getElementById("ssb").onclick = () => postSendCommand('stop');
    document.getElementById("sdb").onclick = () => postSendCommand('down');

    document.getElementById("arlb").onclick = () => app_state.fetchAutomatic();
    document.getElementById("asvb").onclick = () => postAutomatic();

    document.getElementById("csvb").onclick = () => postConfig();
    document.getElementById("crlb").onclick = () => app_state.fetchConfig();

    document.getElementById("mrtb").onclick = () => postMcuRestart();

    document.getElementById("netota").onclick = () => netFirmwareOTA(document.getElementById("id-esp32FirmwareURL").value);
    document.getElementById("netota_master").onclick = () => netFirmwareOTA('http://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/master/firmware/esp32/tronferno-mcu.bin');
    //document.getElementById("netota_beta").onclick = () => netFirmwareOTA('http://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/beta/firmware/esp32/tronferno-mcu.bin');
    document.getElementById("netota_beta").onclick = () => netFirmwareOTA('http://67.199.248.10/37NTwvp'); //bit.ly/

    //151.101.112.133




//https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/beta/firmware/atmega328/fernotron.hex
    for (let i=0; i < tabs.length; ++i) {
        let tab = tabs[i];
        document.getElementById(tab.button_id).onclick = () => app_state.tabVisibility = tab.mask;
    }

    document.getElementById("id_cuasb").onclick = () => postCuasStart();
}

/*
Local Variables:
compile-command: "jshint tfmcu_dev.js"
End:
*/
