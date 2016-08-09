# OpenConnect-GUI

This is the GUI client for openconnect VPN.

This client is in beta testing phase. It cannot be assumed to provide
the required security.

## Development info

### Software requirements
#### Windows 10
* Qt 5.6.x (incl. mingw32) & QtCreator 4+
* NSIS 3.0+
* CMake 3.6+

Newer or older versions may work... feel free tweak main CMakeLists.txt as you need.

#### Fedora 24
TODO...

### Webpage maintenance
1. images/screenshots preaparation:
	- prepare desired screen shots (common width is 650px)
	- checkout project into 'gh-pages' branch
	- create or upload images to e.g. screenshots folder
	- commit & submit - and continue with web-text changes...
2. web page text:
	- open GitHub project settings
	- look for GitHub pages and click "Launch automatic page generator"
	- edit 'markdown' page content
	- click "Continue to layouts"
	- choose 'Tactile' style in list
	- review your changes and finish with clicking to "Publish page"
