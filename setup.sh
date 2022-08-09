#!/usr/bin/env bash


unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}

cd libs/OpenCV
mkdir build
cd build

if [ "${machine}" = "Linux" ];
then
  cmake -DOPENCV_ENABLE_ALLOCATOR_STATS=OFF -DCMAKE_BUILD_TYPE=RELEASE \
        -DBUILD_TIFF=OFF -DWITH_TIFF=OFF -DBUILD_JPEG=OFF -DBUILD_ZLIB=OFF \
        -DBUILD_WEBP=OFF -DBUILD_PNG=OFF -DWITH_OPENEXR=OFF -DWITH_OPENJPEG=OFF \
        -DWITH_JASPER=OFF -DWITH_PROTOBUF=OFF -DWITH_QUIRC=OFF -DWITH_1394=OFF \
        -DWITH_V4L=OFF  -DWITH_GSTREAMER=ON -DWITH_FFMPEG=ON -DWITH_GTK=OFF \
        -DBUILD_SHARED_LIBS=OFF \
        -DBUILD_LIST="core,calib3d,imgproc,videoio,aruco, gapi" \
        -DCMAKE_INSTALL_PREFIX=../opencv_install \
        -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv
  cmake --build . --parallel 8
  cmake --install .
fi

cd ../../

cd Qt5/qt5
pwd
./init-repository -f --module-subset=qtbase,qtsvg,qtmultimedia,qttools,qttranslations,qtdoc,qtrepotools,qtqa,qtwayland,qt3d,qtimageformats,qtserialbus,qtserialport,qtcharts,qtdatavis3d,qtshadertools
cd ..
mkdir build
cd build

if [ "${machine}" = "Linux" ];
then
  ../qt5/configure -static -release -no-pch -prefix ../qt_install -opensource \
        -confirm-license -nomake tools -nomake tests -nomake examples \
        -skip webengine -skip qtandroidextras -skip qtconnectivity  \
        -skip qtdeclarative -skip qtdoc -skip qtgamepad -skip qtgraphicaleffects \
        -skip qtlocation -skip qtmacextras -skip qtnetworkauth -skip qtpurchasing \
        -skip qtquickcontrols -skip qtquickcontrols2 -skip qtscript -skip qtscxml \
        -skip qtsensors -skip qtserialbus -skip qtspeech -skip qttools \
        -skip qtsensors -skip qttranslations -skip qtvirtualkeyboard \
        -skip qtwebchannel -skip qtwebengine -skip qtwebsockets \
        -skip qtwebview -skip qtwinextras -skip qtxmlpatterns -skip qtwebglplugin \
        -no-openssl -skip qtlottie -skip qtmqtt -skip qtopcua -skip qtquicktimeline \
        -skip qtquick3d -skip qtquick3dphysics -skip qtwayland

  cmake --build . --parallel 8
  cmake --install .
fi

cd ../../../

# mkdir build
# cd build

# cmake ..
# cmake --build . --parallel
