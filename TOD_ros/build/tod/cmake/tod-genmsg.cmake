# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "tod: 2 messages, 2 services")

set(MSG_I_FLAGS "-Itod:/home/robotxf/TOD_python/TOD_ros/src/tod/msg;-Istd_msgs:/opt/ros/kinetic/share/std_msgs/cmake/../msg;-Isensor_msgs:/opt/ros/kinetic/share/sensor_msgs/cmake/../msg;-Igeometry_msgs:/opt/ros/kinetic/share/geometry_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(geneus REQUIRED)
find_package(genlisp REQUIRED)
find_package(gennodejs REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(tod_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg" NAME_WE)
add_custom_target(_tod_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "tod" "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg" "std_msgs/Float32MultiArray:std_msgs/MultiArrayDimension:std_msgs/MultiArrayLayout"
)

get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv" NAME_WE)
add_custom_target(_tod_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "tod" "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv" ""
)

get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv" NAME_WE)
add_custom_target(_tod_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "tod" "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv" ""
)

get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg" NAME_WE)
add_custom_target(_tod_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "tod" "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg" "std_msgs/MultiArrayDimension:std_msgs/MultiArrayLayout:std_msgs/UInt8MultiArray"
)

#
#  langs = gencpp;geneus;genlisp;gennodejs;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/Float32MultiArray.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg"
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/tod
)
_generate_msg_cpp(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/UInt8MultiArray.msg"
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/tod
)

### Generating Services
_generate_srv_cpp(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/tod
)
_generate_srv_cpp(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/tod
)

### Generating Module File
_generate_module_cpp(tod
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/tod
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(tod_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(tod_generate_messages tod_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg" NAME_WE)
add_dependencies(tod_generate_messages_cpp _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv" NAME_WE)
add_dependencies(tod_generate_messages_cpp _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv" NAME_WE)
add_dependencies(tod_generate_messages_cpp _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg" NAME_WE)
add_dependencies(tod_generate_messages_cpp _tod_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(tod_gencpp)
add_dependencies(tod_gencpp tod_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS tod_generate_messages_cpp)

### Section generating for lang: geneus
### Generating Messages
_generate_msg_eus(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/Float32MultiArray.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg"
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/tod
)
_generate_msg_eus(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/UInt8MultiArray.msg"
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/tod
)

### Generating Services
_generate_srv_eus(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/tod
)
_generate_srv_eus(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/tod
)

### Generating Module File
_generate_module_eus(tod
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/tod
  "${ALL_GEN_OUTPUT_FILES_eus}"
)

add_custom_target(tod_generate_messages_eus
  DEPENDS ${ALL_GEN_OUTPUT_FILES_eus}
)
add_dependencies(tod_generate_messages tod_generate_messages_eus)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg" NAME_WE)
add_dependencies(tod_generate_messages_eus _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv" NAME_WE)
add_dependencies(tod_generate_messages_eus _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv" NAME_WE)
add_dependencies(tod_generate_messages_eus _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg" NAME_WE)
add_dependencies(tod_generate_messages_eus _tod_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(tod_geneus)
add_dependencies(tod_geneus tod_generate_messages_eus)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS tod_generate_messages_eus)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/Float32MultiArray.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg"
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/tod
)
_generate_msg_lisp(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/UInt8MultiArray.msg"
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/tod
)

### Generating Services
_generate_srv_lisp(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/tod
)
_generate_srv_lisp(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/tod
)

### Generating Module File
_generate_module_lisp(tod
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/tod
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(tod_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(tod_generate_messages tod_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg" NAME_WE)
add_dependencies(tod_generate_messages_lisp _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv" NAME_WE)
add_dependencies(tod_generate_messages_lisp _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv" NAME_WE)
add_dependencies(tod_generate_messages_lisp _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg" NAME_WE)
add_dependencies(tod_generate_messages_lisp _tod_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(tod_genlisp)
add_dependencies(tod_genlisp tod_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS tod_generate_messages_lisp)

### Section generating for lang: gennodejs
### Generating Messages
_generate_msg_nodejs(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/Float32MultiArray.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg"
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/tod
)
_generate_msg_nodejs(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/UInt8MultiArray.msg"
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/tod
)

### Generating Services
_generate_srv_nodejs(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/tod
)
_generate_srv_nodejs(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/tod
)

### Generating Module File
_generate_module_nodejs(tod
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/tod
  "${ALL_GEN_OUTPUT_FILES_nodejs}"
)

add_custom_target(tod_generate_messages_nodejs
  DEPENDS ${ALL_GEN_OUTPUT_FILES_nodejs}
)
add_dependencies(tod_generate_messages tod_generate_messages_nodejs)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg" NAME_WE)
add_dependencies(tod_generate_messages_nodejs _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv" NAME_WE)
add_dependencies(tod_generate_messages_nodejs _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv" NAME_WE)
add_dependencies(tod_generate_messages_nodejs _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg" NAME_WE)
add_dependencies(tod_generate_messages_nodejs _tod_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(tod_gennodejs)
add_dependencies(tod_gennodejs tod_generate_messages_nodejs)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS tod_generate_messages_nodejs)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/Float32MultiArray.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg"
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/tod
)
_generate_msg_py(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayDimension.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/MultiArrayLayout.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/UInt8MultiArray.msg"
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/tod
)

### Generating Services
_generate_srv_py(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/tod
)
_generate_srv_py(tod
  "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/tod
)

### Generating Module File
_generate_module_py(tod
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/tod
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(tod_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(tod_generate_messages tod_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/CoData.msg" NAME_WE)
add_dependencies(tod_generate_messages_py _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/ReqImage.srv" NAME_WE)
add_dependencies(tod_generate_messages_py _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/srv/StartEval.srv" NAME_WE)
add_dependencies(tod_generate_messages_py _tod_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/robotxf/TOD_python/TOD_ros/src/tod/msg/IMG.msg" NAME_WE)
add_dependencies(tod_generate_messages_py _tod_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(tod_genpy)
add_dependencies(tod_genpy tod_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS tod_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/tod)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/tod
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_cpp)
  add_dependencies(tod_generate_messages_cpp std_msgs_generate_messages_cpp)
endif()
if(TARGET sensor_msgs_generate_messages_cpp)
  add_dependencies(tod_generate_messages_cpp sensor_msgs_generate_messages_cpp)
endif()

if(geneus_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/tod)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/tod
    DESTINATION ${geneus_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_eus)
  add_dependencies(tod_generate_messages_eus std_msgs_generate_messages_eus)
endif()
if(TARGET sensor_msgs_generate_messages_eus)
  add_dependencies(tod_generate_messages_eus sensor_msgs_generate_messages_eus)
endif()

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/tod)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/tod
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_lisp)
  add_dependencies(tod_generate_messages_lisp std_msgs_generate_messages_lisp)
endif()
if(TARGET sensor_msgs_generate_messages_lisp)
  add_dependencies(tod_generate_messages_lisp sensor_msgs_generate_messages_lisp)
endif()

if(gennodejs_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/tod)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/tod
    DESTINATION ${gennodejs_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_nodejs)
  add_dependencies(tod_generate_messages_nodejs std_msgs_generate_messages_nodejs)
endif()
if(TARGET sensor_msgs_generate_messages_nodejs)
  add_dependencies(tod_generate_messages_nodejs sensor_msgs_generate_messages_nodejs)
endif()

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/tod)
  install(CODE "execute_process(COMMAND \"/usr/bin/python2\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/tod\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/tod
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_py)
  add_dependencies(tod_generate_messages_py std_msgs_generate_messages_py)
endif()
if(TARGET sensor_msgs_generate_messages_py)
  add_dependencies(tod_generate_messages_py sensor_msgs_generate_messages_py)
endif()
