package com.kevintu.ndk.jni;

/**
 * Create by Kevin-Tu on 2019/10/14.
 */
public class HelloJni {

    static {
        System.loadLibrary("hello-jni");
    }

    public static native String stringFromJNI();

    public static native String stringFromJNI2();

    //public native String unimplementedStringFromJNI();
}
