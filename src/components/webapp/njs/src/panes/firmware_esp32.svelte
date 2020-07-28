<script>
  import McuFirmwareUpd from "../mcu_firmware_upd.svelte";
  import McuFirmwareInfo from "../mcu_firmware_info.svelte";
  import McuFwGitTags from "../mcu_firmware_git_tags.svelte";
  import * as misc from "../misc.js";

  let fwbtns = [
    { name: "latest master firmware", ota_name: "github-master" },
    { name: "latest beta firmware", ota_name: "github-beta" },
  ];

  if (!misc.DISTRO) {
    fwbtns.push({
      name: "firmware from given URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.70:3000/tronferno-mcu.bin",
    });
  }
</script>

<div class="area">
  <McuFirmwareUpd {fwbtns} {McuFwGitTags} chip="" updSecs="15" />
  <McuFirmwareInfo />
</div>
