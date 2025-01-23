<script>
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import { GuiAcc } from "../store/app_state";
  import McuFirmwareUpd from "../app/mcu_firmware_upd.svelte";
  import McuFirmwareInfo from "../app/mcu_firmware_info.svelte";
  import * as misc from "../app/misc.js";

  import { McuGitTagNames, McuGitTagNameLatestMaster, McuGitTagNameLatestBeta } from "../store/mcu_firmware";

  let fw_master = $derived({
    name: $_("firmware.latest_master"),
    input: "none",
    get_ota_name: () => {
      return "tag:master";
    },
    version : $McuGitTagNameLatestMaster,
  });

  let fw_beta = $derived({
    name: $_("firmware.latest_beta"),
    input: "none",
    get_ota_name: () => {
      return "tag:beta";
    },
    version : $McuGitTagNameLatestBeta,
  });

  let fw_version = $derived({
    name: $_("firmware.version"),
    input: "select",
    get_ota_name: () => {
      return "tag:" + fw_version.version;
    },
    version : $McuGitTagNames[0],
    values : $McuGitTagNames,
  });
  
  let fw_url = {
    name: $_("firmware.url"),
    input: "input",
    url: "http://192.168.1.76:3005/tronferno-mcu.bin",
    get_ota_name: () => {
      return fw_url.url;
    },
  };

  let fwbtns = $derived([fw_master, fw_beta, fw_version, ...misc.DISTRO? [] : [fw_url]]);



</script>

{#if $GuiAcc.ota}
  <div class="main-area">
    <h4 class="text-center" use:tippy={{ content: $_("panes.ota.tt.header") }}>{$_("panes.ota.header")}</h4>

    <McuFirmwareUpd fwbtns={fwbtns} chip=""  />
    <div class="area">
      <McuFirmwareInfo />
    </div>
  </div>
{/if}
