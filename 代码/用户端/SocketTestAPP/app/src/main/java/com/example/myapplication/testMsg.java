package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;

public class testMsg extends AppCompatActivity {

    private class ReceiveThread extends Thread {
        @Override
        public void run() {
            String buffer;
            while (true) {
                try {
                    buffer = ((ApplicationUtil)getApplication()).receive();
                } catch (IOException e) {
                    buffer = new String("接受出错");
                }
                TextView board = (TextView) findViewById(R.id.msgToReceive);
                board.setText(buffer);
                try {
                    sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private ReceiveThread rt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_msg);

        rt = new ReceiveThread();
        rt.start();

    }

    public void sendMsg(View v) throws IOException {
        EditText msg = (EditText) findViewById(R.id.msgToSend);
        Toast.makeText(getApplicationContext(),"send:" + msg.getText().toString(), Toast.LENGTH_LONG).show();
        ((ApplicationUtil)getApplication()).send(msg.getText().toString());
    }



}
