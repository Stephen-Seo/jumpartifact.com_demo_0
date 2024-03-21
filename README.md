# (First) Demo for jumpartifact.com

This project will be the demo displayed on the home page of
[jumpartifact.com](https://jumpartifact.com).

Currently, a forgejo action is configured for this repo to publish the WASM
build to [jumpartifact.com/demo0](https://jumpartifact.com/demo0/).

## Setting up

Clone `https://github.com/emscripten-core/emsdk.git` to `${HOME}/git/emsdk`, and
run the following commands in the cloned directory:

    git checkout 3.1.56
    ./emsdk install 3.1.56
    ./emsdk activate 3.1.56

Now you can use emsdk by sourcing the `emsdk_env.sh` file and using emcc/em++.
The Makefile in `wasm_build/` should now "just work" with `make`.

## Testing the wasm build

After building the wasm version of this demo (which should have emitted an .html
file and a .wasm file and possibly more), you can invoke `python -m http.server`
(or use whatever http server you want) in the directory where the .html file is,
and open your browser to `localhost:8000` (if using `python -m http.server`).
You should then be able to click on the emitted .html to open the wasm build in
the browser.
