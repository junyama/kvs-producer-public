cmake_minimum_required(VERSION 3.1)

set(project_name kvs-producer) ## rename your project here

set(CMAKE_CXX_STANDARD 11)

add_library(${project_name}-lib
        src/controller/StaticController.hpp
        src/controller/UserController.hpp
        ## src/db/UserDb.hpp
        ## src/dto/PageDto.hpp
        ## src/dto/StatusDto.hpp
        ## src/dto/UserDto.hpp
        ## src/service/UserService.cpp
        ## src/service/UserService.hpp
        src/AppComponent.hpp
        ## src/DatabaseComponent.hpp
        src/SwaggerComponent.hpp
        ## src/ErrorHandler.cpp
        ## src/ErrorHandler.hpp
        
        src/gst/MyGstreamer.cpp      ## added for gstreamer
        src/dto/DTOs.hpp
        src/websocket/WSListener.cpp
        src/websocket/WSListener.hpp
        
        )

## include directories

target_include_directories(${project_name}-lib PUBLIC src)


## link libs

find_package(oatpp          1.3.0 REQUIRED)
find_package(oatpp-swagger  1.3.0 REQUIRED)
## find_package(oatpp-sqlite   1.3.0 REQUIRED)

find_package(oatpp-websocket    1.3.0 REQUIRED) ##

## added for gstreamer #########################################

find_package(PkgConfig) #finding pkg-config is a helper tool

#using pkg-config to getting Gstreamer
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

#including GStreamer header files directory
include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIRS}
)

#linking GStreamer library directory
link_directories(
        ${GLIB_LIBRARY_DIRS}
        ${GSTREAMER_LIBRARY_DIRS}
)

##################################################################

target_link_libraries(${project_name}-lib
        # Oat++
        PUBLIC oatpp::oatpp
        PUBLIC oatpp::oatpp-swagger
        ## PUBLIC oatpp::oatpp-sqlite
        
        ${GSTREAMER_LIBRARIES}      ## added for gstreamer
        PUBLIC oatpp::oatpp-websocket
)

# If CMake can't find SQLite3:
#
# 1. Make sure that you've built oatpp-sqlite with -DOATPP_SQLITE_AMALGAMATION=ON flag
# 2. If you are not willing to use SQLite amalgamation then uncomment the following lines:
#
#find_package(SQLite3 REQUIRED)
#
#target_link_libraries(${project_name}-lib
#        PUBLIC SQLite::SQLite3
#)

add_definitions(
        ## define path to swagger-ui static resources folder
        -DOATPP_SWAGGER_RES_PATH="${oatpp-swagger_INCLUDE_DIRS}/../bin/oatpp-swagger/res"

        ## SQLite database file
        ## -DDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/db.sqlite"
        ## SQLite database test file
        ## -DTESTDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/test-db.sqlite"

        ## Path to database migration scripts
        ## -DDATABASE_MIGRATIONS="${CMAKE_CURRENT_SOURCE_DIR}/sql"
)

if(CMAKE_SYSTEM_NAME MATCHES Linux)
    find_package(Threads REQUIRED)
    target_link_libraries(${project_name}-lib INTERFACE Threads::Threads ${CMAKE_DL_LIBS})
endif()

## add executables

add_executable(${project_name}-exe src/App.cpp)
target_link_libraries(${project_name}-exe ${project_name}-lib)

## add_executable(${project_name}-test
##         test/tests.cpp
##         test/app/TestClient.hpp
##         ## test/app/TestDatabaseComponent.hpp
##         test/app/TestComponent.hpp
##         test/UserControllerTest.hpp
##         test/UserControllerTest.cpp)
## target_link_libraries(${project_name}-test ${project_name}-lib)

## enable_testing()
## add_test(project-tests ${project_name}-test)

