'use strict';
// src/main.js
//import { version } from '../package.json';

import { onContentLoaded } from './misc.js';
import * as testing from './testing.svelte';




export default function() {
  generate_html();

  //console.log('version ' + version);
  testing.testing_genHtml();
  onContentLoaded();
  testing.testing_netota();


}


import App from './tfmcu_html.svelte';

function generate_html() {
    const app = new App({
    target: document.body,
    props: {
      name: 'world'
    }
  });
}
