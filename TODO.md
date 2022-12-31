TODO
====


* TODO: Write received auto data from 2411 (via RF) to storage in MCU
* TODO: Terminate SET progress bar in web-app when an RF command (e.g. STOP) is received
* FIXME: newly added groups/receivers will not give positiom/pct feedback until MCU restart
* FIXME: astro predictions lags one day behind sometimes (e.g. april-5)
* FIXME: New WLAN-only-ESP32 starts with default network=lan (maybe only in the developer version)
* XXX: E07-MD1101D V2.0 (with long antenna) needs different AGC (05-00-92) than Neuftech CC1101 (07-00-90)
* TODO: Separate web-interface between admin and users
* TODO: Restart webserver after disconnect/reconnect ethernet
* TODO: Check reload after OTA in distribution



DONE
====
* FIXED: ISO time in auto tab of web-app using wrong time zonen (UTC?)
* FIXED: when moving multiple shutters by automatic, only the first one genereates dynamic movement events (MQTT/Websocket/Telnet). Only after movement is done, the position is reportet for all shutters.
* FIXED: sometimes astro movements are not registered (may depend on ntp sync)
ntpd synced: 16:36:24 (FHEM Server time)
astro movement registered: 21, 23, 25 / -2, +2, 0 / 16:34, 16:38, 16:36 (31 / 16:51) (11 ... 15 /16:36)
astro movement ignored: 22, 24,/ -1, +1 / 16:35, 16:37
* FIXED: Make reading cc1101 registers thread safe
* DONE: bubble help for config options
* DONE: Add RF repeater mode for specified Addresses
* DONE: publish timer data on any change. 
* DONE: Make FHEM devices request automatic/timer info
* DONE: CRLF
* FIXED: 2021-03: Our Timer-Interupt is blocked every 100ms if WLAN is enabled (Ethernet is ok)
* DONE: timer flag i should report state after (not before) saving timer data (k flag)


Astro:

Date   pred    real  pidx  ridx
04-05: 20:10   20:18 25    26
04-06: 20:18   20:18 26    26
04-17: 20:36   20:44 28    29

Astro data:
0  16:36, 16:36, 16:36, 16:40, 
4  16:44, 16:48, 16:52, 16:56, 
8  17:02, 17:06, 17:12, 17:18, 
12 17:26, 17:32, 17:40, 17:48, 
16 17:54, 18:02, 18:10, 18:18, 
20 18:26, 18:36, 18:44, 18:52, 
24 1f:00, 1f:10, 1f:18, 1f:28, 
28 1f:36, 1f:44, 1f:54, 20:02, 
32 20:10, 20:18, 20:28, 20:34, 
36 20:42, 20:50, 20:56, 21:02, 
40 21:08, 21:14, 21:18, 21:20, 
44 21:22, 21:24, 21:24, 21:24, 


Table (debug):
Astro data:
16:36, 16:36, 16:38, 16:46, 
16:50, 16:56, 17:02, 17:06, 
17:12, 17:20, 17:26, 17:32, 
17:40, 17:46, 17:52, 18:00, 
18:06, 18:14, 18:22, 18:28, 
18:36, 18:42, 18:50, 18:56, 
1f:04, 1f:12, 1f:18, 1f:28, 
1f:38, 1f:48, 1f:58, 20:08, 
20:16, 20:26, 20:34, 20:42, 
20:50, 20:56, 21:04, 21:08, 
21:14, 21:18, 21:20, 21:22, 
21:24, 21:24, 21:24, 21:24,




EXPERIMENTS
===========

CC1101-Neuftech:
433.914 ... 433.973  (2411@58kHz Bandwith)
 == 433.9435 Mittelwert