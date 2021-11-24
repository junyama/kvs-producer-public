# kvs-producer

Based on oatpp Example-CRUD project and integrated with gstreamer kvssink plugin.

See more:

- [Oat++ Website](https://oatpp.io/)
- [Oat++ Example-CRUD](https://oatpp.io/examples/crud/#example-crud)
- [Gstreamer Tutorial](https://gstreamer.freedesktop.org/documentation/tutorials/basic/concepts.html?gi-language=c#basic-tutorial-2-gstreamer-concepts)
- [amazon KVS Producer C++ SDK](https://docs.aws.amazon.com/kinesisvideostreams/latest/dg/producer-sdk-cpp.html)

## Overview

### Project layout

```
|- CMakeLists.txt                        // projects CMakeLists.txt
|- src/
|    |
|    |- controller/                      // Folder containing API Controllers where all endpoints are declared
|    |- dto/                             // DTOs are declared here
|    |- gst/                             // Gstreamer applications
|    |- websocket                        // WebSocket handler
|    |- SwaggerComponet.hpp              // Swagger-UI config
|    |- AppComponent.hpp                 // Service config
|    |- App.cpp                          // main() is here
|
|- www/                                  // Web UI contents
```

---

### Build and Run

#### Using CMake

##### Pre Requirements
- `oatpp 1.3.0` 
- `oatpp-swagger 1.3.0`
##### Build Project
```
$ mkdir build && cd build
$ cmake ..
$ make
```
##### Set environment variable
```
export GST_PLUGIN_PATH=<directory of kvssink plug-in>
export SWAGGER_SERVER_IP=<IP address of the host>
```
(*Note: If you add the command lines above to ~/.bashrc, you do not need to excute the commands in every session*)
##### Set Gstreamer parameters
Customize gstreamer parameters at gstreamer.json, if necessary. Refere to src/dto/DTOs.hpp for the default values of the parameters. 
```
$ vi ../conf.d/gstreamer.json
```
#### Run the program
```
$ ./kvs-producer-exe
```
---
### Swagger to test APIs
Access the following URL to test APIs
```
http://<IP address of the host>:8080
```
