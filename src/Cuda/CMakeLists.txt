# Modified to add cuda files
macro(ADD_CUDA_SOURCE_GROUP MODULE_NAME)
    file(GLOB MODULE_CUDA_HEADER_FILES "${MODULE_NAME}/*.cuh")
    source_group("Header Files\\${MODULE_NAME}" FILES
            ${MODULE_CUDA_HEADER_FILES})
    file(GLOB MODULE_CUDA_SOURCE_FILES "${MODULE_NAME}/*.cu")
    source_group("Source Files\\${MODULE_NAME}" FILES
            ${MODULE_CUDA_SOURCE_FILES})
endmacro(ADD_CUDA_SOURCE_GROUP)

find_package(CUDA 8.0 REQUIRED)
# NOTE: Eigen <= 3.3.5 has problem with CUDA >= 9.0
# Source code have to be modified in Eigen/Core to ignore fp16:
# Comment the following lines:
# #include "src/Core/arch/CUDA/Half.h"
# #include "src/Core/arch/CUDA/PacketMathHalf.h"
# #include "src/Core/arch/CUDA/TypeCasting.h"

list(APPEND 3RDPARTY_INCLUDE_DIRS ${CUDA_TOOLKIT_INCLUDE})
list(APPEND 3RDPARTY_LIBRARIES
        ${CUDA_LIBRARIES} ${CUDA_curand_LIBRARY})
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${3RDPARTY_INCLUDE_DIRS})
link_directories(${3RDPARTY_LIBRARY_DIRS})

# build
file(GLOB_RECURSE CUDA_ALL_HEADER_FILES "*.h")
file(GLOB_RECURSE CUDA_ALL_SOURCE_FILES "*.cpp")
file(GLOB_RECURSE CUDA_ALL_CUDA_HEADER_FILES "*.cuh")
file(GLOB_RECURSE CUDA_ALL_CUDA_SOURCE_FILES "*.cu")

# CUDA build for nano
set(CUDA_NVCC_FLAGS
        -gencode=arch=compute_53,code=sm_53
        -gencode=arch=compute_53,code=compute_53
        --expt-relaxed-constexpr)



# create (should be static) library
set(OPEN3D_CUDA_MODULE "Open3DCuda" CACHE STRING "CUDA component library")
cuda_add_library(${OPEN3D_CUDA_MODULE}
        ${CUDA_ALL_HEADER_FILES} ${CUDA_ALL_SOURCE_FILES}
        ${CUDA_ALL_CUDA_HEADER_FILES} ${CUDA_ALL_CUDA_SOURCE_FILES}
        $<TARGET_OBJECTS:Camera>
        $<TARGET_OBJECTS:ColorMap>
        $<TARGET_OBJECTS:Geometry>
        $<TARGET_OBJECTS:Integration>
        $<TARGET_OBJECTS:Odometry>
        $<TARGET_OBJECTS:Registration>
        $<TARGET_OBJECTS:Utility>
        $<TARGET_OBJECTS:IO>
        $<TARGET_OBJECTS:Visualization>)
target_link_libraries(${OPEN3D_CUDA_MODULE} ${3RDPARTY_LIBRARIES} -lz) # for zlib
target_include_directories(${OPEN3D_CUDA_MODULE} PUBLIC ${3RDPARTY_INCLUDE_DIRS})

# create subgroups
ADD_SOURCE_GROUP(Common)
ADD_SOURCE_GROUP(Camera)
ADD_SOURCE_GROUP(Container)
ADD_SOURCE_GROUP(Geometry)
ADD_SOURCE_GROUP(Integration)
ADD_SOURCE_GROUP(Registration)
ADD_SOURCE_GROUP(IO)
ADD_SOURCE_GROUP(Odometry)
ADD_SOURCE_GROUP(Visualization)

# and also for cuda
ADD_CUDA_SOURCE_GROUP(Common)
ADD_CUDA_SOURCE_GROUP(Camera)
ADD_CUDA_SOURCE_GROUP(Container)
ADD_CUDA_SOURCE_GROUP(Geometry)
ADD_CUDA_SOURCE_GROUP(Integration)
ADD_CUDA_SOURCE_GROUP(Registration)
ADD_CUDA_SOURCE_GROUP(IO)
ADD_CUDA_SOURCE_GROUP(Odometry)
ADD_CUDA_SOURCE_GROUP(Visualization)

Directories(. CUDA_INCLUDE_DIRS)
set(CUDA_INCLUDE_DIRS ${CUDA_INCLUDE_DIRS} PARENT_SCOPE)
# target_include_directories(Cuda PUBLIC ${CUDA_INCLUDE_DIRS})

#install headers
install(DIRECTORY "."
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${CMAKE_PROJECT_NAME}"
        PATTERN "*.cpp" EXCLUDE
        PATTERN "*.cuh" EXCLUDE
        PATTERN "*.cu" EXCLUDE
        PATTERN "*.txt" EXCLUDE)
