package com.example.myapplication;
import android.os.Looper;
import android.widget.Toast;

import android.content.Context;

import androidx.annotation.NonNull;

import java.io.PrintWriter;
import java.io.StringWriter;


public class CrashHandler implements Thread.UncaughtExceptionHandler {
    public static final String TAG = "CrashHandler";
    private static CrashHandler INSTANCE = new CrashHandler();
    private Context mContext;

    private Thread.UncaughtExceptionHandler mDefaultHandler;

    private CrashHandler() {
    }

    public static CrashHandler getInstance() {
        return INSTANCE;
    }

    public void init(Context context) {
        mContext = context;
        mDefaultHandler = Thread.getDefaultUncaughtExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler(this);
    }

    @Override
    public void uncaughtException(@NonNull Thread t, @NonNull Throwable e) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        e.printStackTrace(pw);
        final String msg=sw.toString();
        new Thread() {
            public void run() {
                // Toast 显示需要出现在一个线程的消息队列中
                Looper.prepare();
                Toast.makeText(mContext, "程序出错:" + msg.toString(), Toast.LENGTH_LONG).show();
                Looper.loop();
                //将异常记录到本地的数据库或者文件中.或者直接提交到后台服务器
            }
        }.start();
    }
}

