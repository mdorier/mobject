include_directories(../include)

add_library(mobject-test Main.cpp)
target_link_libraries(mobject-test PkgConfig::cppunit mobject-server mobject-admin mobject-client)

add_executable(AdminTest AdminTest.cpp)
target_link_libraries(AdminTest mobject-test)

add_executable(ClientTest AdminTest.cpp)
target_link_libraries(ClientTest mobject-test)

add_executable(SequencerTest SequencerTest.cpp)
target_link_libraries(SequencerTest mobject-test)

add_test(NAME AdminTest COMMAND ./AdminTest AdminTest.xml)
add_test(NAME ClientTest COMMAND ./ClientTest ClientTest.xml)
add_test(NAME SequencerTest COMMAND ./SequencerTest SequencerTest.xml)
