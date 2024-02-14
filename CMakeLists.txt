cmake_minimum_required(VERSION 3.16)

project(OSim)

find_package( OpenGL REQUIRED )

include_directories( ${OPENGL_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only")

add_subdirectory(external_libraries/glfw)
add_subdirectory(external_libraries/imgui)
add_subdirectory(external_libraries/glm)

#Assimp Library:
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/libs CACHE PATH "Missing description." FORCE)
FetchContent_Declare(assimp
GIT_REPOSITORY https://github.com/assimp/assimp.git
GIT_TAG master)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

#Google Test Library:
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

add_executable("${CMAKE_PROJECT_NAME}"     
    src/Application.cpp
    
    src/simulation/Simulation.h
    
    src/simulation/TelemetryCollector.h
    src/simulation/TelemetryData.h    
    src/simulation/Simulation.cpp    
    src/simulation/TelemetryCollector.cpp    

    src/infrastructure/FileService.h
    src/infrastructure/FileService.cpp
    src/infrastructure/ImageLoader.h
    src/infrastructure/CommunicationBus.h
    src/infrastructure/CommunicationBus.cpp

    src/math_and_physics/MathTypes.h
    src/math_and_physics/Geometry.h
    src/math_and_physics/PhysicsSolver.h
    src/math_and_physics/Sphere.h
    src/math_and_physics/Geometry.cpp
    src/math_and_physics/PhysicsSolver.cpp
    src/math_and_physics/Sphere.cpp

    src/gui/Gui.h
    src/gui/Gui.cpp
    src/gui/i18n.h
    src/gui/i18n.cpp
    
    src/renderer/Camera.h
    src/renderer/Mesh.h
    src/renderer/Model.h
    src/renderer/ModelRenderer.h
    src/renderer/ObjectRenderer.h
    
    src/renderer/Shader.h
    src/renderer/SkyBoxRenderer.h
    src/renderer/TrajectoryPrediction.h
    src/renderer/Window.h
    src/renderer/Camera.cpp
    src/renderer/Mesh.cpp
    src/renderer/Model.cpp
    src/renderer/ModelRenderer.cpp
    src/renderer/ObjectRenderer.cpp    
    src/renderer/Shader.cpp
    src/renderer/SkyBoxRenderer.cpp
    src/renderer/TrajectoryPrediction.cpp
    src/renderer/Window.cpp
    
    src/vmachine/Instructions.h
    src/vmachine/Memory.h
    "src/infrastructure/ODDMA.h"
    "src/vmachine/Opcodes.h"
    src/vmachine/Registers.h
    "src/infrastructure/RocketCommand.h"
    src/vmachine/Translator.h
    src/vmachine/VMachine.h        
    src/vmachine/Instructions.cpp
    src/vmachine/Memory.cpp
    "src/infrastructure/ODDMA.cpp"    
    src/vmachine/Registers.cpp    
    src/vmachine/Translator.cpp
    src/vmachine/VMachine.cpp

    src/world/Launchpad.h
    src/world/Rocket.h
    src/world/Smoke.h
    src/world/SolarSystemConstants.h    
    src/world/Launchpad.cpp
    src/world/Rocket.cpp
    src/world/Smoke.cpp        

    src/world/CelestialBody.h
    src/world/CloudsAndTrees.h
    src/world/Moon.h
    src/world/Planet.h
    src/world/SolarSystem.h
    src/world/Sun.h
    src/world/CelestialBody.cpp
    src/world/CloudsAndTrees.cpp
    src/world/Moon.cpp
    src/world/Planet.cpp
    src/world/SolarSystem.cpp
    src/world/Sun.cpp
    
    external_libraries/glad/glad.h 
    external_libraries/glad/glad.c
    external_libraries/stb_image.h
    external_libraries/stb_image.cpp   
 )

#copy shaders to binary directory:
if(MSVC)
    add_custom_target(copy_shaders ALL
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${PROJECT_SOURCE_DIR}/src/shaders
                ${PROJECT_BINARY_DIR}/shaders
                COMMENT "Copying shaders into binary directory")

    add_dependencies("${CMAKE_PROJECT_NAME}" copy_shaders)
else()
    add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/src/shaders
    ${PROJECT_BINARY_DIR}/shaders
    COMMENT "Copying shaders into binary directory")

    add_dependencies("${CMAKE_PROJECT_NAME}" copy_shaders)
