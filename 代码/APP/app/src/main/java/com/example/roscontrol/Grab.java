package com.example.roscontrol;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Spinner;

import java.io.IOException;

public class Grab extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_grab);
    }

    public void sendMsg(View v) throws IOException {
        Spinner colorsp = (Spinner) findViewById(R.id.color);
        colorsp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                final String s1 = parent.getItemAtPosition(position).toString();
                Spinner sizesp = (Spinner) findViewById(R.id.size);
                sizesp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                    @Override
                    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                        final String s2 = parent.getItemAtPosition(position).toString();
                        Spinner possp = (Spinner) findViewById(R.id.pos);
                        possp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                            @Override
                            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                                String s3 = parent.getItemAtPosition(position).toString();
                                String grabmsg = new String("4 ");
                                grabmsg.concat(s1);
                                grabmsg.concat(s2);
                                grabmsg.concat(s3);
                                try {
                                    ((ApplicationUtil)getApplication()).send(grabmsg);
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }

                            @Override
                            public void onNothingSelected(AdapterView<?> parent) {

                            }
                        });
                    }

                    @Override
                    public void onNothingSelected(AdapterView<?> parent) {

                    }
                });
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
    }
}
