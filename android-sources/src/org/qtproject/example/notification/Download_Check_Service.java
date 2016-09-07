/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         Download_Check_Service.java
// Zweck:            Service in Backround
//
// erstellt am:      18.06.2015
// zuletzt geändert: 16.03.2016
// ****************************************************************************
*/

package org.qtproject.example.notification;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.text.format.DateUtils;
import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import android.widget.Toast;
import java.lang.Thread.UncaughtExceptionHandler;
import java.util.Calendar;
import java.util.GregorianCalendar ;

import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.InetSocketAddress;
import java.net.*;

import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;
import java.io.FileOutputStream;
import java.io.*;
import java.net.*;

public class Download_Check_Service extends Service
{
    private int last_Height_L = 0;
    private int last_Height_C = 0;
    private int secondLastHeight_L = 0;
    private int secondLastHeight_C = 0;
    private int thirdLastHeight_L = 0;
    private int thirdLastHeight_C = 0;
    private int hour_last = 0;
    private int minute_last = 0;
    private List<Integer> WarningHeights = new ArrayList<Integer>();
    private List<String>  WarningNames = new ArrayList<String>();
    private List<Boolean> AlreadyTriggered = new ArrayList<Boolean>();
    private List<Boolean> StateWarningAtOverUnderheight = new ArrayList<Boolean>();
    private List<Boolean> editables = new ArrayList<Boolean>();

    static boolean connection = true;
    private Thread.UncaughtExceptionHandler onRuntimeError = new Thread.UncaughtExceptionHandler()
    {
        public void uncaughtException(Thread thread, Throwable ex)
        {
            Intent Service = new Intent("org.qtproject.example.notification.Download_Check_Service"); //Try starting the service again
            startService(Service);      //May be a pending intent might work
        }
    };