endif()

#copy assets to binary directory:
if(MSVC)
    add_custom_target(copy_assets ALL
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${PROJECT_SOURCE_DIR}/assets
                ${PROJECT_BINARY_DIR}/assets
                COMMENT "Copying assets into binary directory")

    add_dependencies("${CMAKE_PROJECT_NAME}" copy_assets)
else()
    add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/assets
    ${PROJECT_BINARY_DIR}/assets
    COMMENT "Copying assets into binary directory")

    add_dependencies("${CMAKE_PROJECT_NAME}" copy_assets)
endif()

target_link_libraries("${CMAKE_PROJECT_NAME}" ${OPENGL_LIBRARIES} glfw glm imgui assimp)

if(MSVC)
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0") 
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'GLFW-CMake-starter' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GLFW-CMake-starter )
    endif()
endif()


enable_testing()
add_executable(
  ofsim_tests  
    src/renderer/Camera.h
    src/renderer/Mesh.h
    src/renderer/Model.h
    src/renderer/ModelRenderer.h
    src/renderer/ObjectRenderer.h
    
    src/renderer/Shader.h
    src/renderer/SkyBoxRenderer.h
    src/renderer/TrajectoryPrediction.h
    src/renderer/Window.h
    src/renderer/Camera.cpp
    src/renderer/Mesh.cpp
    src/renderer/Model.cpp
    src/renderer/ModelRenderer.cpp
    src/renderer/ObjectRenderer.cpp    
    src/renderer/Shader.cpp
    src/renderer/SkyBoxRenderer.cpp
    src/renderer/TrajectoryPrediction.cpp
    src/renderer/Window.cpp

    src/world/Launchpad.h
    src/world/Rocket.h
    src/world/Smoke.h
    src/world/SolarSystemConstants.h    
    src/world/Launchpad.cpp
    src/world/Rocket.cpp
    src/world/Smoke.cpp        

    src/world/CelestialBody.h
    src/world/CloudsAndTrees.h
    src/world/Moon.h
    src/world/Planet.h
    src/world/SolarSystem.h
    src/world/Sun.h
    src/world/CelestialBody.cpp
    src/world/CloudsAndTrees.cpp
    src/world/Moon.cpp
    src/world/Planet.cpp
    src/world/SolarSystem.cpp
    src/world/Sun.cpp

    src/math_and_physics/MathTypes.h
    src/math_and_physics/Geometry.h
    src/math_and_physics/PhysicsSolver.h
    src/math_and_physics/Sphere.h
    src/math_and_physics/Geometry.cpp
    src/math_and_physics/PhysicsSolver.cpp
    src/math_and_physics/Sphere.cpp
    
    src/gui/i18n.h    
    src/gui/i18n.cpp    

    src/infrastructure/FileService.h
    src/infrastructure/FileService.cpp
    src/infrastructure/ImageLoader.h
    src/infrastructure/CommunicationBus.h
    src/infrastructure/CommunicationBus.cpp

    src/vmachine/Instructions.h
    src/vmachine/Memory.h
    "src/infrastructure/ODDMA.h"
    "src/vmachine/Opcodes.h"
    src/vmachine/Registers.h
    "src/infrastructure/RocketCommand.h"
    src/vmachine/Translator.h
    src/vmachine/VMachine.h        
    src/vmachine/Instructions.cpp
    src/vmachine/Memory.cpp
    "src/infrastructure/ODDMA.cpp"    
    src/vmachine/Registers.cpp    
    src/vmachine/Translator.cpp
    src/vmachine/VMachine.cpp

    external_libraries/glad/glad.h 
    external_libraries/glad/glad.c
    external_libraries/stb_image.h
    external_libraries/stb_image.cpp  

    "tests/SphereTests.cpp"    
    "tests/PhysicsSolverTests.cpp"
    "tests/MemoryTests.cpp" 
    "tests/com_bus_tests.cpp"
    "tests/TranslatorTests.cpp" 
    "tests/VMTests.cpp")
target_link_libraries(
    ofsim_tests
    ${OPENGL_LIBRARIES}
    glfw
    glm
    assimp
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ofsim_tests)