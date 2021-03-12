<script context="module">
'use strict';
import * as httpFetch from "app/fetch.js";

const load_fetch = httpFetch.FETCH_GMU;

export function init() {
 httpFetch.http_fetchByMask(load_fetch);
}
</script>

