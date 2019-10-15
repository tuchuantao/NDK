package com.kevintu.ndk.jni.callback;

import android.os.Build;
import android.support.annotation.Keep;
import android.util.Log;

/**
 * Create by Kevin-Tu on 2019/10/15.
 */
public class JniHelper {

    /*
     * Print out status to logcat
     */
    @Keep
    private void updateStatus(String msg) {
        if (msg.toLowerCase().contains("error")) {
            Log.e("JniHelper", "Native Err: " + msg);
        } else {
            Log.i("JniHelper", "Native Msg: " + msg);
        }
    }

    /*
     * Return OS build version: a static function
     */
    @Keep
    static public String getBuildVersion() {
        return Build.VERSION.RELEASE;
    }

    /*
     * Return Java memory info
     */
    @Keep
    public long getRuntimeMemorySize() {
        return Runtime.getRuntime().freeMemory();
    }
}
