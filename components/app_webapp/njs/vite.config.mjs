import { defineConfig } from "vite";
import { svelte, vitePreprocess } from "@sveltejs/vite-plugin-svelte";
import tailwindcss from "@tailwindcss/vite";
import { compression } from "vite-plugin-compression2";
import path from "path";
import fs from "fs";

// --- CHANGE THIS IP AS NEEDED ---
const ESP_IP = "192.168.1.64";
const build_directory = process.env.BUILD_DIR || "/tmp/rv/njs/build";
const isProduction = process.env.NODE_ENV === "production";


const proxyTarget = `http://${ESP_IP}`;

const wsTarget = `ws://${ESP_IP}`;

export default defineConfig(({ mode }) => {
  const isGithub = mode === "github";
  return {
    resolve: {
      alias: {
        // make sdconfig visible (provide one in src/config too, for dev and builds outside of esp-idf)
        sdkconfig: (process.env.BUILD_DIR
    ? new URL('sdkconfig.json', `file://${process.env.BUILD_DIR}/../../config/sdkconfig.json`).pathname
    : new URL('./src/config/sdkconfig.json', import.meta.url).pathname),
      },
    },
    define: {
      isProduction: isProduction,
      isDistro: process.env.DISTRO === "1",
    },
    plugins: [
      svelte({
        onwarn: (warning, handler) => {
          const { code, frame } = warning;
          // console.log(code); // <= uncomment to check other warnings
          if (code === "css_unused_selector") return;
          if (code === "a11y_invalid_attribute") return;
          handler(warning);
        },
        preprocess: [vitePreprocess()],
      }),
      tailwindcss(),
      // Generates .gz files
      compression({
        algorithm: "gzip",
        exclude: [/\.(br)$/, /\.(gz)$/],
        include: /\.(html|css||js|map)$/,
      }),
      // Generates .br files
      compression({
        algorithm: "brotliCompress",
        exclude: [/\.(br)$/, /\.(gz)$/],
        include: /\.(html|css|js|map)$/,
      }),
    ],
    base: isGithub ? "./" : isProduction ? "/f/" : "/",

    build: {
      outDir: isGithub ? "dist-gh" : build_directory,
      sourcemap: true,
      cssCodeSplit: isGithub, // Allow splitting on GitHub for performance

      rollupOptions: {
        input: isGithub ? "index.html" : "wapp.html",
        output: isGithub
          ? {
              // Standard GitHub Pages settings (hashed)
              entryFileNames: "assets/[name]-[hash].js",
              chunkFileNames: "assets/[name]-[hash].js",
              assetFileNames: "assets/[name]-[hash][extname]",
            }
          : {
              // ESP32 settings (fixed names, no subfolders)
              entryFileNames: "wapp.js",
              assetFileNames: "wapp.[ext]",
              format: "iife",
              name: "wapp",
            },
      },
    },
    server: {
      port: 3000,
      watch: {},
      proxy: {
        // JSON files and /f/ paths
        "^(.*)\\.json$": {
          target: proxyTarget,
          changeOrigin: true,
          agent: false,
          timeout: 5000, // 5 seconds to wait for a response
          proxyTimeout: 5000, // 5 seconds for the proxy itself
          headers: {
            // Tell the ESP32 to close the socket immediately after sending data
            Connection: "close",
          },
        },
        "/f": {
          target: proxyTarget,
          changeOrigin: true,
          timeout: 5000, // 5 seconds to wait for a response
          proxyTimeout: 5000, // 5 seconds for the proxy itself
          agent: false,
          headers: {
            // Tell the ESP32 to close the socket immediately after sending data
            Connection: "close",
          },
        },

        // WebSockets
        "/ws": {
          target: wsTarget,
          ws: true,
          rewrite: (path) => path, // ensures /ws stays /ws
        },
      },
    },
  };
});
