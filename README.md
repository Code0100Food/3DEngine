# Fiesta Engine
A 3D engine developed by two students of videogames development.
This code is under the MIT License

- Libraries Used:
   MathGeoLib -> http://clb.demon.fi/MathGeoLib/nightly/
   ImGui -> https://github.com/ocornut/imgui
   Bullet -> http://bulletphysics.org
   Sdl -> http://www.libsdl.org/
   Glew -> https://github.com/nigels-com/glew
   cURL -> https://curl.haxx.se/

Bullet and sdl are suggested to zlib license. -> http://www.zlib.net/zlib_license.html
ImGui is suggested to MIT License
MathGeoLib is suggested to Apache 2 License. -> http://www.apache.org/licenses/LICENSE-2.0.html
All those Libraries are not created by us.

# Instructions

To start the engine, execute the Party Engine.exe, if you execute the Launcher.exe it will only execute the launcher not the engine. 

The engine executable calls the launcher executable and then starts the engine.

## Launcher

When you start the program the launcher will be executed. 
If you dont have the last engine version a download button and a message of the new version will appear on the top of the window.
The download button links the new release page where you can download it.

Then you can pulse start and open the engine or close and end the execution.

At the bottom of the window there are links to our github pages where you can see our projects.

## Engine

The engine UI starts showing you the next elements:

- Hierarchy: Show the scene game objects hierarchy
- Scene: Edit camera point of view
- Game: Game camera point of view
- Inspector: Show the selected game object components
- Resource Inspector: Show the resources related with the selected file
- Files: Show the files inside the selected folder
- Directories: Show the folders inside assets
- Play/Pause/Next buttons: Used to start/pause or loop one frame in the game mode. When the game mode starts game time is shown and you can change the time scale.

You can acceed to all the UI elements using the top bar menu that have five sub menus.

- File:
		- Save: Serialize the current scene
		- Load: Load the serialized scene
		- Clean: Delete all the objects in the scene
		- Exit: Close the engine

- Game Objects:
		- Empty: Create a empty object
		- Cube,Sphere…: Create a primitive

- View:
		- Hierarchy: Show you the states of the objects loaded in the scene.
		- Configuration: Show all the modules configuration and let you modify it.
- Tools:
		- Profiler: Show a simple time tracking of all the modules. You can modify the times to define when a module is exceeding his process time.
		- UI Config: You can choose one of the two defined UI themes or edit your own.
- Help:
		- GitHub Repository: A link to the github repository of the engine.
		- Download last: Links to the last github release page.
		- Report a bug: Links to the issues page of the github repository.
		- About: Show basic information about the engine (Libraries, license, authors,...)

Camera movement:
- Right click turns on “WASD” fps-like movement
- Mouse wheel zoom in and out
- Mouse wheel pressed moves camera up, down, left and right
- Alt+Left click orbit the object
- Pressing “f” focus the camera around the geometry

Console:

Console is initially closed.

To open/close the console pulse the grave key.
Console shows information of the engine operations and can handle some basic input.(help,quit,clear).

To see all the console commands, you can open the console header in the configuration window or enter “help” in the console

Resources Manager:

Resources manager generate all the necessary files and folders to manage the user files. You can drop the file or place it inside the assets folder to import it.

# Changelog
	 -Release 0.3:
		Resources Reimporation
		Resources automatically update
		GameObject hierarchy UI improved
		Play/Pause/Next UI output
		Gizmos working(alpha version)
		Debug mode with F1

	 -Release 0.2.7:
		Scene Save/Load with resources
		Scene Resource added

	 -Release 0.2.6:
		Adaptative Octree
		Octree subdivision optimized
		Gizmos UI working
		Gizmos Input(in process)
		Resource Manager Meta generation
		Resource Manager Mesh and Material Resources
		Resource Manager Assets folder load on start

	 -Release 0.2.5:
		Octree multiple subdivision works
		Octree store static objects
		Scene Serialization(Load)
		Play/Pause/Next Buttons 
		Customizable Docking Improved
		Docking Save/Load methods
		Mouse picking
		Edit camera frustum
		Lines antialiasing
		UI always adapts at window size

	 -Release 0.2.3:
		Octree subdivision polish
		Scene Serialization (Save)
		Bounding Boxes Transformation improved

	 -Release 0.2.1:
		GameObjects added
		Components added (material,mesh,transform,renderer)
		File system added
		Importers (materials and meshes)
		Data imported using own format files (.fiesta)

	 -Release 0.2:
		Files can be loaded from any folder
		UI polished to show models stats
		UI audio effects updated
		Windows are resized on window resize
		Load cancelled in bugged fbx
            Camera adapt distance deppending on fbx size 

	 -Release 0.1.9.9:
		Camera input polished
		Texture drag implemented
		Geometry Inspector added 

	 -Release 0.1.9.8:
		Camera movement improved
		Load Textures
		Blit Textures
		Docking improved
		Viewport using docking implemented

 	 -Release 0.1.9.5:
		Render meshes
		Process meshes
		Load meshes

 	 -Release 0.1.9:
		Geometry Manager
		ImGui Docking
		ImGui Themes

	 -Release 0.1.8:
		Render Effects
		Print simple geometry full and wireframe
		Added glew libraries
		Added Input manager
		Added more Drivers and Hardware information
		Added Input info (mouse and keyboard)
		Added Profiler to look timing
		Bugs fixed

	-Release 0.1.5:
	 	Simple Launcher
	 	Config screen able to modify values of the application
	 	Config screen displays fps, memory usage, hardware settings...
	 	Added Main menu bar and moved random generator and intersections to it
	 	Link to the github repository
	 	Link to the last release
	 	Link to report bugs
	 	About screen with some information of us
	 	Added Sounds to Menus
	 	Camera 3D enabled
	 	Load and Save files with json
	 	Implemented mmgr to find memory leaks

	-Release 0.1:
		Simple UI for testing a random number generator
		Simple UI for testing intersections between some geometrical figures
		Console ready to use, still does nothing


# Owners
Eric Solà
Ferran Martín
