cmake_minimum_required(VERSION 2.8)
project(NeuralNetwork)
option(PROJECT_USE_GPU  "GPU Support" 0)


set ( Boost_USE_STATIC_LIBS ON )

find_package(Boost REQUIRED)


if ( Boost_FOUND )
    set(INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
    set(LINK_DIRS ${Boost_INCLUDE_DIRS}/stage/lib)
    set(LINK_LIBS ${Boost_LIBRARIES})
else ( Boost_FOUND )

   message( fatal_error "Boost not found. Cannot continue." )

endif( Boost_FOUND )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(NOT PROJECT_USE_GPU)

    set(SOURCE_FILES
                    main.cpp
                    src/NeuronDatareader.cpp

                    )
    set(HEADER_FILES
                    include/NeuronDatareader.h
                    Neuron_Functors.h
                    include/Network.h
                    include/FixedPoint.h
                    include/NetworkContainer.h
                    fpml/fixed_point.h
                    )
endif(NOT PROJECT_USE_GPU)

if(PROJECT_USE_GPU)

    set(SOURCE_FILES
                    main.cpp
                    src/NeuronDatareader.cpp
                    src/gpunetworkcontextmanager.cpp

                    )
    set(HEADER_FILES
                    include/NeuronDatareader.h
                    Neuron_Functors.h
                    include/Network.h
                    include/FixedPoint.h
                    include/NetworkContainer.h
                    include/gpunetwork.h
                    include/gpunetworkcontainer.h
                    include/gpunetworkcontextmanager.h
                    include/GPUBuffer.h
                    fpml/fixed_point.h
                    )
endif(PROJECT_USE_GPU)


if( PROJECT_USE_GPU)
    find_package(OpenCL REQUIRED)
    set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENCL_INCLUDE_DIRS})
    set(LINK_LIBS ${LINK_LIBS} ${OPENCL_LIBRARIES} )
    message(${INCLUDE_DIRS})
    add_definitions( -DUSE_GPU )
endif(PROJECT_USE_GPU)

add_definitions(-D_USE_MATH_DEFINES)
set(CMAKE_BUILD_TYPE Debug)
include_directories(${INCLUDE_DIRS})
message(${INCLUDE_DIRS})
add_executable( NeuralNetwork ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries( NeuralNetwork ${LINK_LIBS})
