<script>
  "use strict";
  import Layout from "./layout.svelte";
  import Wapp from "./wapp.svelte";
  import { setupI18n } from './services/i18n';

  export let isProduction = false;


  setupI18n();
</script>

<Layout />
<Wapp {isProduction} />
