cmake_minimum_required(VERSION 3.4.1)

add_library(
    hello-jni # so库的名称 libhello-jni.so
    SHARED # 设置为动态库  STATIC/SHARED
    jnicallback.c # 指定C源文件的路径    // jnicallback.c中已经引用了 hello-jni.c，所以不用在CMake中指定
#    hello-jni.c jnicallback.c # 指定C源文件的路径
)

find_library(
        log-lib # 设置路径变量名称
        log # 指定CMake需要加载的NDK库
)

# 链接hello-jni库依赖的库，注意下面变量名的配置
target_link_libraries(
        hello-jni
        android
        log
)