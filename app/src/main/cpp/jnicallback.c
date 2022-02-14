//
// Created by Kevin-Tu on 2019/10/15.
//
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <jni.h>
#include "util/log.h"
#include "hello-jni.c"
#include <assert.h>


typedef struct tick_context {
    JavaVM *javaVM;
    jclass jniHelperClz;
    jobject jniHelperObj;
    jclass mainActivityClz;
    jobject mainActivityObj;
    pthread_mutex_t lock;
    int done;
} TickContext;
TickContext g_ctx;


/*
 *  A helper function to show how to call
 *     java static functions JniHelper::getBuildVersion()
 *     java non-static function JniHelper::getRuntimeMemorySize()
 *  The trivial implementation for these functions are inside file
 *     JniHelper.java
 */
void queryRuntimeInfo(JNIEnv* env, jobject thiz) {
    // Find out which OS we are running on. It does not matter for this app
    // just to demo how to call static functions.
    // Our java JniHelper class id and instance are initialized when this
    // shared lib got loaded, we just directly use them
    //    static function does not need instance, so we just need to feed
    //    class and method id to JNI
    jmethodID versionFunc = (*env)->GetStaticMethodID(
            env, g_ctx.jniHelperClz,
            "getBuildVersion", "()Ljava/lang/String;"
    );

    if (!versionFunc) {
        LOGE("Failed to retrieve getBuildVersion() methodID @ line %d",
             __LINE__);
        return;
    }
    jstring buildVersion = (*env)->CallStaticObjectMethod(env, g_ctx.jniHelperClz, versionFunc);

    const char *version = (*env)->GetStringUTFChars(env, buildVersion, NULL);
    if (!version) {
        LOGE("Unable to get version string @ line %d", __LINE__);
        return;
    }
    LOGI("Android Version - %s", version);
    (*env)->ReleaseStringUTFChars(env, buildVersion, version);

    // we are called from JNI_OnLoad, so got to release LocalRef to avoid leaking
    (*env)->DeleteLocalRef(env, buildVersion);

    // Query available memory size from a non-static public function
    // we need use an instance of JniHelper class to call JNI
    jmethodID memFunc = (*env)->GetMethodID(env, g_ctx.jniHelperClz,
                                            "getRuntimeMemorySize", "()J");
    if (!memFunc) {
        LOGE("Failed to retrieve getRuntimeMemorySize() methodID @ line %d",
             __LINE__);
        return;
    }
    jlong result = (*env)->CallLongMethod(env, thiz, memFunc);
    LOGI("Runtime free memory size: %" PRId64, result);
    (void)result;  // silence the compiler warning
}

void handler(int signum) {
    LOGD("singnum = %d", signum);
}

void my_singal_handler(int signum, siginfo_t *info, void *reserved) {
    LOGI("signum: %d", signum);

    // 创建子线程上报crash
    // 在crash发生的线程上报，部分机型会有回调不到Java层的情况
//    pthread_create(&my_report_pthread, NULL, threadReport, &signum);

    //调用原先的处理函数
//    old_signalhandlers[signum].sa_handler(signum);
}

void set_up_global_signal_handler() {
    struct sigaction handler;
    memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_sigaction = my_singal_handler;
    handler.sa_flags = SA_RESETHAND;

    //register signal num
    #define CATCH_SIG_NUM(NUM) sigaction(NUM, &handler, NULL)

    CATCH_SIG_NUM(SIGQUIT);
    CATCH_SIG_NUM(SIGILL);
    CATCH_SIG_NUM(SIGABRT);
    CATCH_SIG_NUM(SIGBUS);
    CATCH_SIG_NUM(SIGFPE);
    CATCH_SIG_NUM(SIGSEGV);
    CATCH_SIG_NUM(SIGPIPE);
    CATCH_SIG_NUM(SIGSTKFLT);
    CATCH_SIG_NUM(SIGTERM);

    #undef CATCH_SIG_NUM
}

