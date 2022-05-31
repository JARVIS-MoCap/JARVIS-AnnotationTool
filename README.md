# JARVIS AnnotationTool

<p align="center">
<img src="IconThemes/DarkIconTheme/Banner.png" alt="banner" width="70%"/>
</p>

This is the official Github Repository for the **JARVIS Annotation Tool**. To find out more about our 3D markerless motion capture toolbox have a look at 
**[our website](https://jarvis-mocap.github.io/jarvis-docs/)**.

All you need to get started are synchronized multi-camera recordings (check out our [AcquisitionTool](https://github.com/JARVIS-MoCap/JARVIS-AcquisitionTool)) and calibration recordings using a simple checkerboard or ChArUco-board. 
The AnnotationTool has functionallity to **extract representative frames** from your recordings in a semi-supervised fashion and it can be used to **calibrate your cameras**.\
It then uses live updating reprojection-error statistics to make the process of **creating 3D keypoint annotations** as intuitive and precise as possible. If you have real world measurements of the animal or object you're annotating (e.g. the length of all finger segments) you can also use those metrics to guide you during the annotation process.

**Installing our prebuild packages is easy!** Just go to **[our downloads page](https://jarvis-mocap.github.io/jarvis-docs//2021-10-29-downloads.html)** and grab the installer for your operating system. We currently support Windows, MacOS and Ubuntu 20.04/18.04. Installers for the current and previous versions can also be found under [Releases](https://github.com/JARVIS-MoCap/JARVIS-AnnotationTool/releases).

If you want to build the tool yourself here's a step by step guide on how to do it. If the prebuild OpenCV, Qt and yaml-cpp libraries we ship it with don't work for you there's a guide on how to [build the dependencies](#building-opencv-qt5-and-yaml-cpp-yourself) at the bottom of this page. (Please note that the guide is currently not verified, please let us know if you run into any issues!)

<p align="center">
<img src="docs/Annotation_Tool_Vid.gif" alt="banner" width="75%"/>
</p>

# Building from Source

## Linux

### Installing the dependencies
To be able to build the tool install all the dependencies with

      sudo apt get install build-essential libxcb-xinerama0 libdouble-conversion-dev gstreamer1.0-libav ffmpeg libxcb-xinput0 libpcre2-dev libeigen3-dev qt5-default libqt5charts5-dev libqt5serialport5-dev qtmultimedia5-dev
      
### Cloning the repository
Next clone our repository with 

     git clone --recursive https://github.com/JARVIS-MoCap/JARVIS-AnnotationTool.git
     
Go into the repository and create a build directory

    cd JARVIS-AnnotationTool && mkdir build && cd build
    
### Building and installing 
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

## MacOS
Coming soon...

## Windows
coming soon...


# Building OpenCV yourself
We try to provide prebuild versions of all the libraries you will need to compile the tool. You only need to build them yourself incase they don't work for your OS or you want to use different versions than the ones we ship it with.

## Linux and MacOS
- On MacOS: build [libjpeg turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) from git (make sure to build Version 8!)
- Get opencv and opencv_contrib from git with: 
  
      git clone https://github.com/opencv/opencv.git
      git clone https://github.com/opencv/opencv_contrib.git
      
- Checkout desired version with: 
  
      git checkout 4.5.5
	
- Configure with (set DCMAKE_INSTALL_PREFIX to the path you want to install it to):

	    cmake -DOPENCV_ENABLE_ALLOCATOR_STATS=OFF -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_TIFF=OFF -DWITH_TIFF=OFF -DBUILD_JPEG=OFF -DBUILD_ZLIB=OFF -DBUILD_WEBP=OFF -DBUILD_PNG=OFF -DWITH_OPENEXR=OFF -DWITH_OPENJPEG=OFF -DWITH_JASPER=OFF -DWITH_PROTOBUF=OFF -DWITH_QUIRC=OFF -DWITH_1394=OFF -DWITH_V4L=OFF  -DWITH_GSTREAMER=ON -DWITH_FFMPEG=ON -DWITH_GTK=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_LIST="core,calib3d,imgproc,videoio,aruco, gapi" -DCMAKE_INSTALL_PREFIX=../opencv_static -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv

 - Build and install OpenCV to the previously selected directory with:
 
       make -j8 && make install
       
 - You can then either change the OpenCV_DIR paths to point to the directory you installed to or copy the files in the install directory to 

        ${AnnotationTool_Directory}/libs/OpenCV_4.5.1/Ubuntu_xx04(MacOS)
  
  
## Windows
 Coming soon...


# Building QT5 yourself (only neccessary on MacOS):
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
 
        ${AnnotationTool_Directory}/libs/Qt_5.15.2/Ubuntu_xx04(MacOS)
      
    
