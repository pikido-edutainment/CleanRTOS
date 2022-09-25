by Marius Versteegen, 2022

Installation instructions of ESP_IDF including Arduino_IDE platform as
a component. 

The first part is demonstrated in the following youtube video:
https://youtu.be/WFLV4CrmXe8

(At this moment, it's a dutch video. You could turn on translated subtitles for now)

***************************************************************************
Adjustments to sdkconfig
***************************************************************************

I only like to use dual cores. So I uncommented next line:
CONFIG_FREERTOS_UNICORE=n

To be able to flash faster, I changed:
CONFIG_ESPTOOLPY_FLASHFREQ_40M=y to
CONFIG_ESPTOOLPY_FLASHFREQ_80M=y

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
