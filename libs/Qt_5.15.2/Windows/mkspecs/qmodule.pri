QT_CPU_FEATURES.x86_64 = sse sse2
QT.global_private.enabled_features = sse2 alloca_malloc_h alloca avx2 dbus gui network sql testlib widgets xml
QT.global_private.disabled_features = alloca_h android-style-assets private_tests dbus-linked dlopen gc_binaries intelcet libudev posix_fallocate reduce_exports reduce_relocations release_tools relocatable stack-protector-strong system-zlib zstd
QT_COORD_TYPE = double
CONFIG -= precompile_header
CONFIG += sse2 aesni sse3 ssse3 sse4_1 sse4_2 avx avx2 compile_examples f16c largefile optimize_size rdrnd rdseed shani x86SimdAlways
QT_BUILD_PARTS += libs
QT_HOST_CFLAGS_DBUS += 
