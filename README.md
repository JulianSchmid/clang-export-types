# clang-export-types - WIP - NOT READY - DONT EVEN LOOK AT IT
Clang tool for exporting types from c++ source files

## Building

To build this tool you will need to check out the `llvm` repository and checkout the `clang-export-types` repository into it:

```sh
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
git clone https://github.com/JulianSchmid/clang-export-types.git clang-tools-extra/clang-export-types
```

Next you will have to add the tool to the CMakeLists.txt of the llvm repository:

```sh
echo 'add_subdirectory(clang-export-types)' >> clang-tools-extra/CMakeLists.txt
```

 Then setup a build directory for llvm, clang and clang-export-types:

```sh
# in llvm-project
mkdir build
cd build
cmake -G Ninja ../llvm -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra"
ninja clang-export-types
```

Alternatively you can also build the tool in release mode:

```sh
cmake -G Ninja ../llvm -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_ASSERTIONS=0
ninja clang-export-types
```

 It is also possible to build with a different build system then ninja (e.g. xcode, visual studio and make). Read the [LLVM Getting Started Guide](https://llvm.org/docs/GettingStarted.html) for more details.
