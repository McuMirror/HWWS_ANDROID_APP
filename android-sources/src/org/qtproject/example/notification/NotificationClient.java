/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         NotificationClient.java
// Zweck:            send the Notification from C-Code
//
// erstellt am:      18.06.2015
// zuletzt geändert: 16.12.2016
// ****************************************************************************
*/

package org.qtproject.example.notification;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;

//NotificationClient only for simulate (called from C++)

public class NotificationClient extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_builder;
    private static NotificationClient m_instance;

    public NotificationClient()
    {
        m_instance = this;
    }

    public static void notify(String s)
    {
	if (m_notificationManager == null)
	{
            m_notificationManager = (NotificationManager)m_instance.getSystemService(Context.NOTIFICATION_SERVICE);
            m_builder = new Notification.Builder(m_instance);
            m_builder.setSmallIcon(R.drawable.icon);
            m_builder.setContentTitle("Hochwasser Warnsystem");
            m_builder.setDefaults(Notification.DEFAULT_ALL);
        }

        m_builder.setContentText(s);
	m_notificationManager.notify(1, m_builder.build());
    }
}
