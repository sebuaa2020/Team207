package com.example.roscontrol;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import java.io.IOException;

import static java.lang.Integer.parseInt;

public class Navga extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_navga);
    }

    public void sendMsg(View v) throws IOException {
        final EditText x = (EditText) findViewById(R.id.xval);
        final EditText y = (EditText) findViewById(R.id.yval);
        final EditText dire = (EditText) findViewById(R.id.direction);
        String zval = dire.getText().toString();
        double num = Double.parseDouble(zval);
        num = num % 6.28;
        zval = String.valueOf(num);

        String res = new String("3 ");
        res.concat(x.getText().toString());
        res.concat(y.getText().toString());
        res.concat(zval);
        ((ApplicationUtil)getApplication()).send(res);
    }

    public void end(View v) throws IOException {
        String s = new String("3 0 0 -1");
        ((ApplicationUtil)getApplication()).send(s);
    }
}
