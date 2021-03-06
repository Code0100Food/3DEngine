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
   Mono -> http://www.mono-project.com/download/

Bullet and sdl are suggested to zlib license. -> http://www.zlib.net/zlib_license.html
ImGui is suggested to MIT License
MathGeoLib is suggested to Apache 2 License. -> http://www.apache.org/licenses/LICENSE-2.0.html
All those Libraries are not created by us.

# Instructions

To start the engine, execute the Fiesta Engine.exe, if you execute the Launcher.exe it will only execute the launcher not the engine. 

The engine executable calls the launcher executable and then starts the engine.

## Launcher

When you start the program, the launcher will be executed. 
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
		- Cube,Sphere�: Create a primitive

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

### Camera movement:
- Right click turns on �WASD� fps-like movement
- Mouse wheel zoom in and out
- Mouse wheel pressed moves camera up, down, left and right
- Alt+Left click orbit the object
- Pressing �f� focus the camera around the geometry

### Console:

Console is initially closed.

To open/close the console pulse the grave key.
Console shows information of the engine operations and can handle some basic input.(help,quit,clear).

To see all the console commands, you can open the console header in the configuration window or enter �help� in the console

Console Will display the Compile Errors in red with it's line and the error it self.
Also, if the Script compiles it will show Compile Suceed

### Resources Manager:

Resources manager generate all the necessary files and folders to manage the user files. When a file is imported the appropriated files are generated:

- Material: .meta & .dds
- Mesh: .meta & .fiesta
- Scene: .meta & .scn 

To import a file drop it to the engine window or place it on assets before the start of the engine.

If a file is removed from assets the imported resources aren�t deleted from the library so you can already use it in the engine.

When you edit a file the resource manager updates the related resources and modify them.

If a resource is not used in the scene it�s released from the memory till some gameobject use it.

### Launcher:

When openning the Engine, before the main loop, the launcher will be executed and will read the file updates.json from Github.
If your update is different from the online update file, a Download button will appear and will send you to the Project Latest Release.
If you click start the application will run with the current version.

### Gizmos

When you click an object the gizmo will appear. To change between gizmos press Q,W,E,R 
If you are focus in an object [pressing f], the gizmo works perfectly but if the object is far away from the center
it will have a little gap between the gizmo and the mouse position.
The Global/Local buttons on top of the screen will modify how the gizmo afects to the game object

### Hierarchy Window

When Right click in the hierarchy window or a GameObject, a menu will appear and will show some options.

### Scene

You can place two or more cameras in the scene. If you go to the inspector and select Main Camera in the Camera Component
this will be the Camera that displays the Game view. You can also separate the Scene and Game view and move the 
cameras to see the results in real time.

We have placed a tank into the scene, this has 3 scripts, one to move and two to rotate. The controls are:

 -"W" to move forward.
 -"S" to move backwards.
 -"A" & "D" to rotate the tank.
 -Mouse to rotate the Turret of the tank. 

"W,A,S,D" are fully reprogramable you just have to change the FiestaInput.FiestaKeyCode."W" in the script. Not all keys are binded so check the DATA/Scripting/FiestaEngineEnviroment.txt to be sure not to use a wrong key.

### Camera

To modify Camera FOV or other settings go to "View" -> "Configuration" -> "Camera"

### Octree

To calculate the octree go to �View� -> �Configuration� -> �Scene� -> Recalculate

Octree will be filled with static objects. To set a static objects check the inspector �Static� checkbox. The childs of a static object become static.



### Debug mode

When Pressing F1 debug mode is active and you will see the mouse picking ray and the editor camera frustrum in the Game View


### Scripting System

Scripting System uses mono to compile and link C# scripts with the engine environment. 

#### Scripting Resources:
All the scripts placed in the assets folder or dropped on the engine window are imported. To import a script the resource manager generates a script resource and call the module scripting to compile it and get all the necessary data, like the fields and the compilation result. The compilation result is a DLL that is saved in the library, so on the next engine start the already imported scripts don�t need to be compiled, only linked with the correct script resources with the meta files.
When the script is imported you can see it in the resource inspector and edit it with the scripting console pressing the �Edit� button. When you edit a script and save, the script will be reimported so all the related data will be updated if the compilation is successfully completed.

#### Script Editor:
As we mention before there�s a scripts editor that let you modify and create new scripts during the engine execution. To open the scripts editor, you can press F2 or �Edit� button on the resources inspector when a script resource is selected.
When you create a new script from the editor a .txt will be placed on the Assets folder and the script will be imported generating the related files.

#### Scripts Inspector:
All the scripts imported or created can be used in the scene. To attach a script to a game object check the inspector and press de button �Add Component�. In the add component menu you can try to add any available script or create a new one.
If you select �New Script� the script editor opens and you can start writing the new script. If you select an existent script a ComponentScript will be added to the current GameObject with the correct ScriptResource. 
When the script is added the fields values can be changed and that values will be the initial stats of the ComponentScript when the game starts. The initial value will be set again when the game mode ends.

#### MonoEmbedding DLL
We have created a .dll which is the responsible to compile and execute the scripts in runtime.
The .dll have a bunch of methods to use the Mono functionality such as load assemblies, invoke methods, fill c# fields, attach c++ methods to c# methods and more.

# Changelog

	 -Release 0.5:
		Scripting Console Updated
		Script Resource track fields
		ScriptComponent Added
		Engine environment for scripting defined

	 -Release 0.3.1:
		Scripting Console Added
		Scrip Resource Added
		Scripts Manager Added
		Mono Scripting Added
		Mono Scripting linked with resources
	
	 -Release 0.3:
		Resources Reimporation
		Resources automatically update
		GameObject hierarchy UI improved
		Play/Pause/Next UI output
		Gizmos working(alpha version)
		Debug mode with F1
		Huge Bug Fixing
		

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
Eric Sol�
Ferran Mart�n

Github Repository: 	https://github.com/Code0100Food/3DEngine
Assigment 3 Release:	https://github.com/Code0100Food/3DEngine/releases/tag/Assignment3	
Web Link: https://code0100food.github.io/3DEngine/
