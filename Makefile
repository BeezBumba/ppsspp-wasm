.PHONY: config-wasm build-wasm

config-wasm:
	emcmake cmake -S . -B build-wasm -G Ninja   -DCMAKE_BUILD_TYPE=Release   -DUSING_GLES2=ON   -DUSING_EGL=OFF   -DVULKAN=OFF   -DUSE_NO_MMAP=ON   -DUSE_FFMPEG=OFF   -DUSE_DISCORD=OFF   -DUSE_MINIUPNPC=OFF   -DUSE_SYSTEM_LIBSDL2=OFF   -DUSE_SYSTEM_LIBPNG=OFF   -DUSE_SYSTEM_FREETYPE=OFF   -DUSE_SYSTEM_LIBZIP=OFF

build-wasm:
	cmake --build build-wasm -j