/*
 * processing one time initialization:
 *     Cache the javaVM into our context
 *     Find class ID for JniHelper
 *     Create an instance of JniHelper
 *     Make global reference since we are using them from a native thread
 * Note:
 *     All resources allocated here are never released by application
 *     we rely on system to free all global refs when it goes away;
 *     the pairing function JNI_OnUnload() never gets called at all.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    memset(&g_ctx, 0, sizeof(g_ctx));

    g_ctx.javaVM = vm;
    //if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {}
    if ((*vm) -> GetEnv(vm, (void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR; // JNI version not supported.
    }

    // Note JNI调用Java类方法的实现步骤：
    // 1、通过类的具体路径获得类对象
    jclass clz = (*env)->FindClass(env, "com/kevintu/ndk/jni/callback/JniHelper");
    g_ctx.jniHelperClz = (*env)->NewGlobalRef(env, clz);

    // 2、获取构造方法 / 普通方法 / 属性
    jmethodID  jniHelperCtor = (*env)->GetMethodID(env, g_ctx.jniHelperClz, "<init>", "()V");

    // 3、通过类对象和构造方法，生成对象
    jobject helper = (*env)->NewObject(env, g_ctx.jniHelperClz, jniHelperCtor);
    g_ctx.jniHelperObj = (*env)->NewGlobalRef(env, helper);

    queryRuntimeInfo(env, g_ctx.jniHelperObj);

    g_ctx.done = 0;
    g_ctx.mainActivityObj = NULL;


    // 动态注册方法
    const JNINativeMethod methods[] = {
            {"stringFromJNI3", "()Ljava/lang/String;", (jstring*)stringFromJNI3} //
            // Ljava/lang/String; 注意后面带的冒号
    };
    if (registerMethods(env, "com/kevintu/ndk/jni/HelloJni", methods, sizeof(methods) / sizeof
    (JNINativeMethod)) == JNI_OK) {
        LOGD("日志输出： 动态注册成功");
    } else {
        LOGD("日志输出： 动态注册失败");
    }

    // 全局捕获 Native 异常
    // signal
//    signal(SIGQUIT, &handler);
//    signal(SIGILL, &handler);
//    signal(SIGABRT, &handler);
//    signal(SIGIOT, &handler);
//    signal(SIGBUS, &handler);
//    signal(SIGFPE, &handler);
//    signal(SIGSEGV, &handler);
//    signal(SIGPIPE, &handler);
//    signal(SIGSTKFLT, &handler);
//    signal(SIGTERM, &handler);


//signal(SIGHUP, &handler);
//signal(SIGINT, &handler);
//signal(SIGQUIT, &handler);
//signal(SIGILL, &handler);
//signal(SIGTRAP, &handler);
//signal(SIGABRT, &handler);
//signal(SIGIOT, &handler);
//signal(SIGBUS, &handler);
//signal(SIGFPE, &handler);
//signal(SIGKILL, &handler);
//signal(SIGUSR1, &handler);
//signal(SIGSEGV, &handler);
//signal(SIGUSR2, &handler);
//signal(SIGPIPE, &handler);
//signal(SIGALRM, &handler);
//signal(SIGTERM, &handler);
//signal(SIGSTKFLT, &handler);
//signal(SIGCHLD, &handler);
//signal(SIGCONT, &handler);
//signal(SIGSTOP, &handler);
//signal(SIGTSTP, &handler);
//signal(SIGTTIN, &handler);
//signal(SIGTTOU, &handler);
//signal(SIGURG, &handler);
//signal(SIGXCPU, &handler);
//signal(SIGXFSZ, &handler);
//signal(SIGVTALRM, &handler);
//signal(SIGPROF, &handler);
//signal(SIGWINCH, &handler);
//signal(SIGIO, &handler);
//signal(SIGPOLL, &handler);
//signal(SIGPWR, &handler);
//signal(SIGSYS, &handler);
//signal(SIGUNUSED, &handler);
//signal(__SIGRTMIN, &handler);

//    set_up_global_signal_handler();

    return JNI_VERSION_1_6;
}

/*
 * A helper function to wrap java JniHelper::updateStatus(String msg)
 * JNI allow us to call this function via an instance even it is
 * private function.
 */
void   sendJavaMsg(JNIEnv *env, jobject instance,
                   jmethodID func,const char* msg) {
    jstring javaMsg = (*env)->NewStringUTF(env, msg);
    (*env)->CallVoidMethod(env, instance, func, javaMsg);
    (*env)->DeleteLocalRef(env, javaMsg);
}

