meta:
	ADDON_NAME = ofxHIDAPI
	ADDON_DESCRIPTION = thin wapper of hidapi for openFrameworks
	ADDON_AUTHOR = 2bit
	ADDON_TAGS = "utility"
	ADDON_URL = http://github.com/2bbb/ofxHIDAPI

common:
	# dependencies with other addons, a list of them separated by spaces 
	# or use += in several lines
	ADDON_DEPENDENCIES = 
	
	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	ADDON_INCLUDES  = /opt/homebrew/opt/hidapi/include/hidapi
	
	# any special flag that should be passed to the compiler when using this
	# addon
	# ADDON_CFLAGS =
	
	# any special flag that should be passed to the linker when using this
	# addon, also used for system libraries with -lname
	ADDON_LDFLAGS  = -L/opt/homebrew/opt/hidapi/lib -lhidapi
