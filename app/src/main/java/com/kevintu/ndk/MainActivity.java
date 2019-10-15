package com.kevintu.ndk;

import android.databinding.DataBindingUtil;
import android.support.annotation.Keep;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.kevintu.ndk.databinding.ActivityMainBinding;
import com.kevintu.ndk.jni.HelloJni;
import com.kevintu.ndk.jni.TimerCallback;

public class MainActivity extends AppCompatActivity implements TimerCallback {

    private ActivityMainBinding binding;

    int hour = 0;
    int minute = 0;
    int second = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = DataBindingUtil.setContentView(this, R.layout.activity_main);

        initView();
    }

    private void initView() {
        binding.btn.setOnClickListener(view -> {
            String jniStr = HelloJni.stringFromJNI();
            binding.resultView.setText(jniStr);
        });

        binding.btn2.setOnClickListener(view -> {
            String jniStr = HelloJni.stringFromJNI2();
            binding.resultView.setText(jniStr);
        });

        binding.btn3.setOnClickListener(view -> {
            HelloJni.startTicks(this);
        });

        binding.btn4.setOnClickListener(view -> {
            HelloJni.stopTicks();
        });
    }

    @Keep
    @Override
    public void updateTimer() {
        ++second;
        if (second >= 60) {
            ++minute;
            second -= 60;
            if (minute >= 60) {
                ++hour;
                minute -= 60;
            }
        }

        runOnUiThread(() -> {
            String ticks = MainActivity.this.hour + ":" +
                    MainActivity.this.minute + ":" +
                    MainActivity.this.second;

            MainActivity.this.binding.timerView.setText(ticks);
        });
    }
}
