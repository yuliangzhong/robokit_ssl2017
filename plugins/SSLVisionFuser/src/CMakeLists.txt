# output directory
# ----------------
include(SetOutputDirectory)

# pre process
# -----------
IF(WIN32)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${RBK_PROTO_DIR}/ssl/src_rawvision.proto
											${RBK_PROTO_DIR}/ssl/messages_robocup_ssl_detection.proto
											${RBK_PROTO_DIR}/ssl/messages_robocup_ssl_geometry.proto
											${RBK_PROTO_DIR}/ssl/messages_robocup_ssl_wrapper.proto)
ENDIF()

# define
# ------
add_definitions(-D__RBK_PLUGIN__="${PROJECT_NAME}")

# source
# ------
aux_source_directory(. SRCS)
aux_source_directory(./follow DIR_FOLLOW_SRCS)


# include
# -------
IF(UNIX)
	IF(NOT IS_SUBPROJECT)
		include_directories(BEFORE ${RBK_PATH}/include/robokit/protocol)
	ELSE()
		include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src/robokit/protocol)
	ENDIF()
ENDIF()

# link
# ----

# for VS solution file
# --------------------
IF(WIN32)
	file(GLOB HEADERS "*.h")
	file (GLOB FOLLOWHEADERS "follow/*.h")
	source_group("include" FILES ${HEADERS})
	source_group("src" FILES ${SRCS})
	source_group("include\\follow" FILES ${FOLLOWHEADERS})
	source_group("src\\follow" FILES ${DIR_FOLLOW_SRCS})
ENDIF()

# target
# ------
IF(WIN32)
	# windows
	add_library(${PROJECT_NAME} SHARED
			${SRCS}
			${HEADERS}
			${PROTO_SRCS}
			${PROTO_HDRS}
			${FOLLOWHEADERS}
			${DIR_FOLLOW_SRCS})
	target_link_libraries(${PROJECT_NAME}
			core
			foundation
			utils
			${LUA_LIBRARIES}
			${Protobuf_LIBRARIES}
			${SQLite3_LIBRARY})
ELSEIF(UNIX)
	IF(APPLE)
        # macOS
		add_library(${PROJECT_NAME} SHARED
				${SRCS}
				${DIR_FOLLOW_SRCS})
		target_link_libraries(${PROJECT_NAME}
				core
				protocol
				foundation
				utils
				boost_system-mt
				boost_thread-mt)
	ELSE(APPLE)
        # linux
		add_library(${PROJECT_NAME} SHARED
				${SRCS}
				${DIR_FOLLOW_SRCS})
		target_link_libraries(${PROJECT_NAME}
				core
				protocol
				foundation
				utils
				boost_system-mt
				boost_thread-mt)
	ENDIF()
ENDIF()

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "rbk_plugin")

# other
# -----
