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

### Build
```
$ mkdir build && cd build
$ cmake ..
$ make
```
### Set environment variable
If you add the following line to .bashrc, you do not need to excute the setting in every session
```
export GST_PLUGIN_PATH=<directory of kvssink plug-in>
```
### Run web server
```
$ ./kvs-producer-exe

```
### Start kvs producer
```
$ curl http://localhost:8000/start

```
### Pause kvs producer
```
$ curl http://localhost:8000/pause

```
### Stop kvs producer
```
$ curl http://localhost:8000/stop_kvssink

```
### Send metadata to kvssink
```
$ curl -X POST -H "Content-Type: application/json" -d '{"x":100, "y":100, "z":100}' http://localhost:8000/send_metadata
```
### Monitor gstreamer pipeline
You can start the continuous process (loop) as follows to monitor the bus and check any error or EOS (end of stream). If the process finds any error, EOS or kvssink state changed to NULL, it shutdowns the pipeline.
- Connect websocket at 
```
ws://localhost:8000/ws
```
- Send a text message, 'monitor' to the socket (no ' for an actuall message)
### Swagger to test APIs
Access the following URL to test APIs
```
http://localhost:8000

```