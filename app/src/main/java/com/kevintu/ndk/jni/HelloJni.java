package com.kevintu.ndk.jni;

import com.kevintu.ndk.MainActivity;

/**
 * Create by Kevin-Tu on 2019/10/14.
 */
public class HelloJni {

    static {
        System.loadLibrary("hello-jni");
    }

    public static native String stringFromJNI();

    public static native String stringFromJNI2();

    public static native void startTicks(TimerCallback callback);

    public static native void stopTicks();
}
