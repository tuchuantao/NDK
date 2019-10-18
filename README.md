[Google NDK帮助文档](https://developer.android.com/ndk)<br/>
[Google NDK sample](https://github.com/android/ndk-samples) <br/>
[https://www.jianshu.com/p/87ce6f565d37](https://www.jianshu.com/p/87ce6f565d37)<br/>

# 一、NDK （Native Develop Kit）
Android NDK 是一套允许您使用 C 和 C++ 等语言，以原生代码实现部分应用的工具集。在开发某些类型的应用时，这有助于您重复使用以这些语言编写的代码库。并提供众多[平台库](https://developer.android.com/ndk/guides/stable_apis.html)，您可使用这些平台库管理原生 Activity 和访问物理设备组件，例如传感器和轻触输入。

## 1.1 作用
* 在平台之间移植其应用。
* 重复使用现有库，或者提供其自己的库供重复使用。
* 进一步提升设备性能，以降低延迟，特别是像游戏这种运行计算密集型应用。
* 代码保护。由于apk的java层代码很容易被反编译，而C/C++库反汇难度较大。
## 1.2 android框架
![图片](https://uploader.shimo.im/f/LgBF5FX07c06o4sG.jpg!thumbnail)

# 二、JNI（Java Native Interface）
JNI，全称为Java Native Interface，即Java本地接口，JNI是Java调用Native 语言的一种特性。通过JNI可以使得Java与C/C++机型交互。即可以在Java代码中调用C/C++等语言的代码或者在C/C++代码中调用Java代码。由于JNI是JVM规范的一部分，因此可以将我们写的JNI的程序在任何实现了JNI规范的Java虚拟机中运行。同时，这个特性使我们可以复用以前用C/C++写的大量代码。JNI是一种在Java虚拟机机制下的执行代码的标准机制。

## 2.1 JNI的限制
开发JNI程序会受到系统环境限制，因为用C/C++ 语言写出来的代码或模块，**编译过程当中要依赖当前操作系统环境所提供的一些库函数，并和本地库链接在一起**。而且编译后生成的二进制代码只能在本地操作系统环境下运行，因为不同的操作系统环境，有自己的本地库和CPU指令集，而且各个平台对标准C/C++的规范和标准库函数实现方式也有所区别。这就造成了各个平台使用JNI接口的Java程序，不再像以前那样自由的跨平台。如果要实现跨平台， 就必须将本地代码在不同的操作系统平台下编译出相应的动态库。

## 2.2 ABI管理
不同的 Android 手机使用不同的 CPU，而不同的 CPU 支持不同的指令集。CPU 与指令集的每种组合都有专属的应用二进制接口，即 ABI。ABI 可以非常精确地定义应用的机器代码在运行时如何与系统交互。您必须为应用要使用的每个 CPU 架构指定 ABI。

```
android {
  ...
  defaultConfig {
  
    // 设置ABI / cpu
    ndk {
      // Specifies the ABI configurations of your native
      // libraries Gradle should build and package with your APK.
      abiFilters 'armeabi-v7a', 'x86' //, armeabi, arm64-v8a, x86, x86_64
    }
  }
}
```
[https://developer.android.com/ndk/guides/abis.html](https://developer.android.com/ndk/guides/abis.html)
## 2.3 JNI的优势
JNI的主要竞争优势在于：它在设计之初就确保了二进制的兼容性，JNI编写的应用程序兼容性以及其在某些具体平台上的Java虚拟机兼容性(当谈及JNI时，这里并不特比针对Davik虚拟机，JNI适用于所有JVM虚拟机)。

### 2.3.1 二进制兼容性：
指的就是在升级（也可能是 bug fix）库文件的时候，不必重新编译使用这个库的可执行文件或使用这个库的其他库文件，程序的功能不被破坏。

## 2.4 JNI的流程
### 2.4.1 JNI的三个角色
JNI下一共涉及到三个角色：

C/C++代码、

本地方法接口类、

Java层中具体业务类。

![图片](https://uploader.shimo.im/f/ZlDRTaj7sy0BvNZF.jpg!thumbnail)

### 2.4.2 JNI的简要流程
![图片](https://uploader.shimo.im/f/cnpZTv9XZzcQ3OSJ.jpg!thumbnail)

## 2.5 JNI的数据类型及签名
**1、基本数据类型：**

![图片](https://uploader.shimo.im/f/HQS3k4cWXE4NgFoQ.png!thumbnail)

**2、引用数据类型：**

![图片](https://uploader.shimo.im/f/epwjasuWS30xffnm.png!thumbnail)

## 2.6 Java与Native相互调用
[https://www.jianshu.com/p/b71aeb4ed13d](https://www.jianshu.com/p/b71aeb4ed13d)

[https://www.jianshu.com/p/0f34c097028a](https://www.jianshu.com/p/0f34c097028a)

动态注册 ：  JNI_OnLoad     RegisterNatives（）

```
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){

    __android_log_print(ANDROID_LOG_DEBUG,"JNITag","enter jni_onload");

    JNIEnv* env = NULL;
    jint result = -1;

    if((*vm)->GetEnv(vm,(void**)&env,JNI_VERSION_1_6)!= JNI_OK){
        return result;
    }

    //注册四个方法，注意签名
    const JNINativeMethod method[]={
            {"add","(II)I",(void*)addNumber},
            {"sub","(II)I",(void*)subNumber},
            {"mul","(II)I",(void*)mulNumber},
            {"div","(II)I",(void*)divNumber}
    };
    //找到对应的JNITools类
    jclass jClassName=(*env)->FindClass(env,"com/gebilaolitou/jni/JNITools");
    //开始注册
    jint ret = (*env)->RegisterNatives(env,jClassName,method, 4);

    if (ret != JNI_OK) {
        __android_log_print(ANDROID_LOG_DEBUG, "JNITag", "jni_register Error");
        return -1;
    }

    return JNI_VERSION_1_6;
}
```



# 三、Tools
## 3.1 CMake
CMake 是一个跨平台、开源的构建系统。它是一个集软件构建、测试、打包于一身的软件。它使用与平台和编译器独立的配置文件来对软件编译过程进行控制。

1、app/build.gradle

```
android {
  ...
  externalNativeBuild {
      cmake {
          version '3.10.2'
          path "src/main/cpp/CMakeLists.txt"
      }
  }
}
```
2、CMakeLists.txt
```
cmake_minimum_required(VERSION 3.4.1)
add_library(
    hello-jni # so库的名称 libhello-jni.so
    SHARED # 设置为分享库
    hello-jni.c jnicallback.c #指定C源文件的路径
)
#find_library(
#        log-lib # 设置路径变量名称
#        log # 指定CMake需要加载的NDK库
#)
# 链接hello-jni库依赖的库，注意下面变量名的配置
target_link_libraries(
        hello-jni
        android
        log
)
```

参考：

[https://developer.android.com/ndk/guides/cmake](https://developer.android.com/ndk/guides/cmake)
[https://blog.csdn.net/afei__/article/details/81201039](https://blog.csdn.net/afei__/article/details/81201039)

[https://blog.csdn.net/z_h_s/article/details/50699905](https://blog.csdn.net/z_h_s/article/details/50699905)




1、 在 C 语言中，“签名”只表示方法名称。在 C++ 中，“签名”不仅表示方法名称，还表示其参数名称和类型。


