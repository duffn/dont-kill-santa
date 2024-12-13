#!/bin/bash

# Change the current directory to the script's directory
cd "$(dirname "$0")" || exit

# Clean the index.html file first since we manually created it for uploading to a site
rm -f ../build-web/index.html
mkdir -p ../build-web

# Copy the contents of the 'assets' directory to 'build-web/assets/'
cp -R ../assets/ ../build-web/assets/

# Change the current directory to 'build-web'
cd ../build-web || exit

if [ "$1" == "async" ]; then
	echo "using asyncify"
	emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-s USE_GLFW=3" -DCMAKE_EXECUTABLE_SUFFIX=".html" -DASYNCIFY=1
elif [ "$1" == "debug" ]; then
	emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXE_LINKER_FLAGS="-s USE_GLFW=3" -DCMAKE_EXECUTABLE_SUFFIX=".html"
elif [ -n "$1" ]; then
	echo "Unknown argument: $1"
	exit 1
else
	emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-s USE_GLFW=3" -DCMAKE_EXECUTABLE_SUFFIX=".html"
fi

# Build the project using Emscripten
emmake make

mv ../build-web/dontkillsanta.html ../build-web/index.html
