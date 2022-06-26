<script>
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import McuFirmwareUpd from "app/mcu_firmware_upd.svelte";
  import McuFirmwareInfo from "app/mcu_firmware_info.svelte";
  import * as misc from "app/misc.js";

  import { McuGitTagNames, McuGitTagNameLatestMaster, McuGitTagNameLatestBeta } from "stores/mcu_firmware";

  let fw_master = {
    name: $_('firmware.latest_master'),
    input: "none",
    get_ota_name: () => {
      return "github-master";
    },
  };

  let fw_beta = {
    name: $_('firmware.latest_beta'),
    input: "none",
    get_ota_name: () => {
      return "github-beta";
    },
  };

  let fw_version = {
    name: $_('firmware.version'),
    input: "select",
    get_ota_name: () => {
      return "tag:" + fw_version.value;
    },
  };

  let fwbtns = [fw_master, fw_beta, fw_version];

  $: {
    fw_master.version = $McuGitTagNameLatestMaster;
    fw_beta.version = $McuGitTagNameLatestBeta;
    fw_version.values = $McuGitTagNames;
    fwbtns = fwbtns;    
  }

  $: fw_version_value0 = $McuGitTagNames[0];
  $: {
    fw_version.value = fw_version_value0;
  }

  if (!misc.DISTRO) {
    let fw_url = {
      name: $_('firmware.url'),
      input: "input",
      value: "http://192.168.1.76:3005/tronferno-mcu.bin",
      get_ota_name: () => {
        return fw_url.value;
      },
    };
    fwbtns.push(fw_url);
  }
</script>

<div class="area">
  <h4  class="text-center" use:tippy={{ content: $_("app.navTab.cfg.mcu.ota.tt.header") }}>{$_("app.navTab.cfg.mcu.ota.header")}</h4>

  <McuFirmwareUpd bind:fwbtns chip="" updSecs="30" />
  <McuFirmwareInfo />
</div>
