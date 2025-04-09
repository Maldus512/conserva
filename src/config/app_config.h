#ifndef APP_CONFIG_H_INCLUDED
#define APP_CONFIG_H_INCLUDED

#define STR_VALUE(arg)   #arg
#define INT_TO_STR(name) STR_VALUE(name)

#define APP_CONFIG_SOCKET_NAME_FMT                                                                                     \
    BUILD_PROJECT_NAME "_%i_v" INT_TO_STR(BUILD_VERSION_MAJOR) "." INT_TO_STR(BUILD_VERSION_MINOR) ".sock"
#define APP_CONFIG_SOCKET_PATH_FMT "/tmp/" APP_CONFIG_SOCKET_NAME_FMT

#endif
