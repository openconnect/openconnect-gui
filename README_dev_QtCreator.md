### Development with QtCreator
- checkout project from [GitHub](https://github.com/openconnect/openconnect-gui)
- update project submodules
- optionally initialize [git-flow](http://danielkummer.github.io/git-flow-cheatsheet/) developmenet scheme if you like
- and switch to 'develop' branch
- start QtCreator
- create/select a session if make sense
- open CMakeLists.txt from project root
- update desired Qt 5.6 version build types and click "Configure"
- open 'Project' tab on left side of QtCreator with CMake configuration
- change
	- 'PROJ\_UAC\_ON' to 'off' because QtCreator is not able to start app with UAC (?) :/
	- if you like; change 'PROJ\_INI\_SETTINGS' to 'off' if you want store all settings into file instead of Registry for development/tests
- click 'Apply Configuration Changes' and then switch again to 'Edit' tab on left side of QtCreator
- build the project

Optionally setup MAKEFLAGS in Projects settings if you like for faster build.

