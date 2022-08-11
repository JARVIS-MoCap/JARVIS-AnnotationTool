mkdir ../build
cd ../build
cmake -DUBUNTU_VERSION=1804 -DCMAKE_BUILD_TYPE=RELEASE ..
cmake --build . --parallel 8
cmake --install .
cd ../deployment
dpkg-deb --build --root-owner-group deb_packages/JARVIS-AnnotationTool_1.2-1_amd64_1804
mv deb_packages/JARVIS-AnnotationTool_1.2-1_amd64_1804.deb .