void* UpdateTicks(void* context) {
    TickContext *pctx = (TickContext*) context;
    JavaVM *javaVM = pctx->javaVM;
    JNIEnv *env;

    jint res = (*javaVM)->GetEnv(javaVM, (void**)&env, JNI_VERSION_1_6);
    if (res != JNI_OK) {
        res = (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);
        if (JNI_OK != res) {
            LOGE("Failed to AttachCurrentThread, ErrorCode = %d", res);
            return NULL;
        }
    }

    jmethodID statusId = (*env)->GetMethodID(env, pctx->jniHelperClz,
                                             "updateStatus",
                                             "(Ljava/lang/String;)V");
    sendJavaMsg(env, pctx->jniHelperObj, statusId,
                "TickerThread status: initializing...");

    // get mainActivity updateTimer function
    jmethodID timerId = (*env)->GetMethodID(env, pctx->mainActivityClz,
                                            "updateTimer", "()V");

    struct timeval beginTime, curTime, usedTime, leftTime;
    const struct timeval kOneSecond = {
            (__kernel_time_t)1,
            (__kernel_suseconds_t) 0
    };

    sendJavaMsg(env, pctx->jniHelperObj, statusId,
                "TickerThread status: start ticking ...");

    while(1) {
        gettimeofday(&beginTime, NULL);

        pthread_mutex_lock(&pctx->lock);
        int done = pctx->done;
        if (pctx->done) {
            pctx->done = 0;
        }
        pthread_mutex_unlock(&pctx->lock);

        if (done) {
            break;
        }

        (*env)->CallVoidMethod(env, pctx->mainActivityObj, timerId);

        gettimeofday(&curTime, NULL);
        timersub(&curTime, &beginTime, &usedTime);
        timersub(&kOneSecond, &usedTime, &leftTime);
        struct timespec sleepTime;
        sleepTime.tv_sec = leftTime.tv_sec;
        sleepTime.tv_nsec = leftTime.tv_usec * 1000;

        if (sleepTime.tv_sec <= 1) {
            nanosleep(&sleepTime, NULL);
        } else {
            sendJavaMsg(env, pctx->jniHelperObj, statusId,
                        "TickerThread error: processing too long!");
        }
    }

    sendJavaMsg(env, pctx->jniHelperObj, statusId,
                "TickerThread status: ticking stopped");
    (*env)->DeleteWeakGlobalRef(env, statusId);
    (*env)->DeleteWeakGlobalRef(env, timerId);
    (*javaVM)->DetachCurrentThread(javaVM);
    return context;
}

/**
 *
 * @param env
 * @param thiz 第二个参数 jobject是HelloJni这个对象
 * @param callback native方法传入Java对象，通过该对象进行回调
 */
void Java_com_kevintu_ndk_jni_HelloJni_startTicks(JNIEnv *env, jobject thiz, jobject callback) {
    pthread_t      threadInfo_;
    pthread_attr_t threadAttr_;

    pthread_attr_init(&threadAttr_);
    pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED);

    pthread_mutex_init(&g_ctx.lock, NULL);

    jclass clz = (*env)->GetObjectClass(env, callback);
    g_ctx.mainActivityClz = (*env)->NewGlobalRef(env, clz);
    g_ctx.mainActivityObj = (*env)->NewGlobalRef(env, callback);

    int result = pthread_create(&threadInfo_, &threadAttr_, UpdateTicks, &g_ctx);
    assert(result == 0);

    pthread_attr_destroy(&threadAttr_);

    (void)result;
}

void Java_com_kevintu_ndk_jni_HelloJni_stopTicks(JNIEnv *env, jobject thiz) {
    pthread_mutex_lock(&g_ctx.lock);
    g_ctx.done = 1;
    pthread_mutex_unlock(&g_ctx.lock);

    // waiting for ticking thread to flip the done flag
    struct timespec sleepTime;
    memset(&sleepTime, 0, sizeof(sleepTime));
    sleepTime.tv_nsec = 100000000;
    while (g_ctx.done) {
        nanosleep(&sleepTime, NULL);
    }

    // release object we allocated from StartTicks() function
    (*env)->DeleteGlobalRef(env, g_ctx.mainActivityClz);
    (*env)->DeleteGlobalRef(env, g_ctx.mainActivityObj);
    g_ctx.mainActivityObj = NULL;
    g_ctx.mainActivityClz = NULL;

    pthread_mutex_destroy(&g_ctx.lock);
}
