<!-- @migration-task Error while migrating Svelte code: `<tr>` cannot be a child of `<table>`. `<table>` only allows these children: `<caption>`, `<colgroup>`, `<tbody>`, `<thead>`, `<tfoot>`, `<style>`, `<script>`, `<template>`. The browser will 'repair' the HTML (by moving, removing, or inserting elements) which breaks Svelte's assumptions about the structure of your components. -->
<script>
  "use strict";
  import { Verbose, TabIdx } from "../store/app_state.js";

</script>

<table>
  <tr><th></th><td></td></tr>
  <tr><th>Show Help</th><td><input type="checkbox" bind:checked={$TabIdx.showHelp} /></td></tr>
</table>