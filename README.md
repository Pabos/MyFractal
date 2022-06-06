# MyFractal
The Source folder should contain all what's needed to build a Visual Studio solution with CMake.
CMake will most likely issues some warning about deprecated stuff, which I’ve always ignored.
After launching the Mandel.sln solution, select Mandelbulb as starting project.
At first run, a directory should be created in a user APP Folder (See the Console window) and user setup files will be copied from the program directory.
Modified user setup will only affect the App folder; the setup files in the Program directory will not be modified. They can be used to restore the setup files if anything screws them up.
To reload the setup files from the Program directory just delete the App folder and rerun the program.

If everything worked, a windows will show up, like the one in the Screenshot folder.
The program runs on my desktop on Intel I7, Windows 10 and Nvidia 1050 GTX graphic card.

Tried it on my wife's desktop with AMD Rizen 5 and Vega graphic cards and got some troubles with the Shaders' code (sounds like the GLSL compilers aren't 100% compatible).
I fixed the problems on the AMD graphic but did the test just on two systems and I don't know how solid the build is.
Never tried on a system with Intel graphics.

The Fractal Manual Word file is a preliminary attempt to document the user interface.
I'm providing guidelines for just a couple of GUI windows, barely enough to startplaying and try to explore some fractal.
The navigation occurs through the mouse, clicking and dragging (see the manual).
It isn't necessarily intuitive but is thought to move across the fractal just using mouse, mouse buttons and mouse wheel.
I'll spend time on the other GUI windows only if I get any feedback telling me that somebody looked into it and would like more info.

Warning:
The shader may run for more than 2 seconds if the accuracy is pushed too far or some inconsistencies in parameter set force it into an endless loop.
When that happens Windows throws in an exception and the program crashes.
It doesn’t happen on my system since I fixed most bugs affecting the problem, but on system with a slow graphic card it may still happen. It doesn’t harm the system, just need to stop and restart the program.
