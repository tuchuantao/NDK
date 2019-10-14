package com.kevintu.ndk;

import android.databinding.DataBindingUtil;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import com.kevintu.ndk.databinding.ActivityMainBinding;
import com.kevintu.ndk.jni.HelloJni;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = DataBindingUtil.setContentView(this, R.layout.activity_main);

        initView();
    }

    private void initView() {
        binding.btn.setOnClickListener( view -> {
            String jniStr = HelloJni.stringFromJNI();
            binding.resultView.setText(jniStr);
        });

        binding.btn2.setOnClickListener( view -> {
            String jniStr = HelloJni.stringFromJNI2();
            binding.resultView.setText(jniStr);
        });
    }
}
