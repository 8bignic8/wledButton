# wledButton
This is a programm to set up a ESP32 to send HTTP requests to your WLED host (or any other device and or service).

After powering the ESP32 it creates a Access Point.
You need to connect to via a Wireless Network device. 
Than you need to go to the router Adress if you did not get redereced to the ESP32 WiFi Settings menu.
There you can change the HTTP requests, the Reset Pin, the touchpin gpio and the WiFi password of the network it should connect to. 
Afterwards the ESP will restart.

Now you can send the HTTP request you set up before via touchpin 3 (GPIO 15) if you did not change it.

I want to incoperate more functionality like holding or dubble tapping the button in the future :).

To go back to the configuration put the coosen reset pin to LOW(Ground) and trigger the touchpin standart 3(GPIO 15).

More inforamtion about how to send a HTTP request to your WLED Lamp you can find here https://github.com/Aircoookie/WLED/wiki/HTTP-request-API

Additional Info: The programm uses DeepSleep so it might work with a battery power setup. (not tested)
