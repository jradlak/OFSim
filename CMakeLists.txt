cmake_minimum_required(VERSION 3.16)

project(OFSim)
find_package( OpenGL REQUIRED )
find_package( Python REQUIRED Development)

include_directories( ${OPENGL_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only")

add_subdirectory(external_libraries/imgui)

# <================ Setup libraries =================>:

include(FetchContent)

# Low level 3D Graphics and Math libraries:
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
FetchContent_Declare(glad GIT_REPOSITORY https://github.com/SasLuca/MyGlad.git GIT_TAG master)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glad)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG		   0.9.9.2
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
	FetchContent_Populate(glm)
	set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
	add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()
message(STATUS "${Green} GLM Should Be Downloaded ${ColourReset}")

#Assimp Library:
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
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.15.2
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
    src/infrastructure/EventProcessor.h
    src/infrastructure/EventProcessor.cpp
            
    src/infrastructure/SimulationEvent.h

    src/python_integration/PythonMachine.h
    src/python_integration/PythonMachine.cpp
    
    src/math_and_physics/MathTypes.h
    
    src/math_and_physics/PhysicsSolver.h
    src/math_and_physics/Sphere.h
    
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
    
    src/world/Launchpad.h
    src/world/Rocket.h
    src/world/Smoke.h
    src/world/SolarSystemConstants.h    
    src/world/Launchpad.cpp
    src/world/Rocket.cpp
    src/world/Smoke.cpp        

    src/world/CelestialBody.h
    src/world/CloudsAndTrees.h        
    src/world/SolarSystem.h    
    src/world/CelestialBody.cpp
    src/world/CloudsAndTrees.cpp        
    src/world/SolarSystem.cpp    
  
    src/vmachine/Instructions.cpp
    src/vmachine/Instructions.h
    src/vmachine/Memory.cpp
    src/vmachine/Memory.h
    src/vmachine/Opcodes.h
    src/vmachine/Registers.cpp
    src/vmachine/Registers.h
    src/vmachine/Translator.cpp
    src/vmachine/Translator.h
    src/vmachine/VMachine.cpp
    src/vmachine/VMachine.h
    
    external_libraries/stb_image.h
    external_libraries/stb_image.cpp       )

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

target_link_libraries("${CMAKE_PROJECT_NAME}" ${OPENGL_LIBRARIES} glad glfw glm imgui assimp Python::Python)

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
    src/world/SolarSystem.h    
    src/world/CelestialBody.cpp
    src/world/CloudsAndTrees.cpp
    src/world/SolarSystem.cpp    

    src/math_and_physics/MathTypes.h
    
    src/math_and_physics/PhysicsSolver.h
    src/math_and_physics/Sphere.h
    
    src/math_and_physics/PhysicsSolver.cpp
    src/math_and_physics/Sphere.cpp
    
    src/gui/i18n.h    
    src/gui/i18n.cpp    

    src/infrastructure/FileService.h
    src/infrastructure/FileService.cpp
    src/infrastructure/ImageLoader.h    
    src/infrastructure/EventProcessor.h
    src/infrastructure/EventProcessor.cpp
    
    
 
    src/infrastructure/SimulationEvent.h

    src/vmachine/Instructions.cpp
    src/vmachine/Instructions.h
    src/vmachine/Memory.cpp
    src/vmachine/Memory.h
    src/vmachine/Opcodes.h
    src/vmachine/Registers.cpp
    src/vmachine/Registers.h
    src/vmachine/Translator.cpp
    src/vmachine/Translator.h
    src/vmachine/VMachine.cpp
    src/vmachine/VMachine.h
  
    external_libraries/stb_image.h
    external_libraries/stb_image.cpp  

    "tests/SphereTests.cpp"    
    "tests/PhysicsSolverTests.cpp"
    "tests/MemoryTests.cpp"
    "tests/TranslatorTests.cpp"
    "tests/VMTests.cpp")

target_link_libraries(
    ofsim_tests
    ${OPENGL_LIBRARIES}
    glad
    glfw
    glm
    assimp
    Python::Python
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ofsim_tests)
