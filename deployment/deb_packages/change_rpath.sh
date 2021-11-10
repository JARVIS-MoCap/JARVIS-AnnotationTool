#!/bin/bash
for filename in OpenCV/*; do
    patchelf --set-rpath '/usr/local/lib/JARVIS-AnnotationTool/OpenCV' $filename
done
