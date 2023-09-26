by Marius Versteegen, 2022

Installation instructions of ESP_IDF including Arduino_IDE platform as
a component. 

1. Install the ESP_IDF after downloading it from espressif.com
2. If you like to be able to use "Arduino IDE" libraries for ESP32 for
   your ESP_IDF as well, just follow the steps from Neil Kolbans pdf "book on ESP32",
   from the chapter: "Using the Arduino libraries as an ESP-IDF component":

   a. Create a new, empty app:
      $ git clone https://github.com/espressif/esp-idf-template.git myapp
   b. Create a new directory called "components" in the app directory:
      $ mkdir components
   d. Change into the components directory:
      $ cd components
   e. Install the Arduino-ESP32 library relative to here by performing a Github Clone:
      $ git clone https://github.com/espressif/arduino-esp32.git

   After that, all you need to do to use Arduino libraries is to use
   #include <Arduino.h>.

3. Make the adjustments shown below in the sdkconfig file of 
   your application.

The above steps are also demonstrated in the following youtube video:
https://youtu.be/WFLV4CrmXe8

(At this moment, it's a dutch video. You could turn on translated subtitles for now)

Note: Some of my students noticed that things run fine from the ESP-CMD prompt,
while use of the ESP-powershell comes with troubles.

***************************************************************************
Adjustments to sdkconfig
***************************************************************************

I only like to use dual cores. So I uncommented next line:
CONFIG_FREERTOS_UNICORE=n

To be able to flash faster, I changed:
CONFIG_ESPTOOLPY_FLASHFREQ_40M=y to
CONFIG_ESPTOOLPY_FLASHFREQ_80M=y
and
CONFIG_ESPTOOLPY_FLASHFREQ="40m" to
CONFIG_ESPTOOLPY_FLASHFREQ="80m"

I like a tick size of 1ms instead of 10ms. (that is pretty common, similar
to for instance the default setting in the Arduino IDE)
Therefore, I adjusted next line:
CONFIG_FREERTOS_HZ=100 to
CONFIG_FREERTOS_HZ=1000

My ESP32 core can run on 240MHZ. Thus I changed next lines:
CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ=160 to
CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ=240
and
CONFIG_ESP32_DEFAULT_CPU_FREQ_160=y to
CONFIG_ESP32_DEFAULT_CPU_FREQ_240=y

During my first build, I got a warning that my microcontroller has 4MB, while
the limit is set to 2MB.
To be able to use the full 4MB, I changed
CONFIG_ESPTOOLPY_FLASHSIZE_2MB=y to
CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y

During development, normally, it is a good idea to show the ESP_LOGI for 
"info" level (level 3) message:
CONFIG_LOG_DEFAULT_LEVEL_INFO=y
CONFIG_LOG_DEFAULT_LEVEL=3
CONFIG_LOG_MAXIMUM_EQUALS_DEFAULT=y

If you really get stuck, you could temporarily raise the log level to "verbose" 
(level 5), to get a bit more information about the error:
CONFIG_LOG_DEFAULT_LEVEL_VERBOSE=y
CONFIG_LOG_DEFAULT_LEVEL=5
CONFIG_LOG_MAXIMUM_EQUALS_DEFAULT=y


***************************************************************************
***************************************************************************
Optional - use "judiciously" / think before acting:

Dependent on the QIO support of your specific chip (see its datasheet), you
could try to double the effective flash speed:
CONFIG_FLASHMODE_DIO=y becomes:
CONFIG_FLASHMODE_QIO=y

If you like to sacrifice part of the adressable memory to be able to use it
as SPDIFF memory, than you can do so as follows:
CONFIG_PARTITION_TABLE_SINGLE_APP=y comment out (put a # before it)
CONFIG_PARTITION_TABLE_CUSTOM=y and set this

CONFIG_PARTITION_TABLE_FILENAME="partitions_singleapp.csv" becomes:
CONFIG_PARTITION_TABLE_FILENAME="partitions.csv"
.. and copy the partitions.csv file from the folder "optional partition file"
to the folder that also contains the sdkconfig file.