    private String Password = new String();
    private String User = new String();
    private String Url = new String();
    private Boolean showNotification = false;

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public void onCreate()
    {
        super.onCreate();
        Thread.setDefaultUncaughtExceptionHandler(onRuntimeError);

        Log.v("Download_Check_Service", System.currentTimeMillis() + ": Download_Check_Service erstellt.");

	Intent MyServiceIntent = new Intent(this, org.qtproject.example.notification.Download_Check_Service.class);
        PendingIntent MyServicePendingIntent = PendingIntent.getService(this, 0, MyServiceIntent,0);

        long interval = DateUtils.MINUTE_IN_MILLIS * 1; // start Service every minute

        /*Calendar cur_cal = new GregorianCalendar();
        cur_cal.setTimeInMillis(System.currentTimeMillis());//set the current time and date for this calendar

        Calendar cal = new GregorianCalendar();
        cal.add(Calendar.DAY_OF_YEAR, cur_cal.get(Calendar.DAY_OF_YEAR));
        cal.set(Calendar.HOUR_OF_DAY, cur_cal.get(Calendar.HOUR_OF_DAY));
        cal.set(Calendar.MINUTE, cur_cal.get(Calendar.MINUTE));
        cal.set(Calendar.SECOND, 15);
        cal.set(Calendar.MILLISECOND, cur_cal.get(Calendar.MILLISECOND));
        cal.set(Calendar.DATE, cur_cal.get(Calendar.DATE));
        cal.set(Calendar.MONTH, cur_cal.get(Calendar.MONTH));

        long firstStart = cal.getTimeInMillis() + interval;*/

        long firstStart = System.currentTimeMillis() + interval;

        AlarmManager am = (AlarmManager) getSystemService(Context.ALARM_SERVICE);
        am.setInexactRepeating(AlarmManager.RTC, firstStart, interval, MyServicePendingIntent);
        Log.v("Download_Check_Service", "AlarmManager gesetzt");

        //Toast.makeText(this, "HWWS Service onCreate", Toast.LENGTH_LONG).show();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        Log.v("Download_Check_Service", System.currentTimeMillis() + ": Download_Check_Service gestartet.");

        //#############################################################################

        Calendar Date = Calendar.getInstance();
        int day = Date.get(Calendar.DAY_OF_MONTH);
        int year = Date.get(Calendar.YEAR);
        int month = Date.get(Calendar.MONTH) + 1;
	hour_last = 0;
	minute_last = 0;

        DecimalFormat df = new DecimalFormat("00");
        String FilenameForDownload = "data_%s_%s_%s.txt";
        FilenameForDownload = String.format(FilenameForDownload, year, df.format(month), df.format(day));
	ReadInSecretFile();


       // MessageNotification.notify(this, Url, 0, "Bitte Internetverbindung überprüfen");

        if((User.length() > 3) && (Password.length() > 3))
        {
            if(connection == true)
            {
                Download(Url + "/hwws", User, Password, FilenameForDownload);   //TODO use another file to save data resources

                ReadInData("/storage/emulated/0/HWWS/Daten/" + FilenameForDownload);
                ReadInWarnings("/storage/emulated/0/HWWS/Daten/Warnings.txt");
                CheckWarnings();
                WriteWarnings();

                if(showNotification == true)
                {
		    NewMessageNotification_steady.cancel(this);
		    NewMessageNotification_steady.notify(this, String.format("%02d", hour_last) + ":" + String.format("%02d", minute_last) +
		    " --> " + String.valueOf((last_Height_C + last_Height_L) / 2)
                    + " " + "\u00B1" + " " + String.valueOf((Math.abs(last_Height_C-last_Height_L)/2)) + " cm", 0);
                }
                else
                {
                    NewMessageNotification_steady.cancel(this);
                }

		Calendar cal = new GregorianCalendar();
		cal.setTimeInMillis(System.currentTimeMillis());//set the current time and date for this calendar
		int current_hour = cal.get(Calendar.HOUR_OF_DAY);
		int current_minute = cal.get(Calendar.MINUTE);

		if(current_hour > hour_last + 1 || current_minute > minute_last + 5)
		{
		    MessageNotification.notify(this, "Achtung: Daten sind nicht aktuell!", 0, "möglicherweise werden keine Warnungen gesendet!");
		}
            }
            else
            {
                MessageNotification.notify(this, "keine Internetverbindung", 0, "Bitte Internetverbindung überprüfen");
            }
	}	    
	else
	{
	    NewMessageNotification_steady.notify(this, "abgemeldet...keine Überprüfung", 0);
	}

        //#############################################################################

        stopSelf(); //after working stop the Service self

        return START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        Log.v("Download_Check_Service", System.currentTimeMillis() + ": Download_Check_Service zerstoert.");
    }

    private void Download(String hostname, String username, String password, String filename)
    {
        String ftpUrl = "ftp://%s:%s@%s/%s";
        ftpUrl = String.format(ftpUrl, username, password, hostname, filename);

        FTP_Download Download = new FTP_Download();
        Download.setSaveFileName(filename);
        Download.execute(ftpUrl);
    }

