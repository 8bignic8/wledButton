# wledButton
This is the Program to set up a esp32 to send HTTP requests to your WLED Network. Or any http Site you want.

After Powering the ESP32 it creates a AP. 
There you can change the HTTP requests, and the Reset Pin. 
After you conect to the Wifi of your choice, it restarts and on Touch Pin 3 (GPIO 15) you can now send the Http request of Short press.

Other funktions are ToDo´s :).

You can change the reset pin in the Wireless config.
To go back to the configuration put the Reset pin to LOW(Ground) and trigger the T3(GPIO 15).

More inforamtion on how to send the right http request you can find here https://github.com/Aircoookie/WLED/wiki/HTTP-request-API

Info: The programm uses DeepSleep so it might work with battery power.
