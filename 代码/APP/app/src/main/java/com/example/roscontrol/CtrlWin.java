package com.example.roscontrol;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import java.io.IOException;

public class CtrlWin extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ctrl_win);
        //Intent intent = getIntent();
    }

    public void forward(View v) throws IOException {
        String up = new String("2 1");
        ((ApplicationUtil)getApplication()).send(up);
    }

    public void backward(View v) throws IOException {
        String down = new String("2 2");
        ((ApplicationUtil)getApplication()).send(down);
    }

    public void turnleft(View v) throws IOException {
        String left = new String("2 3");
        ((ApplicationUtil)getApplication()).send(left);
    }

    public void turnright(View v) throws IOException {
        String right = new String("2 4");
        ((ApplicationUtil)getApplication()).send(right);
    }

    public void rotateleft(View v) throws IOException {
        String left = new String("2 6");
        ((ApplicationUtil)getApplication()).send(left);
    }

    public void rotateright(View v) throws IOException {
        String right = new String("2 5");
        ((ApplicationUtil)getApplication()).send(right);
    }
}
