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

Note: The CleanRTOS library does not depend on Arduino IDE.
It can be used on any other ESP32 supporting platform as well.

Enjoy! Marius