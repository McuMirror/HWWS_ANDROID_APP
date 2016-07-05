/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         StartServiceAfterBootReceiver.java
// Zweck:            start Service when Boot is completed
//
// erstellt am:      18.06.2015
// zuletzt geändert: 16.12.2016
// ****************************************************************************
*/


package org.qtproject.example.notification;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

public class StartServiceAfterBootReceiver extends BroadcastReceiver
{
    public StartServiceAfterBootReceiver()
    {
    }

    @Override
    public void onReceive(Context context, Intent intent)
    {
        if (intent.getAction().equalsIgnoreCase(Intent.ACTION_BOOT_COMPLETED))
        {
	    Toast.makeText(context, "HWWS Service gestartet", Toast.LENGTH_LONG).show();
	    Intent Service = new Intent(context, Download_Check_Service.class);
	    context.startService(Service);

        }
    }
}
