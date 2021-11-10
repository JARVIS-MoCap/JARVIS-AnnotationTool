mkdir ../build
cd ../build
cmake ..
make -j12
make install
cd ../deployment
dpkg-deb --build --root-owner-group deb_packages/JARVIS-AnnotationTool_0.3-1_amd64_2004
mv deb_packages/JARVIS-AnnotationTool_0.3-1_amd64_2004.deb .
