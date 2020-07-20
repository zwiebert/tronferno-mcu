<script>
  import McuFirmwareUpd from "../mcu_firmware_upd.svelte";
  import McuFirmwareInfo from "../mcu_firmware_info.svelte";
  import McuFwGitTags from "../mcu_firmware_git_tags.svelte";

  export let isProduction = false;

  let fwbtns = [
    { name: "latest master firmware", ota_name: "github-master" },
    { name: "latest beta firmware", ota_name: "github-beta" },
  ];

  // eslint-disable-next-line no-unused-labels
  testing: if (!isProduction) {
    fwbtns.push({
      name: "firmware from given URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.70:3000/tronferno-mcu.bin",
    });
  }
</script>

<div class="area">
     
    <h4>NetMCU ESP32</h4>
    <McuFirmwareUpd {fwbtns}  {McuFwGitTags} chip="" updSecs="15" />
    <McuFirmwareInfo />
 
</div>