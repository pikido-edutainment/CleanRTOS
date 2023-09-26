# by Marius Versteegen, 2022

# Of course, it's up to you where you choose to store your libraries.

# My personal preference is not to add any additional libraries to the
# libraries folder of the Arduino component.

# Instead additional libraries - CleanRTOS and any other third party libraries,
# I chose to store in a folder called "libs", which resides in the same folder
# as the folder "main" does.

# Thus, it stays clear for me which libraries came with the standard Arduino component
# and which ones I added myself.

# Note: CleanRTOS does not depend on the standard Arduino component, so 
# you can ommit that as you like.

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.cpp")
set(COMPONENT_ADD_INCLUDEDIRS 
"."
"../libs/CleanRTOS/src" 
"../libs/CleanRTOS/src/internals" 
"../libs/CleanRTOS/examples/AllWaitables"
"../libs/CleanRTOS/examples/Flag"
"../libs/CleanRTOS/examples/Queue"
"../libs/CleanRTOS/examples/Timer"
"../libs/CleanRTOS/examples/HelloWorld"
"../libs/CleanRTOS/examples/TwoTasks"
"../libs/CleanRTOS/examples/MutexSection"
"../libs/CleanRTOS/examples/Pool"
"../libs/CleanRTOS/examples/Handler"
"../libs/CleanRTOS/examples/Logger"
"../libs/CleanRTOS/examples/TenTasks"
)

register_component()

# PS: it is possible to use the CleanRTOS library as a component instead.
# To do that, follow the instructions in the CMakeLists file in the folder
# "optional when using as component"