mkdir ../build
cd ../build
cmake -DUBUNTU_VERSION=2004 ..
make -j12
make install
cd ../deployment
dpkg-deb --build --root-owner-group deb_packages/JARVIS-AnnotationTool_1.1-1_amd64_2004
mv deb_packages/JARVIS-AnnotationTool_1.1-1_amd64_2004.deb .
