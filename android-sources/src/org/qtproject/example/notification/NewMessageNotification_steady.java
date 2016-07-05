/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan Wögerbauer
// Filename:         NewMessageNotification_steady.java
// Zweck:            send the Notification for the status
//
// erstellt am:      18.06.2015
// zuletzt geändert: 16.12.2016
// ****************************************************************************
*/

package org.qtproject.example.notification;

import android.annotation.TargetApi;
import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Build;
import android.content.Intent;
import android.app.PendingIntent;

//import android.support.v4.app.NotificationCompat;

/**
 * Helper class for showing and canceling new message steady
 * notifications.
 * <p/>
 * This class makes heavy use of the {@link NotificationCompat.Builder} helper
 * class to create notifications in a backward-compatible way.
 */
public class NewMessageNotification_steady {
    /**
     * The unique identifier for this type of notification.
     */
    private static final String NOTIFICATION_TAG = "NewMessage_steady";

    /**
     * Shows the notification, or updates a previously shown notification of
     * this type, with the given parameters.
     * <p/>
     * TODO: Customize this method's arguments to present relevant content in
     * the notification.
     * <p/>
     * TODO: Customize the contents of this method to tweak the behavior and
     * presentation of new message steady notifications. Make
     * sure to follow the
     * <a href="https://developer.android.com/design/patterns/notifications.html">
     * Notification design guidelines</a> when doing so.
     *
     * @see #cancel(Context)
     */
    public static void notify(final Context context, final String exampleString, final int number)
    {
        final Resources res = context.getResources();

        // This image is used as the notification's large icon (thumbnail).
        // TODO: Remove this if your notification has no relevant thumbnail.
        final Bitmap picture = BitmapFactory.decodeResource(res, R.drawable.icon);


        final String ticker = exampleString;
        final String title = "HWWS-Status";
        final String text = exampleString;

        Intent startService = new Intent(context, Download_Check_Service.class);
        //Intent stopService = new Intent(context, Download_Check_Service.class);

        //stopService.putExtra("stopSelf", 1);

        PendingIntent pendingIntentStart = PendingIntent.getService(context, 0, startService, 0);

    final NotificationCompat.Builder builder = new NotificationCompat.Builder(context)

                // Set appropriate defaults for the notification light, sound,
                // and vibration.
		//.setDefaults(Notification.DEFAULT_ALL)

                        // Set required fields, including the small icon, the
                        // notification title, and text.
                .setSmallIcon(R.drawable.icon)
                .setContentTitle(title)
                .setContentText(text)

                        // All fields below this line are optional.

                        // Use a default priority (recognized on devices running Android
                        // 4.1 or later)
                .setPriority(NotificationCompat.PRIORITY_DEFAULT)

                        // Provide a large icon, shown with the notification in the
                        // notification drawer on devices running Android 3.0 or later.
                .setLargeIcon(picture)

                        // Set ticker text (preview) information for this notification.
                .setTicker(ticker)

                        // Show a number. This is useful when stacking notifications of
                        // a single type.
                .setNumber(number)
                .addAction(R.drawable.ic_action_stat_reply,
                        "Service neu starten",
                         pendingIntentStart
                         ) // #0



                .setOngoing(true);

        notify(context, builder.build());
    }

    @TargetApi(Build.VERSION_CODES.ECLAIR)
    private static void notify(final Context context, final Notification notification)
    {
        final NotificationManager nm = (NotificationManager) context
                .getSystemService(Context.NOTIFICATION_SERVICE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.ECLAIR)
        {
            nm.notify(NOTIFICATION_TAG, 0, notification);
        }
        else
        {
            nm.notify(NOTIFICATION_TAG.hashCode(), notification);
        }
    }

    /**
     * Cancels any notifications of this type previously shown using
     * {@link #notify(Context, String, int)}.
     */
    @TargetApi(Build.VERSION_CODES.ECLAIR)
    public static void cancel(final Context context)
    {
        final NotificationManager nm = (NotificationManager) context
                .getSystemService(Context.NOTIFICATION_SERVICE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.ECLAIR)
        {
            nm.cancel(NOTIFICATION_TAG, 0);
        }
        else
        {
            nm.cancel(NOTIFICATION_TAG.hashCode());
        }
    }
}