    private void ReadInData(String Path_filename)
    {
	List<String>  records =	    new ArrayList<String>();
	List<Integer> hours =	    new ArrayList<Integer>();
	List<Integer> minutes =	    new ArrayList<Integer>();
	List<Integer> Height_L =    new ArrayList<Integer>();
	List<Integer> Height_C =    new ArrayList<Integer>();
        List<Integer> temperature = new ArrayList<Integer>();

	hours.clear();
	minutes.clear();
	records.clear();
	Height_L.clear();
	Height_C.clear();
	temperature.clear();

        try
        {
            BufferedReader reader = new BufferedReader(new FileReader(Path_filename));
            String line;
            while ((line = reader.readLine()) != null)
            {
                records.add(line);
            }
            reader.close();
        }
        catch (Exception e)
        {
            System.err.format("Exception occurred trying to read '%s'.", Path_filename);
            e.printStackTrace();
        }

        for(int i=0; i<records.size(); i++)
        {
            String[] sl = records.get(i).split(":");

            hours.add(Integer.parseInt(sl[0].trim()));
            minutes.add(Integer.parseInt(sl[1].trim()));
            Height_C.add(Integer.parseInt(sl[3].trim()));
            Height_L.add(Integer.parseInt(sl[4].trim()));
            temperature.add(Integer.parseInt(sl[5].trim()));
        }

	hour_last = hours.get(hours.size() - 1);
	minute_last = minutes.get(minutes.size() - 1);

	last_Height_C = Height_C.get(Height_C.size() - 1);
	last_Height_L = Height_L.get(Height_L.size() - 1);
	secondLastHeight_C = Height_C.get(Height_C.size() - 2);
	secondLastHeight_L = Height_L.get(Height_L.size() - 2);
	thirdLastHeight_C  = Height_C.get(Height_C.size() - 3);
	thirdLastHeight_L  = Height_L.get(Height_L.size() - 3);
    }

    private void CheckWarnings()
    {
       int count = 0;
       if(WarningHeights.size() != 0)
       {
           for(int i=0; i<WarningHeights.size(); i++)
           {
	       if(StateWarningAtOverUnderheight.get(i) == false)
	       {
		   if((last_Height_L >= WarningHeights.get(i)) && (secondLastHeight_L >= WarningHeights.get(i)) && (thirdLastHeight_L >= WarningHeights.get(i))
		   || (last_Height_C >= WarningHeights.get(i)) && (secondLastHeight_C >= WarningHeights.get(i)) && (thirdLastHeight_C >= WarningHeights.get(i))
		   || (last_Height_C >= WarningHeights.get(i)) && (last_Height_L >= WarningHeights.get(i)))
		   {
		       count++;
		       if(AlreadyTriggered.get(i) == false)
		       {
			   MessageNotification.notify(this, "Achtung: Überschreitung der Höhe bei: " + WarningNames.get(i), 0, String.valueOf(count-1) + " weitere aktive Warnungen");
			   AlreadyTriggered.set(i, true);
		       }
		   }
                   else if ((last_Height_L <= WarningHeights.get(i) - 1) && (last_Height_C <= WarningHeights.get(i) - 1))
		   {
		       AlreadyTriggered.set(i, false);
		   }
	       }
	       else
	       {
		   if((last_Height_L <= WarningHeights.get(i)) && (secondLastHeight_L <= WarningHeights.get(i)) && (thirdLastHeight_L <= WarningHeights.get(i))
		   || (last_Height_C <= WarningHeights.get(i)) && (secondLastHeight_C <= WarningHeights.get(i)) && (thirdLastHeight_C <= WarningHeights.get(i))
		   || (last_Height_C <= WarningHeights.get(i)) && (last_Height_L <= WarningHeights.get(i)))
		   {
		       count++;
		       if(AlreadyTriggered.get(i) == false)
		       {
			   MessageNotification.notify(this, "Achtung: Unterschreitung der Höhe bei: " + WarningNames.get(i), 0, String.valueOf(count-1) + " weitere aktive Warnungen");
			   AlreadyTriggered.set(i, true);
		       }
		   }
                   else if ((last_Height_L >= WarningHeights.get(i) + 1) && (last_Height_C >= WarningHeights.get(i) + 1))
		   {
		       AlreadyTriggered.set(i, false);
		   }
	       }
           }
       }
       WriteWarnings();
    }

