//
// Created by Kevin-Tu on 2019/10/14.
//
#include <string.h>
#include <jni.h>
#include "util/log.h"

/**
 * JNIEXPORT jstring JNICALL
 * 可以省略为： jstring
 *
 * src/main/java/com/kevintu/ndk/jni/HelloJni
 */
JNIEXPORT jstring JNICALL
Java_com_kevintu_ndk_jni_HelloJni_stringFromJNI(JNIEnv* env, jobject thiz)
{
#if defined(__arm__)
    #if defined(__ARM_ARCH_7A__)
        #if defined(__ARM_NEON__)
            #if defined(__ARM_PCS_VFP)
                #define ABI "armeabi-v7a/NEON (hard-float)"
            #else
                #define ABI "armeabi-v7a/NEON"
            #endif
        #else
            #if defined(__ARM_PCS_VFP)
                #define ABI "armeabi-v7a (hard-float)"
            #else
                #define ABI "armeabi-v7a"
            #endif
        #endif
    #else
        #define ABI "armeabi"
    #endif
#elif defined(__i386__)
    #define ABI "x86"
#elif defined(__x86_64__)
    #define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
    #define ABI "mips64"
#elif defined(__mips__)
    #define ABI "mips"
#elif defined(__aarch64__)
    #define ABI "arm64-v8a"
#else
    #define ABI "unknown"
#endif

    LOGD("日志输出： Java_com_kevintu_ndk_jni_HelloJni_stringFromJNI()");
    return (*env)->NewStringUTF(env, "Hello from JNI !  Compiled with ABI " ABI ".");
}

jstring Java_com_kevintu_ndk_jni_HelloJni_stringFromJNI2(JNIEnv* env, jobject thiz)
{
    LOGI("日志输出： Java_com_kevintu_ndk_jni_HelloJni_stringFromJNI2()");
    return (*env)->NewStringUTF(env, "Hello JNI, I am coming !");
}