package com.example.myapplication;

import android.os.Looper;
import android.widget.Toast;

import java.util.LinkedList;
import java.util.Queue;

public class MsgBuffer {
    Queue<String> sendMsg = new LinkedList<String>();
    Queue<String> recvMsg = new LinkedList<String>();
    public void setSend(String msg) {
        synchronized (sendMsg) {
            sendMsg.add(msg);
            if (sendMsg.size() == 1) {
                sendMsg.notify();
            }
        }
    }

    public String getSend() {
        synchronized (sendMsg) {
            if (sendMsg.size() == 0) {
                try {
                    sendMsg.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            return sendMsg.poll();
        }
    }

    public void setrecv(String msg) {
        synchronized (recvMsg) {
            recvMsg.add(msg);
            if (recvMsg.size() == 1) {
                recvMsg.notify();
            }
        }
    }

    public String getrecv() {
        synchronized (recvMsg) {
            if (recvMsg.size() == 0) {
                try {
                    recvMsg.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            return recvMsg.poll();
        }
    }
}
