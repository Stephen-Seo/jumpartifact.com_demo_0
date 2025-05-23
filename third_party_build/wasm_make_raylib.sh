#!/usr/bin/env bash

EMSDK_ENV_SCRIPT="${HOME}/git/emsdk/emsdk_env.sh"
OUTPUT_DIR="$(dirname "$0")/raylib_out"
CLONE_DIR="$(dirname "$0")/raylib_clone"
RAYLIB_GIT_URL="https://github.com/raysan5/raylib.git"
RAYLIB_GIT_TAG="5.5"

while getopts 'o:e:c:h' opt; do
    case $opt in
        'o')
            OUTPUT_DIR="$OPTARG";;
        'e')
            EMSDK_ENV_SCRIPT="$OPTARG";;
        'c')
            CLONE_DIR="$OPTARG";;
        'h')
            echo "-o <output_dir>"
            echo "-e <emsdk_env_sh>"
            echo "-c <clone_dir>"
            exit 0;;
    esac
done

# Validate args

if [[ -z "$OUTPUT_DIR" ]]; then
    echo 'ERROR: The OUTPUT_DIR variable is an empty string!'
    exit 1
elif ! [[ -d "$OUTPUT_DIR" ]]; then
    if ! mkdir -p "$OUTPUT_DIR"; then
        echo "ERROR: Failed to 'mkdir -p \"$OUTPUT_DIR\"!"
        exit 2
    fi
fi

pushd "$OUTPUT_DIR" >&/dev/null
OUTPUT_DIR="$(pwd)"
popd >&/dev/null

#echo "Using output dir: $OUTPUT_DIR"

if ! [[ -d "$CLONE_DIR" ]]; then
    mkdir -p "$CLONE_DIR"
fi

pushd "$CLONE_DIR" >&/dev/null
CLONE_DIR="$(pwd)"
popd >&/dev/null

#echo "Using clone dir: $CLONE_DIR"

if ! [[ -r "$EMSDK_ENV_SCRIPT" ]]; then
    echo "ERROR: \"$EMSDK_ENV_SCRIPT\" is not readable!"
    exit 3
fi

# Validate and set up Raylib git clone

if ! [[ -d "$CLONE_DIR" ]] || ! [[ -d "$CLONE_DIR/.git" ]]; then
    if ! git clone --depth 1 --no-single-branch "$RAYLIB_GIT_URL" "$CLONE_DIR"; then
        echo "ERROR: Failed to clone Raylib into \"$CLONE_DIR\"!"
        exit 4
    fi
fi

if ! (cd "$CLONE_DIR" && git fetch --all -p && git checkout "$RAYLIB_GIT_TAG"); then
    echo "ERROR: Failed to fetch/checkout in Raylib dir!"
    exit 5
fi

function cleanup() {
    cd "$CLONE_DIR"
    git clean -xfd
    git restore .
}

trap cleanup EXIT

set -ve

pushd "$CLONE_DIR" >&/dev/null

# Patch for building for wasm.

patch -N -p1 <<"EOF"
Do not allow F12 usage (screeenshot) as that is used in the browser for
debugging purposes.

--- a/src/config.h
+++ b/src/config.h
@@ -59,7 +59,7 @@
 // Use a partial-busy wait loop, in this case frame sleeps for most of the time, but then runs a busy loop at the end for accuracy
 #define SUPPORT_PARTIALBUSY_WAIT_LOOP    1
 // Allow automatic screen capture of current screen pressing F12, defined in KeyCallback()
-#define SUPPORT_SCREEN_CAPTURE          1
+//#define SUPPORT_SCREEN_CAPTURE          1
 // Allow automatic gif recording of current screen pressing CTRL+F12, defined in KeyCallback()
 #define SUPPORT_GIF_RECORDING           1
 // Support CompressData() and DecompressData() functions
EOF

# Do the build
pushd src >&/dev/null
(source "$EMSDK_ENV_SCRIPT"; make PLATFORM=PLATFORM_WEB)

# Save the library
install -D -m444 libraylib.a "$OUTPUT_DIR/lib/libraylib.a"

# Save the relevant headers
for header in raylib.h raymath.h; do
    install -D -m444 "$header" "$OUTPUT_DIR/include/$header"
done

popd >&/dev/null

popd >&/dev/null

echo Success
