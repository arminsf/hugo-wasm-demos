# Emscripten demos embedded into a Hugo site

## Building for Hugo

In the root directory:

```bash
emcmake cmake -S cpp -B build-web -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
cmake --install build-web --prefix hugo/static
cd hugo
hugo server --buildDrafts
```

## Building SDL demos only (native)

`cd cpp` first. Then to compile:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Building SDL demos only (Emscripten)

To compile to WASM, make sure your environment has [emcc](https://emscripten.org/docs/getting_started/downloads.html), then:

```bash
emcmake cmake -S . -B build-web -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
```

Then just use an http server to see them.
