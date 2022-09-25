by Marius Versteegen, 2022

// To help prevent potential headerfile name clashes, the files of CleanRTOS are prefixed with crt_
// The classnames themselves don't need the prefix because of membership of the namespace.
// You can thus use autocompletion after typing crt:: to find all CleanRTOS parts.

This crt folder contains the CleanRTOS core code:

crt_CleanRTOS.h  -  This header includes all the other headers in this folder.
                    To use Clean RTOS, just include is this header file.

crt_Config.h    - This file is included by CleanRTOS.h
                Normally, you don't need to change the settings in it.

MainInits  -  Create an object of this type at the start of your main.cpp.
              It is needed to Initialize CleanRTOS.
              (Timer constructors that are called before the constructor of MainInits 
              will assert-fail.)

Task       -  You can create a task by deriving from this base class (see examples)
              within the main function of a task, you can wait for waitables.

Waitable   -  Waitable is the base class of anything that a task can wait for.
              It is the base class of Flag, Queue and Timer.
              You don't need to use it directly yourself.

Flag	     -  A Flag is a waitable. It is meant for inter task communications.
              The task that owns the flag has a public function that can be 
              called from other tasks. When that happens, the public functions sets the flag.

              The main function of the task that owns the flag should wait for the flag to 
              become "set", and respond to it.

Queue      -  A Queue is a waitable. It is meant for inter task communications.
              The task that owns the queue has a public function that can be 
              called from other tasks. When that happens, the public function feeds data from its input
              parameters into the queue.

              The main function of the task that owns the queueu should wait for the queue to become "nonempty",
              and respond to it (by reading / removing the contents of the queue one by one).

Timer      -  A Timer is a microsecond timer. It can be fire once (20 us or more) or periodic(50 us or more).
              The timer is also a waitable. It can be waited for by the task that owns it.

Mutex      - A mutex could be created for each resource that is shared by multiple threads.
             The mutex can be used to avoid concurrent usage. 
             Potential deadlocks due to misaligned order of locking is automatically detected.
             
             Instead of locking the Mutex directly, it is better to do it via MutexSection instead.
             (which uses Mutex internally). That helps to ensure that every mutex lock is
             always paired with a corresponding unlock.
             
             Note: Most of the times, it is neater instead to make sure that each resource 
             is only directly accessed by a single thread (let's call that thread the 
             "resource keeper"). Multiple threads could then queue resource-interaction 
             commands to the "resource keeper". Use of Mutex(-Sections) can be omitted, then.

MutexSection - During the lifetime of a MutexSection object, the associated mutex is locked.
             

Handler    -  A Handler object offers a convenient way to execute objects that periodically
              perform a task within a single thread, by periodically calling their update()
              function. Thus, resources associated with thread overhead can be saved.

IHandler   -  Handler derives from IHandler. 
              Every object that is to be driven by a Handler, registers itself
              at that Handler its IHandler interface.

IHandlerListener - Every object that is to be driven by a Handler, should derive from IHandlerListener.

Logger     -  A maximally fast logger, meant for debugging purposes.
              // With CleanRTOS, the main file is responsible for setting up global objects.
              // A global object that should be normally created is a logger.