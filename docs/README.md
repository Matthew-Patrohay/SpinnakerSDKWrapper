# SpinnakerSDKWrapper

## Overview

Welcome to **SpinnakerSDKWrapper**, an open-source C++ wrapper library for the Flir Spinnaker SDK. This project aims to simplify the usage of Flir Spinnaker cameras by providing an easy-to-use, high-level API. With SpinnakerSDKWrapper, you can quickly integrate camera functionality into your C++ applications.

## Simple Use

If you have the Spinnaker SDK installed, and you are using a Mac (it installs to the Applications folder) then you can compile example programs as follows:

g++ -std=c++14 -I/Applications/Spinnaker/include -o ./bin/simple ./examples/simple.cpp ./src/SpinnakerSDK_SpinCamera.cpp ./src/SpinnakerSDK_SpinImage.cpp -L/usr/local/lib -lSpinnaker -Wl,-rpath,/Applications/Spinnaker/lib