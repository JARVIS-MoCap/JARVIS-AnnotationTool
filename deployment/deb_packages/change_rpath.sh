#!/bin/bash
for filename in OpenCV/*; do
    patchelf --set-rpath 'AnnotationTool_0.3-1_amd64_2004/usr/local/lib/AnnotationTool/OpenCV' $filename
done
