# If you like to use the CleanRTOS library as a component:
#
# 1. Copy CleanRTOS folder to the components folder
#    (if you included the Arduino IDE components, that folder should already
#     be present within the same folder as your "main" folder.)
# 2. Copy this CMakeLists file into the CleanRTOS folder.

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS)
set(COMPONENT_ADD_INCLUDEDIRS 
"."
"src" 
"src/internals" 
"examples/AllWaitables"
"examples/Flag"
"examples/Queue"
"examples/Timer"
"examples/HelloWorld"
"examples/TwoTasks"
"examples/MutexSection"
"examples/Pool"
"examples/Handler"
"examples/Logger"
"examples/TenTasks"
)

register_component()

# PS: it is also possible to install CleanRTOS as a normal, "non-component" library.
  In that case, don't use this CMakeLists file, but follow the instructions in the
  CMakeLists file in the "content for root folder" instead.