    private void ReadInWarnings(String Path_filename)
    {
        List<String> records = new ArrayList<String>();
        try
        {
            BufferedReader reader = new BufferedReader(new FileReader(Path_filename));
            String line;
            while ((line = reader.readLine()) != null)
            {
                records.add(line);
            }
            reader.close();
        }
        catch (Exception e)
        {
            System.err.format("Exception occurred trying to read '%s'.", Path_filename);
            e.printStackTrace();
        }

        WarningNames.clear();
        WarningHeights.clear();
        AlreadyTriggered.clear();
        StateWarningAtOverUnderheight.clear();
	editables.clear();

        for(int i=0; i<records.size(); i++)
        {
            String[] sl = records.get(i).split(":");

            WarningNames.add(sl[0].trim());
            WarningHeights.add(Integer.parseInt((sl[1].trim())));
            AlreadyTriggered.add(Boolean.parseBoolean(sl[2]));
	    StateWarningAtOverUnderheight.add(Boolean.parseBoolean(sl[3]));
	    editables.add(Boolean.parseBoolean(sl[4]));
        }
    }

    private void WriteWarnings()
    {
        /*
        try
        {
	   File file = new File("/storage/emulated/0/HWWS/Daten/Warnings.txt");

            // if file doesn't exists, then create it
            if (!file.exists())
            {
                file.createNewFile();
            }
            FileWriter fw = new FileWriter(file.getAbsoluteFile());
            BufferedWriter bw = new BufferedWriter(fw);

            for(int i=0; i<WarningHeights.size(); i++)
            {
                String WarningData = "%s:%s:%s:%s";
                WarningData = String.format(WarningData, WarningNames.get(i), WarningHeights.get(i).toString(),
                AlreadyTriggered.get(i).toString(), StateWarningAtOverUnderheight.get(i).toString());

                bw.write(WarningData);
                bw.newLine();
            }
            bw.close();

            System.out.println("Done");
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }*/
        try
        {
            File file = new File("/storage/emulated/0/HWWS/Daten/Warnings.txt");
            FileOutputStream fop = new FileOutputStream(file);

            FileLock fl = fop.getChannel().tryLock();
            if(fl != null)
            {
                // if file doesnt exists, then create it
                if (!file.exists())
                {
                    file.createNewFile();
                }

                for(int i=0; i<WarningHeights.size(); i++)
                {
		    String WarningData = "%s:%s:%s:%s:%s";
                    WarningData = String.format(WarningData, WarningNames.get(i), WarningHeights.get(i).toString(),
		    AlreadyTriggered.get(i).toString(), StateWarningAtOverUnderheight.get(i).toString(), editables.get(i).toString());

                    byte[] WarningDataInBytes = WarningData.getBytes();
                    fop.write(WarningDataInBytes);
                    String Line = "\n";
                    byte[] newLine = Line.getBytes();
                    fop.write(newLine);
                }

                fop.flush();
                fop.close();

                fl.release();
            }
        }

        catch (IOException e)
        {
            e.printStackTrace();
        }

   /*
    finally
    {
        try
        {
            if (fop != null)
            {
                    fop.close();
            }
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }*/
    }

    private void ReadInSecretFile()
    {
       List<String> records = new ArrayList<String>();
       try
       {
	   BufferedReader reader = new BufferedReader(new FileReader("/storage/emulated/0/HWWS/Daten/secret"));
	   String line;
	   while ((line = reader.readLine()) != null)
	   {
	       records.add(line);
	   }
	   reader.close();
       }
       catch (Exception e)
       {
	   System.err.format("Exception occurred trying to read '%s'.", "/storage/emulated/0/HWWS/Daten/secret");
	   e.printStackTrace();
       }

       for(int i=0; i<records.size(); i++)
       {
	   String[] sl = records.get(i).split(":");
	   User = sl[0].trim();
	   Password = sl[1].trim();
	   Url = sl[2].trim();
           showNotification = Boolean.valueOf(sl[3].trim());
       }
    }

    private boolean pingHost(String host, Integer port, Integer timeout)
    {
        try
        {
            Socket socket = new Socket();
            MessageNotification.notify(this, "innen", 0, "Bitte Internetverbindung überprüfen");
            socket.connect(new InetSocketAddress(host, port), timeout);

            return true;
        }
        catch (IOException e)
        {
            return false; // Either timeout or unreachable or failed DNS lookup.
        }
    }    
}
