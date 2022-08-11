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

If you want to build the tool yourself here's a step by step guide on how to do it.

<p align="center">
<img src="docs/Annotation_Tool_Vid.gif" alt="banner" width="75%"/>
</p>

<br>

# Building from Source

## Linux

#### Installing the dependencies
On **Debian** based systems (e.g. Ubuntu and Mint) run the follwing command:

      sudo apt install cmake git build-essential libxcb-xinerama0 libdouble-conversion-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-gl ffmpeg libxcb-xinput0 libpcre2-dev libeigen3-dev libgl-dev zlib1g-dev libfontconfig-dev libjpeg-dev libharfbuzz-dev '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev  

**Important:** If you're using Ubuntu 18.04 or 20.04 updating CMake is required. See the [FAQ](https://github.com/JARVIS-MoCap/JARVIS-AnnotationTool#faq) section for instructions.<br>
<br>

On **Arch** based systems (e.g. Manjaro) run the following command (Currently there is a problem with building Qt6 on Arch based systems):

      sudo pacman -S base-devel git cmake double-conversion gst-libav gst-plugins-good gst-plugins-base ffmpeg eigen zlib libjpeg fontconfig harfbuzz
      
#### Cloning the repository
Next clone our repository with 

     git clone --recursive https://github.com/JARVIS-MoCap/JARVIS-AnnotationTool.git
     
     
Change to the repositories main directory

     cd JARVIS-AnnotationTool
     
#### Building and installing
Build Qt and OpenCV using the provided setup script by runnning

     sh setup.sh
     
Create and enter a build directory 

    mkdir build && cd build
    
Run cmake to configure and build the AnnotationTool

	cmake .. && cmake --build . --parallel 8
     
If you want to create a debian package go to the deployment folder and run (replace XX04 by your Ubuntu Version)

     sh deploy_Ubuntu_XX04.sh

And finally install with (replacing the Xs with the numbers in the package you created)

     sudo apt install ./JARVIS-AnnotationTool_X.X-X_amd64_XX04.deb
     
If you want to remove it run

     sudo dpkg -r AnnotationTool

## MacOS 
- Initialize the Xcode tools by running the following command in the terminal:

      xcode-select --install

- Install cmake (either using Homebrew or by downloading it from [here](https://cmake.org/download/)

#### Cloning the repository
Next clone our repository with 

     git clone --recursive https://github.com/JARVIS-MoCap/JARVIS-AnnotationTool.git
     
Change to the repositories main directory

     cd JARVIS-AnnotationTool
     
#### Building and installing
Build Qt and OpenCV using the provided setup script by runnning

     sh setup.sh
     
Create and enter a build directory 

    mkdir build && cd build
    
Run cmake to configure and build the AnnotationTool

	cmake .. && cmake --build . --parallel 8
     

## Windows
- Install a version of Visual Studio (tested on 2015 or newer). The latest versioon can be found [here](https://visualstudio.microsoft.com/)
- Install Git for Windows from [here](https://gitforwindows.org/)
- Install Strawberry Perl from [here](https://strawberryperl.com/)

#### Cloning the repository
Next clone our repository with 

     git clone --recursive https://github.com/JARVIS-MoCap/JARVIS-AnnotationTool.git
     
Change to the repositories main directory

     cd JARVIS-AnnotationTool

#### Building and installing 
Switch to a **x64** VS Developer Command Prompt and run the setup batch file:

    setup.bat

Create a build directory

    mkdir build && cd build

Then run cmake

    cmake -DCMAKE_BUILD_TYPE=RELEASE .. -G "Ninja" && cmake --build . --parallel 8 --config Release
    
To run the AnnotationTool.exe without inistalling it you need to copy all opencv dlls to the build directory!
    
	
We currently use the free version Advanced Installer to create our '.msi' installer files. This is not an optimal solution, so if you know how to build a better pipeline to build them please feel free to implement that!


# FAQ
### Qt does not compile throwing 'CMake 3.21 or higher is required.'
This will occur on Ubuntu 20.04 or earlier. To fix it install the latest cmake release with the following commands.
1. Remove the old cmake install

       sudo apt remove --purge --auto-remove cmake
     
2. Prepare install

       sudo apt update && sudo apt install -y software-properties-common lsb-release && sudo apt clean all
     
3. Get kitware's signing key

       wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null

4. Add repo to list of sources

       sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"

5. Install kitware-archive-keyring package:

       sudo apt update && sudo apt install kitware-archive-keyring && sudo rm /etc/apt/trusted.gpg.d/kitware.gpg
     
6. Add public key

       sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 6AF7F09730B3F0A4

7. Install cmake

       sudo apt update && sudo apt install cmake
       
