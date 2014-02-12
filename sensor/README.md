Sensors
=======

PrimeSense:

	1. Download the NiTE and OpenNI versions for your OS/Architecture
	2. Extract them and move them where you want them to live "permanently"
	3. Open a terminal and run 'sudo ./install.sh' in both of their root folders
	4. Each install script creates a file called OpenNI/NiTEDevEnvironment in the same directory as the script,
	   open them and paste their contents into your ~/.bashrc file
	5. Paste the following line after the newly added OpenNI/NiTE environment variables:
	   'export LD_LIBRARY_PATH=$OPENNI2_REDIST:$NITE2_REDIST:$LD_LIBRARY_PATH'
	6. Save the file and open a new ternimal session or relaunch bash
	8. Copy the NiTE2 folder from the NiTE-X-X-X.X/Samples/Bin directory to the directory with the binary you are trying to execute
	   (Note) - You must be in the same directory as the executable in order to launch it
	7. The supplied make files should build successfully