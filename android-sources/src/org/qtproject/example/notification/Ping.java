package org.qtproject.example.notification;

import java.net.HttpURLConnection;
import java.net.URL;
import java.net.InetSocketAddress;
import java.net.*;

import java.io.*;
import java.net.*;
import java.*;

public class Ping
{
    public static boolean pingHost(String host, Integer port, Integer timeout)
    {
        try
        {
            Socket socket = new Socket();
            socket.connect(new InetSocketAddress(host, port), timeout);
            System.out.println("true");
            return true;
        }
        catch (IOException e)
        {
            System.out.println("false");
            return false; // Either timeout or unreachable or failed DNS lookup.
        }
    }
}
