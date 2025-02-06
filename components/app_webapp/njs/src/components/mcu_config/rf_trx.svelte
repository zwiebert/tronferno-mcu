<script>
  let { name, value = $bindable() } = $props();
</script>

<select class="config-input" id="cfg_{name}" bind:value>
  <option value="none">None</option>
  <option value="rx-tx">RX & TX</option>
  <option value="rx-itx">RX & inverse-TX</option>
  <option value="cc1101">CC1101 Transceiver</option>
</select>
