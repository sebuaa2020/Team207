package com.example.roscontrol;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class Menu extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_menu);
    }

    public void toCtrl(View view){
        Intent intent = new Intent(this, CtrlWin.class);
        startActivity(intent);
    }

    public void Nav(View view){
        Intent intent = new Intent(this, Navga.class);
        startActivity(intent);
    }

    public void Gra(View view){
        Intent intent = new Intent(this, Grab.class);
        startActivity(intent);
    }
}
