package com.example.roscontrol;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.example.roscontrol.Menu;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class MainActivity extends AppCompatActivity {

    private Socket client;

    public MainActivity() {
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }


    public void connectServer(View v) throws IOException {
        EditText serInput = (EditText) findViewById(R.id.serverAddInput);
        EditText portInput = (EditText) findViewById(R.id.portInput);
        TextView conMsg = (TextView) findViewById(R.id.connectMsg);
        conMsg.setText("连接中");

        String serverName = serInput.getText().toString();
        String portStr = portInput.getText().toString();
        Integer port = Integer.valueOf(portStr);
        if(port == null) {
            conMsg.setText("获取端口失败:"+ portStr);
            return;
        }
        ((ApplicationUtil)getApplication()).startConnect(serverName, port);
        int time = 0;
        while (!((ApplicationUtil)getApplication()).isConnected()) {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
            }
            time = time +1;
            if (time >20) {
                conMsg.setText("连接超时");
                return;
            }
        }

        conMsg.setText("连接成功");

        startActivity(new Intent(MainActivity.this, Menu.class));
    }

    public void jump(View v) {
        startActivity(new Intent(MainActivity.this, Menu.class));
    }

    public Socket getClient() {
        return client;
    }
}
