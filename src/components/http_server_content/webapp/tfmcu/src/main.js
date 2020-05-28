'use strict';
// src/main.js
//import { version } from '../package.json';
import App from './tfmcu_html.html';
import { onContentLoaded } from './misc.js';
import * as testing from './testing.js';

export default function() {

  const app = new App({
    target: document.body,
    props: {
      name: 'world'
    }
  });

  //console.log('version ' + version);
  testing.testing_genHtml();
  onContentLoaded();
  testing.testing_netota();


}




