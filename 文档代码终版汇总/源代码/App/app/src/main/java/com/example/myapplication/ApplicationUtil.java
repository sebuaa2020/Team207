package com.example.myapplication;

import android.app.Application;
import android.os.Looper;
import android.widget.Toast;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class ApplicationUtil extends Application {
    //缓存接收、发送消息的仓库
    private MsgBuffer msgBuffer = new MsgBuffer();
        //管理socket连接的线程
        private class SocketConnect extends Thread {
            private Socket client;
            String host;
            int port;
            private BufferedReader receiver;
            private PrintWriter sender;

            public SocketConnect(String host, int port) {
                this.host = host;
                this.port = port;
            }

            public void run() {
                //建立socket连接
                try {
                    client = new Socket(host, port);
                    receiver = new BufferedReader(new InputStreamReader(client.getInputStream()));
                    sender = new PrintWriter(new OutputStreamWriter(client.getOutputStream()));
                } catch (IOException e) {
                    CrashHandler.getInstance().uncaughtException(this, e);
                }
                //开启接收用子线程
                new Thread() {
                    @Override
                    public void run() {
                        String msg = null;
                        while(client.isConnected()) {
                            try {
                                msg = receiver.readLine();
                            } catch (IOException e) {
                                CrashHandler.getInstance().uncaughtException(this, e);
                            }
                            final String s = msg;
                            new Thread() {
                                public void run() {
                                    Looper.prepare();
                                    Toast.makeText(getApplicationContext(), "socket recieve： " + s, Toast.LENGTH_LONG).show();
                                    Looper.loop();
                                }
                            }.start();
                            if (msg != null) {
                                msgBuffer.setrecv(msg);
                                msg = null;
                            }
                        }
                    }
                }.start();
                //在本线程进行发送
                while(client.isConnected()) {
                    String msg = null;
                    msg = msgBuffer.getSend();
                    final String s = msg;
                    new Thread() {
                        public void run() {
                            Looper.prepare();
                            Toast.makeText(getApplicationContext(), "socket send： "+s, Toast.LENGTH_LONG).show();
                            Looper.loop();
                        }
                    }.start();
                    if (msg != null) {
                        sender.println(msg);
                        sender.flush();
                        msg = null;
                    }
                }
                //连接断开时，线程将自动停止
            }

            public boolean isConnected() {
                return (client != null) && client.isConnected();
            }
        }
    private SocketConnect scThread;

    @Override
    public void onCreate() {
        super.onCreate();
        CrashHandler mException=CrashHandler.getInstance();
        mException.init(getApplicationContext());  //注册全局异常捕捉
    }

    public void startConnect(String host, int port) throws IOException {
        //开始新socket连接线程
        scThread = new SocketConnect(host, port);
        scThread.start();
    }

    public void send(String cmd) throws IOException {
        msgBuffer.setSend(cmd);
    }

    public String receive() throws IOException {
        return msgBuffer.getrecv();

    }

    public boolean isConnected() {
        return scThread.isConnected();
    }


}
