INCLUDE(BuildPlugin)

BUILD_PLUGIN(jacobi jacobi_instrument.cpp jacobi_instrument.h MOCFILES jacobi_instrument.h EMBEDDED_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.png")


