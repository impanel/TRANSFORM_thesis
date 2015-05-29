#TRANSFORM_thesis

#What does the program do

This is a stripped down version of the Milan code (TRANSFORM_app) for faster prototyping. It includes a gui for addressing and changin individual pins and has implemented TCP capability to be used with the EasyPin tool.

#How to use the program

TODO

#How to run the program

##Make sure you are using openFrameworks 0.8.0

You can download OF 0.8.0 here
https://github.com/openframeworks/openFrameworks/tree/0.8.0


##Make sure the CoreOF.xconfig file is updated

open /Users/bimster/Sites/OF/of_v0.8.0_osx_release/libs/openFrameworksCompiled/project/osx/CoreOF.xcconfig
add "$(LIB_FREEIMAGE)" and "$(LIB_FREETYPE)" to the OF_CORE_LIBS

go to /Users/bimster/Sites/OF/of_v0.8.0_osx_release/libs/openFrameworksCompiled/lib/osx/ and delete everything in that folder


##Make sure you clone the app into the proper folder structure

The content of inFORM_thesis should be inside of_v0.8.0_osx_release/apps/INFORM/inFORM_thesis/

##Make sure all dependencies are linked correctly

In Xcode in the left column expand the addons folder and check the inside of the addons folder if the text is in red. This means that either the addon does not exist in your openFrameworks/addons folder or it is not correctly linked.

This video tutorial explains how to include addons in your OF application
https://vimeo.com/34092591


##Make sure the ofMovieExporter libaries are linked correctly

In Xcode go into the project's target build settings. Scroll to "Search Paths" and open "Library Search Paths". Change the line "/Users/bimster/Sites/OF/of_v0.8.0_osx_release/addons/ofxMovieExporter/libs/libav/lib/osx" to point to the  library on your system.

##Make sure you clean the project before you compile

In Xcode go to Product -> Clean


=> the application should now compile and run succesfully