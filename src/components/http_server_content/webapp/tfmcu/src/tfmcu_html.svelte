<script>


import ShutterBasic from './shutter_basic.svelte';
import ShutterAuto from './shutter_auto.svelte';
import ShutterAlias from './shutter_alias.svelte';
import ShutterPrefs from './shutter_prefs.svelte';
import McuConfig from './mcu_config.svelte';
import McuFirmware from './mcu_firmware.svelte';

</script>

<!-- Tab links -->
<!--  ------------------------------------------------------------------------------ -->
    <div id="tabBar" class="tab"></div>
<!--  ------------------------------------------------------------------------------ -->
    <div  id="navTabs" class="container tabcontent">
<!--  ------------------------------------------------------------------------------ -->
      <ShutterBasic/>
      <ShutterAuto/>
      <ShutterAlias/>
      <ShutterPrefs/>

<!--  ------------------------------------------------------------------------------ -->


<!--  ------------------------------------------------------------------------------ -->
   <McuConfig/>
   <McuFirmware/>
<!--  ------------------------------------------------------------------------------ -->




<!--  ------------------------------- dev-distro-delete-begin -------------------------------------------- -->
      <div id="testsdiv">

          
      </div>
<!--  ------------------------------- dev-distro-delete-end ----------------------------------------------- -->

    </div>
