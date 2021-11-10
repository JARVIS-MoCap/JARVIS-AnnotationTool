# Building OpenCV, QT5 and yaml-cpp yourself

## Linux

### Build QT5:
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
 

### Building OpenCV:
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
  
  
### Building yaml-cpp
 - Get yaml-cpp from git with:
 
        git clone https://github.com/jbeder/yaml-cpp.git
        
 - Inside the yaml-cpp directory create a build directory with:
 
       mkdir build && cd build
 
 - Run Cmake (set DCMAKE_INSTALL_PREFIX to the path you want to install it to):
 
       cmake .. -DCMAKE_INSTALL_PREFIX=<PATH TO INstall To>
      
 - Make and install with:
 
       make -j8 && make install
