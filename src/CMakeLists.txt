# set source files
set (server-src-files
     Provider.cpp
     Backend.cpp)

set (client-src-files
     Client.cpp
     SequencerHandle.cpp
     AsyncRequest.cpp)

set (admin-src-files
     Admin.cpp)

set (dummy-src-files
     dummy/DummyBackend.cpp)

set (module-src-files
     BedrockModule.cpp)

# load package helper for generating cmake CONFIG packages
include (CMakePackageConfigHelpers)

# where to install files for "find_package"
set (mobject-pkg "share/cmake/mobject")

# set version fo shared library
set (mobject-vers "${MOBJECT_VERSION_MAJOR}.${MOBJECT_VERSION_MINOR}")

# server library
add_library (mobject-server ${server-src-files} ${dummy-src-files})
target_link_libraries (mobject-server
    thallium
    PkgConfig::ABTIO
    PkgConfig::UUID
    nlohmann_json::nlohmann_json
    spdlog::spdlog)
target_include_directories (mobject-server PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (mobject-server BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (mobject-server BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (mobject-server
    PROPERTIES VERSION ${MOBJECT_VERSION}
    SOVERSION ${MOBJECT_VERSION_MAJOR})

# client library
add_library (mobject-client ${client-src-files})
target_link_libraries (mobject-client thallium PkgConfig::UUID spdlog::spdlog)
target_include_directories (mobject-client PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (mobject-client BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (mobject-client BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (mobject-client
    PROPERTIES VERSION ${MOBJECT_VERSION}
    SOVERSION ${MOBJECT_VERSION_MAJOR})

# admin library
add_library (mobject-admin ${admin-src-files})
target_link_libraries (mobject-admin thallium PkgConfig::UUID nlohmann_json::nlohmann_json spdlog::spdlog)
target_include_directories (mobject-admin PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (mobject-admin BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (mobject-admin BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (mobject-admin
    PROPERTIES VERSION ${MOBJECT_VERSION}
    SOVERSION ${MOBJECT_VERSION_MAJOR})

if (${ENABLE_BEDROCK})
# bedrock module library
add_library (mobject-bedrock-module ${module-src-files})
target_link_libraries (mobject-bedrock-module mobject-server mobject-admin mobject-client bedrock-server)
target_include_directories (mobject-bedrock-module PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (mobject-bedrock-module BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (mobject-bedrock-module BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (mobject-bedrock-module
    PROPERTIES VERSION ${MOBJECT_VERSION}
    SOVERSION ${MOBJECT_VERSION_MAJOR})
endif ()

# installation stuff (packaging and install commands)
write_basic_package_version_file(
    "mobject-config-version.cmake"
    VERSION ${MOBJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)

# generate our config file for find_package()
set(INCLUDE_INSTALL_DIR include/)
set(LIB_INSTALL_DIR lib/)
configure_package_config_file(mobject-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/mobject-config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/mobject/cmake
  PATH_VARS INCLUDE_INSTALL_DIR)

# some bits for the pkg-config file
set (DEST_DIR "${CMAKE_INSTALL_PREFIX}")
set (SERVER_PRIVATE_LIBS "-lmobject-server")
set (CLIENT_PRIVATE_LIBS "-lmobject-client")
set (ADMIN_PRIVATE_LIBS  "-lmobject-admin")
configure_file ("mobject-server.pc.in" "mobject-server.pc" @ONLY)
configure_file ("mobject-client.pc.in" "mobject-client.pc" @ONLY)
configure_file ("mobject-admin.pc.in"  "mobject-admin.pc"  @ONLY)

# configure config.h
configure_file ("config.h.in" "config.h" @ONLY) 

# "make install" rules
install (TARGETS mobject-admin mobject-server mobject-client
         EXPORT mobject-targets
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
if (${ENABLE_BEDROCK})
    install (TARGETS mobject-bedrock-module
             ARCHIVE DESTINATION lib
             LIBRARY DESTINATION lib)
endif ()
install (EXPORT mobject-targets
         DESTINATION ${mobject-pkg}
         FILE "mobject-targets.cmake")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/mobject-config.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/mobject-config-version.cmake"
         DESTINATION ${mobject-pkg} )
install (DIRECTORY ../include/mobject
         DESTINATION include
         FILES_MATCHING PATTERN "*.h")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/mobject-server.pc"
               "${CMAKE_CURRENT_BINARY_DIR}/mobject-client.pc"
               "${CMAKE_CURRENT_BINARY_DIR}/mobject-admin.pc"
         DESTINATION "lib/pkgconfig/")
