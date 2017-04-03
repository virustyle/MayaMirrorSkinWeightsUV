cmake_minimum_required(VERSION 2.6)
project(MirrorSkinWeightsUV)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

set(SOURCE_FILES "src/MirrorSkinWeightsUVMain.cpp"
    "src/MirrorSkinWeightsUV.cpp" "src/MirrorSkinWeightsUV.h" 
)

find_package(Maya REQUIRED)

include_directories(${MAYA_INCLUDE_DIR})
link_directories(${MAYA_LIBRARY_DIR})
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${MAYA_LIBRARIES})

MAYA_PLUGIN(${PROJECT_NAME})


