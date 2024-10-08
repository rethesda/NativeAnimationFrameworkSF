cmake_minimum_required(VERSION 3.21)

# info
project(
	NativeAnimationFrameworkSF
	VERSION 1.2.2
	LANGUAGES CXX
)

# boiler
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)

# in-place configuration
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Plugin.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/Plugin.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

# source files
execute_process(COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/!update.ps1" "SOURCEGEN" "${PROJECT_VERSION}" "${CMAKE_CURRENT_BINARY_DIR}")
include(${CMAKE_CURRENT_BINARY_DIR}/sourcelist.cmake)
source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}
	FILES ${SOURCES}
)

source_group(
	TREE ${CMAKE_CURRENT_BINARY_DIR}
	FILES ${CMAKE_CURRENT_BINARY_DIR}/include/Plugin.h
)

# dependencies
find_package(libzippp CONFIG REQUIRED)
find_package(simdjson CONFIG REQUIRED)
#find_package(fastgltf CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_dependency_path(DKUtil include/DKUtil/Logger.hpp)
find_dependency_path(CommonLibSF include/SFSE/SFSE.h)
find_dependency_path(ozz-animation include/ozz/animation/runtime/animation.h)
find_dependency_path(fastgltf include/fastgltf/core.hpp)

# cmake target
# runtime
add_library(
	${PROJECT_NAME}
	SHARED
		${SOURCES}
		${CMAKE_CURRENT_BINARY_DIR}/include/Plugin.h
		${CMAKE_CURRENT_BINARY_DIR}/version.rc
		.clang-format
		vcpkg.json
 
 "src/Tasks/MainLoop.cpp"
 "src/Animation/Graph.cpp"
 "src/Animation/Transform.cpp"
 "src/Util/Math.cpp"
 "src/Animation/GraphManager.cpp" 
 "src/Settings/Settings.cpp" 
 "src/API/API_Internal.cpp" 
 "src/Settings/SkeletonDescriptor.cpp" 
 "src/Animation/Node.cpp"  
 "src/Serialization/GLTFImport.cpp"
 "src/Util/String.cpp"
 "src/Commands/NAFCommand.cpp"
 "src/Tasks/SaveLoadListener.cpp"
 "src/Animation/Generator.cpp"
 "src/Animation/IKTwoBoneData.cpp"
 "src/Animation/FileManager.cpp" 
 "src/Animation/Ozz.cpp"
 "src/Animation/SyncInstance.cpp"
 "src/Util/Trampoline.cpp"
 "src/Animation/Face/Manager.cpp"
 "src/Util/Timing.cpp"
 "src/Serialization/GLTFExport.cpp"
 "src/Tasks/Input.cpp"
 "src/Animation/Sequencer.cpp"
 "src/Commands/BetterConsole/NAF_UI.cpp"
 "src/Util/VM.cpp" 
 "src/Papyrus/NAFScript.cpp"
 "src/Animation/FileID.cpp"
 "src/Papyrus/EventManager.cpp"
 "src/Animation/PoseCache.cpp"
 "src/Animation/MorphAnimation.cpp"
 "src/Animation/Procedural/PGraph.cpp"
 "src/Animation/Procedural/PFullAnimationNode.cpp"
 "src/Animation/Procedural/PSmoothedRandNode.cpp"
 "src/Util/General.cpp"
 "src/Animation/Procedural/PBlend1DNode.cpp"
 "src/Animation/Procedural/PActorNode.cpp"
 "src/Animation/Procedural/PNode.cpp"
 "src/Serialization/BlendGraphImport.cpp"
 "src/Animation/Procedural/PInternalCacheReleaseNode.cpp"
 "src/Animation/Procedural/PFixedValueNode.cpp"
 "src/Animation/IAnimationFile.cpp"
 "src/Animation/Procedural/PAdditiveBlendNode.cpp"
 "src/Animation/Procedural/PVariableNode.cpp"
 "src/Animation/Procedural/PLimitROCNode.cpp"
 "src/Animation/Procedural/PTransformRangeNode.cpp")

# copy to Starfield directory
set(STARFIELD_PATH "C:/Program Files (x86)/Steam/steamapps/common/Starfield/Data/SFSE/Plugins")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${STARFIELD_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> ${STARFIELD_PATH}
    COMMENT "Copying plugin to Starfield directory..."
)

# include dir
target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

# linkage
target_link_libraries(
	${PROJECT_NAME} 
	PRIVATE
		DKUtil::DKUtil
		CommonLibSF::CommonLibSF
		spdlog::spdlog
		simdjson::simdjson
		fastgltf::fastgltf
		libzippp::libzippp
		ozz_animation
		ozz_animation_offline
)

# compiler def
if (MSVC)
	add_compile_definitions(_UNICODE)

	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			/MP
			/await
			/W0
			/WX
			/permissive-
			/utf-8
			/Zc:alignedNew
			/Zc:auto
			/Zc:__cplusplus
			/Zc:externC
			/Zc:externConstexpr
			/Zc:forScope
			/Zc:hiddenFriend
			/Zc:implicitNoexcept
			/Zc:lambda
			/Zc:noexceptTypes
			/Zc:preprocessor
			/Zc:referenceBinding
			/Zc:rvalueCast
			/Zc:sizedDealloc
			/Zc:strictStrings
			/Zc:ternary
			/Zc:threadSafeInit
			/Zc:trigraphs
			/Zc:wchar_t
			/wd4200 # nonstandard extension used : zero-sized array in struct/union
			/FI${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.h
	)
endif()

# PCH
target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
		src/PCH.h
)

set_property(
	TARGET 
	${PROJECT_NAME}
	PROPERTY VS_USER_PROPS 
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_stl_modules.props"
)

# update deployments
add_custom_command(
	TARGET 
	${PROJECT_NAME} 
	POST_BUILD
	COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File 
		"${CMAKE_CURRENT_SOURCE_DIR}/!update.ps1" "DISTRIBUTE" "${PROJECT_VERSION}" "${CMAKE_CURRENT_BINARY_DIR}/$(ConfigurationName)" "${PROJECT_NAME}" 
)
