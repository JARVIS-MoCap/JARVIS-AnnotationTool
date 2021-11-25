#!/bin/bash
for filename in lib/*; do
    patchelf --set-rpath '../libs/OpenCV_4.5.1/Ubuntu_2004/lib' $filename
done
