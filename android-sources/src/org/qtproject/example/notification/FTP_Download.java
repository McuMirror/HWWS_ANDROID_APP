/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         FTP_Download.java
// Zweck:            Download File from Server
//
// erstellt am:      18.06.2015
// zuletzt geändert: 16.03.2016
// ****************************************************************************
*/

package org.qtproject.example.notification;

import android.os.AsyncTask;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

public class FTP_Download extends AsyncTask<String, String, Long>
{
    private static final int BUFFER_SIZE = 4096;
    String SaveFileName;

    protected Long doInBackground(String ...ftpUrl)
    {
        /*
        String ftpUrl = "ftp://%s:%s@%s/%s";
        String host = "oteloserver.ddns.net/hwws";
        String user = "anonymous";
        String pass = "password";
        String filePath = filename[0].toString();
        ftpUrl = String.format(ftpUrl, user, pass, host, filePath);
        */
        String savePath = "/storage/emulated/0/HWWS/Daten/" + SaveFileName;

        System.out.println("URL: " + ftpUrl[0]);

        try
        {
            URL url = new URL(ftpUrl[0]);

            URLConnection conn = url.openConnection();
            InputStream inputStream = conn.getInputStream();

            FileOutputStream outputStream = new FileOutputStream(savePath);

            byte[] buffer = new byte[BUFFER_SIZE];
            int bytesRead = -1;
            while ((bytesRead = inputStream.read(buffer)) != -1)
            {
                outputStream.write(buffer, 0, bytesRead);
            }

            outputStream.close();
            inputStream.close();

            System.out.println("File downloaded");

        }
        catch (IOException ex)
        {
            ex.printStackTrace();
        }
        long a = 0;
        return a;
    }

    public void setSaveFileName(String filename)
    {
        SaveFileName = filename;
    }
}
