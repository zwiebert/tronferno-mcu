import { PersistentOptionStore } from "./custom_stores.js";
import { derived, get } from "svelte/store";

export const McuDocs = PersistentOptionStore();


export const McuDocs_cliHelpConfig = derived(McuDocs, (docs) => {
    let result = {};

   const txt = docs["/f/cli/help/config"];
   if (!txt)
     return result;

   const lines = txt.text.split(/\r\n|\r|\n/);

   let lastKey = "";

   for (let i=2; i < lines.length; ++i) {
     const line = lines[i];
     const ma = line.match(/^([a-zA-Z0-9-]+)[= ].+/);
     if (ma) {
       lastKey = ma[1];
       result[lastKey] = ma[0];
     } else if (lastKey) {
        result[lastKey] += "\n" + line;
     }
   }

   return result;
});
