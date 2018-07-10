# output directory
# ----------------
include(SetOutputDirectory)

# pre process
# -----------
IF(WIN32)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${RBK_PROTO_DIR}/ssl/src_rawvision.proto
											${RBK_PROTO_DIR}/ssl/messages_robocup_ssl_detection.proto
											${RBK_PROTO_DIR}/ssl/messages_robocup_ssl_geometry.proto
											${RBK_PROTO_DIR}/ssl/messages_robocup_ssl_wrapper.proto
											${RBK_PROTO_DIR}/ssl/src_debug.proto
											${RBK_PROTO_DIR}/ssl/referee.proto
											${RBK_PROTO_DIR}/ssl/src_cmd.proto)
ENDIF()

# define
# ------
add_definitions(-D__RBK_PLUGIN__="${PROJECT_NAME}")
remove_definitions(-DUNICODE -D_UNICODE)

# source
# ------
aux_source_directory(. SRCS)
aux_source_directory(./LuaModule DIR_LUAMODULE_SRCS)
aux_source_directory(./ssl DIR_SSL_SRCS)
aux_source_directory(./ssl/bayes DIR_SSL_BAYES_SRCS)
aux_source_directory(./ssl/cmu DIR_SSL_CMU_SRCS)
aux_source_directory(./ssl/cornell DIR_SSL_CORNELL_SRCS)
aux_source_directory(./ssl/defence DIR_SSL_DEFENCE_SRCS)
aux_source_directory(./ssl/rolematch DIR_SSL_ROLEMATCH_SRCS)
aux_source_directory(./ssl/skill DIR_SSL_SKILL_SRCS)
aux_source_directory(./ssl/tinyxml DIR_SSL_TINYXML_SRCS)

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
	file (GLOB LUAMODULEHEADERS "LuaModule/*.h")
	file (GLOB SSLHEADERS "ssl/*.h")
	file (GLOB SSL_BAYES_HEADERS "ssl/bayes/*.h")
	file (GLOB SSL_CMU_HEADERS "ssl/cmu/*.h")
	file (GLOB SSL_CORNELL_HEADERS "ssl/cornell/*.h")
	file (GLOB SSL_DEFENCE_HEADERS "ssl/defence/*.h")
	file (GLOB SSL_ROLEMATCH_HEADERS "ssl/rolematch/*.h")
	file (GLOB SSL_SKILL_HEADERS "ssl/skill/*.h")
	file (GLOB SSL_TINYXML_HEADERS "ssl/tinyxml/*.h")
	source_group("include" FILES ${HEADERS})
	source_group("src" FILES ${SRCS})
	source_group("include\\LuaModule" FILES ${LUAMODULEHEADERS})
	source_group("src\\LuaModule" FILES ${DIR_LUAMODULE_SRCS})
	source_group("include\\ssl" FILES ${SSLHEADERS})
	source_group("src\\ssl" FILES ${DIR_SSL_SRCS})
	source_group("include\\ssl\\bayes" FILES ${SSL_BAYES_HEADERS})
	source_group("src\\ssl\\bayes" FILES ${DIR_SSL_BAYES_SRCS})
	source_group("include\\ssl\\cmu" FILES ${SSL_CMU_HEADERS})
	source_group("src\\ssl\\cmu" FILES ${DIR_SSL_CMU_SRCS})
	source_group("include\\ssl\\cornell" FILES ${SSL_CORNELL_HEADERS})
	source_group("src\\ssl\\cornell" FILES ${DIR_SSL_CORNELL_SRCS})
	source_group("include\\ssl\\defence" FILES ${SSL_DEFENCE_HEADERS})
	source_group("src\\ssl\\defence" FILES ${DIR_SSL_DEFENCE_SRCS})
	source_group("include\\ssl\\rolematch" FILES ${SSL_ROLEMATCH_HEADERS})
	source_group("src\\ssl\\rolematch" FILES ${DIR_SSL_ROLEMATCH_SRCS})
	source_group("include\\ssl\\skill" FILES ${SSL_SKILL_HEADERS})
	source_group("src\\ssl\\skill" FILES ${DIR_SSL_SKILL_SRCS})
	source_group("include\\ssl\\tinyxml" FILES ${SSL_TINYXML_HEADERS})
	source_group("src\\ssl\\tinyxml" FILES ${DIR_SSL_TINYXML_SRCS})
ENDIF()

include_directories(./LuaModule)
include_directories(./ssl)
link_directories(./lib)

# target
# ------
IF(WIN32)
	# windows
	add_library(${PROJECT_NAME} SHARED
			${SRCS}
			${HEADERS}
			${PROTO_SRCS}
			${PROTO_HDRS}
			${DIR_LUAMODULE_SRCS}
			${DIR_SSL_SRCS}
			${DIR_SSL_BAYES_SRCS}
			${DIR_SSL_CMU_SRCS}
			${DIR_SSL_CORNELL_SRCS}
			${DIR_SSL_DEFENCE_SRCS}
			${DIR_SSL_ROLEMATCH_SRCS}
			${DIR_SSL_SKILL_SRCS}
			${DIR_SSL_TINYXML_SRCS}
			${LUAMODULEHEADERS}
			${SSLHEADERS}
			${SSL_BAYES_HEADERS}
			${SSL_CMU_HEADERS}
			${SSL_CORNELL_HEADERS}
			${SSL_DEFENCE_HEADERS}
			${SSL_ROLEMATCH_HEADERS}
			${SSL_SKILL_HEADERS}
			${SSL_TINYXML_HEADERS})
	target_link_libraries(${PROJECT_NAME}
			core
			foundation
			utils
			${LUA_LIBRARIES}
			${Protobuf_LIBRARIES}
			${SQLite3_LIBRARY}
			tolua)
ELSEIF(UNIX)
	IF(APPLE)
        # macOS
		add_library(${PROJECT_NAME} SHARED
				${SRCS}
				${PROTO_SRCS}
				${PROTO_HDRS}
				${DIR_LUAMODULE_SRCS}
				${DIR_SSL_SRCS}
				${DIR_SSL_BAYES_SRCS}
				${DIR_SSL_CMU_SRCS}
				${DIR_SSL_CORNELL_SRCS}
				${DIR_SSL_DEFENCE_SRCS}
				${DIR_SSL_ROLEMATCH_SRCS}
				${DIR_SSL_SKILL_SRCS}
				${DIR_SSL_TINYXML_SRCS})
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
				${PROTO_SRCS}
				${PROTO_HDRS}
				${DIR_LUAMODULE_SRCS}
				${DIR_SSL_SRCS}
				${DIR_SSL_BAYES_SRCS}
				${DIR_SSL_CMU_SRCS}
				${DIR_SSL_CORNELL_SRCS}
				${DIR_SSL_DEFENCE_SRCS}
				${DIR_SSL_ROLEMATCH_SRCS}
				${DIR_SSL_SKILL_SRCS}
				${DIR_SSL_TINYXML_SRCS})
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
