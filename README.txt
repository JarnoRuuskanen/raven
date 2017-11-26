UPDATE: I have since switched back to a private repository and
will most likely not continue pushing new stuff into this public 
repository.


HOW TO RUN VULKAN VALIDATION FOR THE PROGRAM
(Information copied from VulkanCookbook)

On the Windows operating system family:
1. Go to the folder in which the SDK was installed and then open the Config subdirectory.
2. Copy the vk_layer_settings.txt file into the directory of the executable you
want to debug (into a folder of an application you want to execute).
3. Create an environment variable named VK_INSTANCE_LAYERS:
1. Open the command-line console (Command Prompt/cmd.exe).
2. Type the following:
 setx VK_INSTANCE_LAYERS
 VK_LAYER_LUNARG_standard_validation
 3. Close the console.
4. Re-open the command prompt once again.
5. Change the current directory to the folder of the application you want to execute.
6. Run the application; potential warnings or errors will be displayed in the
standard output of the command prompt.

On the Linux operating system family:
1. Go to the folder in which the SDK was installed and then open the Config subdirectory.
2. Copy the vk_layer_settings.txt file into the directory of the executable you
want to debug (into a folder of an application you want to execute).
3. Create an environment variable named VK_INSTANCE_LAYERS:
1. Open the Terminal window.
2. Type the following:
 export
 VK_INSTANCE_LAYERS=VK_LAYER_LUNARG_standard_validation
4. Run the application; potential warnings or errors will be displayed in the
standard output of the Terminal window.