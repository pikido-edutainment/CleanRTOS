by Marius Versteegen, 2022

Welcome to my CleanRTOS library for ESP32.
CleanRTOS is a collection of wrappers around FreeRTOS.

It lives in the namespace crt (abbreviation of CleanRTOS).
It's purpose is to make use of FreeRTOS easier and safer.

With CleanRTOS, you can easily manage your tasks and 
synchronisations mostly with next user-friendly CleanRTOS objects:

Task   : an object that represents a task.
Flag   : something that can be used to notify another task.
Queue  : a buffer that can be used to receive data from other tasks.
Timer  : a periodic or one-shot timer that can be used to wake a task.

You might also want to checkout the _crt_Readme.txt in the src
folder for a full overview.

Just checkout the examples to see how it can be used.

Enjoy! Marius

PS:
The CleanRTOS library does not depend on Arduino IDE.
It can be used on any other ESP32 supporting platform as well.

For instance for faster building with better debug feedback,
I personally like to use the ESP_IDF. 

PS2:
If you also like to use Arduino libraries for ESP32 that do depend on Arduino IDE,
you can achieve with by a small trick.
Just read the ReadMe file in the folder "extras\for building with ESP_IDF" 
if you'd like to try that road.

PS3:
To view ESP_LOGI output messages in Arduino IDE, 
set Tools -> Core Debug Level to "info" 
and Serial Monitor baud rate to 115200
When using ESP_IDF, make sure that CONFIG_LOG_DEFAULT_LEVEL_INFO=y in sdkconfig