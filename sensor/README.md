Sensors
=======

PrimeSense:

	1. Download the NiTE and OpenNI versions for your OS/Architecture
	2. Extract them and move them where you want them to live "permanently"

	   (Note: as of this writing, there is a bug in OpenNI-X-x-x.x/Include/OniPlatform.h on line 40 that will prevent compilation for
	    32 bit linux users. Replace line 40 with '#elif (__linux__ && (__i386__ || __x86_64__))' to fix)

	3. Open a terminal and run 'sudo ./install.sh' in both of their root folders
	4. Each install script creates a file called OpenNI/NiTEDevEnvironment in the same directory as the script,
	   open them and paste their contents into your ~/.bashrc file
	5. Paste the following line after the newly added OpenNI/NiTE environment variables:
	   'export LD_LIBRARY_PATH=$OPENNI2_REDIST:$NITE2_REDIST:$LD_LIBRARY_PATH'
	6. Save the file and open a new ternimal session or relaunch bash
	8. Copy the NiTE2 folder from the NiTE-X-X-X.X/Samples/Bin directory to the directory with the binary you are trying to execute
	   (Note) - You must be in the same directory as the executable in order to launch it
	7. The supplied make files should build successfully

LeapMotion:
	1. Download current Leap Motion SDK
	2. Extract sdk to "permanent" location
	3. Paste the following lines into your .bashrc file:
		'export LEAP_INCLUDE=<leap /include folder path>'
		'export LEAP_REDIST=<path to leap /lib/x64 or /lib/x86 folder>'
	4. Copy the 'libLeap.so' file from the appropriate lib folder into the binary directory

