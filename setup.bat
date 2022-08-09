@echo off

cd libs/OpenCV
mkdir build
cd build

cmake -DOPENCV_ENABLE_ALLOCATOR_STATS=OFF -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_TIFF=OFF -DWITH_TIFF=OFF -DBUILD_JPEG=OFF -DBUILD_ZLIB=OFF -DBUILD_WEBP=OFF -DBUILD_PNG=OFF -DWITH_OPENEXR=OFF -DWITH_OPENJPEG=OFF -DWITH_JASPER=OFF -DWITH_PROTOBUF=OFF -DWITH_QUIRC=OFF -DWITH_1394=OFF -DWITH_V4L=OFF  -DWITH_GSTREAMER=ON -DWITH_FFMPEG=ON -DWITH_GTK=OFF -DBUILD_SHARED_LIBS=ON -DBUILD_LIST="core,calib3d,imgproc,videoio,aruco, gapi" -DCMAKE_INSTALL_PREFIX=../opencv_install -DOPENCV_EXTRA_MODULES_PATH="../opencv_contrib/modules" ../opencv -G "Ninja"

cmake --build . --parallel
cmake --install .
cd ../../

cd Qt5/qt5
perl init-repository -f --module-subset=qtbase,qtsvg,qtmultimedia,qttools,qttranslations,qtdoc,qtrepotools,qtqa,qt3d,qtimageformats,qtserialbus,qtserialport,qtcharts,qtdatavis3d,qtshadertools
cd ..
mkdir build
cd build
call "../qt5/configure.bat" -static -release -no-pch -prefix ../qt_install -opensource -confirm-license -nomake tools -nomake tests -nomake examples -skip webengine -skip qtandroidextras -skip qtconnectivity  -skip qtdeclarative -skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtlocation -skip qtmacextras -skip qtnetworkauth -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtspeech -skip qttools -skip qtsensors -skip qttranslations -skip qtvirtualkeyboard -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtxmlpatterns -skip qtwebglplugin -no-openssl -skip qtlottie -skip qtmqtt -skip qtopcua -skip qtquicktimeline -skip qtquick3d -skip qtquick3dphysics -opengl desktop -cmake-generator "Ninja"
cmake --build . --parallel
cmake --install .
