cmake_minimum_required(VERSION 3.9)
project(SURFACE_RECONSTRUCTION)

set(CGAL_DIR "$ENV{HOME}/CGAL-5.5.2")
set(CMAKE_BUILD_TYPE "Release")

find_package(CGAL REQUIRED)
find_package (Eigen3 REQUIRED NO_MODULE)
message("-- CGAL VERSION: ${CGAL_VERSION}")

add_executable(alpha_wrap main.cpp shrink_wrap.cpp)
target_include_directories(alpha_wrap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(alpha_wrap CGAL::CGAL)
target_link_libraries(alpha_wrap Eigen3::Eigen)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS filesystem)

if(Boost_FOUND)
    target_include_directories(alpha_wrap PUBLIC ${Boost_INCLUDE_DIRS}) 
    target_link_libraries(alpha_wrap ${Boost_LIBRARIES})
endif()