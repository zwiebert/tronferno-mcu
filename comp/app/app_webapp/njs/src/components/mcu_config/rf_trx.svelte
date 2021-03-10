<script>
  export let name;
  export let value;
</script>

<select class="config-input" id="cfg_{name}" bind:value>
  <option value="none">None</option>
  <option value="rx-tx">RX and TX</option>
  <option value="rx-itx">RX and inverted TX</option>
  <option value="cc1101">CC1101 Transceiver</option>
</select>
