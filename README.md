# CoffeeLOG
Access control / Log for Coffee Grinders (or other devices), using RFID cards

**Video:** https://youtu.be/XY7MGmBWQRc

**From “Blynk” app you can set:**

- ON time for the grinder (0.1 sec = 1 unit in app)
- Lock / Unlock the issued RFID cards (realtime) -> Select a User -> Push “Lock/Unlock” button
- Turn ON the grinder for “Guest” in app → Push “Grind Now for Guest” button
- Replace a “Lost Card”  -> Select a User ->  Push “Lost Card” button -> approach a New Card to the RFID module
- Reset the counters to “0”, for all Users → Push “Reset” button

**Hardware Components:**

- Wemos D1 Mini or NodeMCU (ESP8266)
- RFID module – MFRC522
- RFID cards/tags (13.56MHz)
- SSR to control the Grinder

**Software:**

- install “Blynk”(Legacy) app from Play Store / App Store.
- Create a “Blynk” account
- Scan the provided QR code for this project (you need to buy some “Energy” points)
- Check the Blynk1,2,3 pictures for the next steps
- Copy the provided Libraries to Arduino > Libraries directory
- Replace the “xxxxxxx” in the code, with your “blynk_token” received from Blynk
- After programming the WiFi module, connect to the “CoffeLOG” network Access Point, started by the WiFi module
- Type this IP address in your web browser, to access the device Config Portal: 192.168.4.1
- Configure WiFi > Select your WiFi network > Insert the Password  > Save.
- Now, after AutoRestart, the module should be visible in the Blynk app.

Note: while the grinder is ON, the communication with Blynk app is interrupted.

