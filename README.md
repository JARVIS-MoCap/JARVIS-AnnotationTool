This is the official Github Repository for the JARVIS Annotation Tool. To find out more about our 3D markerless motion capture toolbox have a look at 
[our website](https://jarvis-mocap.github.io/jarvis-docs/).

Installing our prebuild packages is easy! Just go to [our downloads page](https://jarvis-mocap.github.io/jarvis-docs/) and grab the installer for your operating system. We currently support Windows, MacOS and Debian based Linux distributions. Installers for the current and previous versions can also be found under [Releases](https://github.com/JARVIS-MoCap/JARVIS-AnnotationTool/releases).

If you want to build the tool yourself here's a step by step guide on how to do it. If the prebuild OpenCV, Qt and yaml-cpp libraries we ship it with don't work for you there's a guide on how to [build the dependencies](#building-opencv-qt5-and-yaml-cpp-yourself) at the bottom of this page.

# Linux 

### Installing the dependencies
To be able to build the tool install all the dependencies with

      sudo apt get install libxcb-xinerama0, libdouble-conversion-dev, gstreamer1.0-libav, ffmpeg, libxcb-xinput0, libpcre2-dev, build-essential
      
### Cloning the repository
Next clone our repository with 

     git clone --recursive https://github.com/JARVIS-MoCap/JARVIS-AnnotationTool.git
     
Go into the repository and create a build directory

    cd JARVIS-AnnotationTool && mkdir build && cd build
    
Run cmake (replace XX04 by either 2004 or 1804) depending on your Ubuntu Version.

	cmake -DUBUNTU_VERSION=xx04 ..
	
Run make to build the tool (replace x by the number of available cores on your CPU)

     make -jx
     
If you want to create a debian package go to the deployment folder and run (replace XX04 by your Ubuntu Version)

     sh deploy_Ubuntu_XX04.sh

And finally install with (replacing the Xs with the numbers in the package you created)

     sudo apt install ./JARVIS-AnnotationTool_X.X-X_amd64_XX04.deb
     
If you want to remove it run

     sudo dpkg -r AnnotationTool

# Building OpenCV QT5 and yaml-cpp yourself
We try to provide prebuild versions of all the libraries you will need to compile the tool. You only need to build them yourself incase they don't work for your OS or you want to use different versions than the ones we ship it with.

## Linux

#### Build QT5:
- Get QT from git with: 

      git clone git://code.qt.io/qt/qt5.git
      
- Checkout desired version with: 

      git checkout 5.15.2

- Inside qt5 run 

      ./init-repository
      
- Create build directory and go into it with:

      mkdir build && cd build

- Configure with (set prefix to the path you want to install it to):

      ../qt5/configure -release -no-pch -prefix <PATH TO INSTALL TO> -opensource -confirm-license -nomake tools -nomake tests
      -nomake examples -no-opengl -skip webengine -skip qt3d -skip qtandroidextras -skip qtcanvas3d -skip qtconnectivity -skip qtdatavis3d 
      -skip qtdeclarative -skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtlocation -skip qtmacextras -skip qtnetworkauth 
      -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus 
      -skip qtquick3d -skip qtspeech -skip qttools -skip qtsensors -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel 
      -skip qtwebengine -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtxmlpatterns  -skip qtwebglplugin -no-openssl -no-vulkan
      
 - Build and install Qt to the previously selected directory with:
 
       make -j8 && make install
       
 - You can then either change the QT_DIR paths to point to the directory you installed to or copy the files in the install directory to 
 
        ${AnnotationTool_Directory}/libs/Qt_5.15.2/Ubuntu_xx04
 

#### Building OpenCV:
- Get opencv from git with: 
  
      git clone https://github.com/opencv/opencv.git
      
- Checkout desired version with: 
  
      git checkout 4.5.1 (DONT use 4.5.3, somehow it doesn't build libade.a)
	
- Configure with (set DCMAKE_INSTALL_PREFIX to the path you want to install it to):

	    cmake -DOPENCV_ENABLE_ALLOCATOR_STATS=OFF -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_TIFF=OFF -DWITH_TIFF=OFF -DBUILD_JPEG=OFF 
      -DBUILD_ZLIB=OFF -DBUILD_WEBP=OFF -DBUILD_PNG=OFF -DWITH_OPENEXR=OFF -DWITH_OPENJPEG=OFF -DWITH_JASPER=OFF -DWITH_PROTOBUF=OFF 
      -DWITH_QUIRC=OFF -DWITH_1394=OFF -DWITH_V4L=OFF  -DWITH_GSTREAMER=ON -DWITH_FFMPEG=ON -DWITH_GTK=OFF -DBUILD_SHARED_LIBS=ON 
      -DBUILD_LIST="core,calib3d,imgproc,videoio,aruco" -DCMAKE_INSTALL_PREFIX=<PATH TO INSTALL TO> 
      -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-master/modules ../opencv-master

 - Build and install OpenCV to the previously selected directory with:
 
       make -j8 && make install
       
 - You can then either change the OpenCV_DIR paths to point to the directory you installed to or copy the files in the install directory to 

        ${AnnotationTool_Directory}/libs/OpenCV_4.5.1/Ubuntu_xx04
  
  
#### Building yaml-cpp
 - Get yaml-cpp from git with:
 
        git clone https://github.com/jbeder/yaml-cpp.git
        
 - Inside the yaml-cpp directory create a build directory with:
 
       mkdir build && cd build
  
 - Run Cmake (set DCMAKE_INSTALL_PREFIX to the path you want to install it to):
 
       cmake .. -DCMAKE_INSTALL_PREFIX=<PATH TO INSTALL To>
      
 - Make and install with:
 
       make -j8 && make install
       
 
 ## Windows
 Coming soon...
 
 ## MacOS
 Coming soon...
