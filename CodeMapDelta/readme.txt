Current developer environment:
OS: Win7
IDE: Visual Studio 2017
Build tool: Cmake 3.9.6
Dependecy manager: vcpkg

Dependencies:
 - clang 6.0
 - Qt 5.10.1
 - catch2 2.5.0
 
 
To include catch2 with vcpkg:
 - on comandline add : "-DCMAKE_TOOLCHAIN_FILE=<vcpkg>\scripts\buildsystems\vcpkg.cmake"
    And call clean. Delete cache and try again if find_package cant find cahce2 at first.
 - with VS2017: create CMakeSettings.json and add these lines.
      "variables": [
        {
          "name": "CMAKE_TOOLCHAIN_FILE",
          "value": "<vcpkg>\\scripts\\buildsystems\\vcpkg.cmake"
        }
      ]
