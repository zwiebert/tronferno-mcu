<script>
  export let name;
  export let value;
</script>

<select class="config-input" id="cfg_{name}" bind:value={value}>
  <option value="lan8720">LAN8720</option>
  <option value="rtl8201">RTL8201</option>
  <option value="ip101">IP101</option>
</select>