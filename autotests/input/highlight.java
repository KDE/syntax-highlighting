/* highlighting test case for Java */

// single line comment with ### TODO FIXME

/* multiline comment with ### TODO FIXME */

/** Javadoc comment with ### TODO FIXME
 * @param foo bla bla ATTENTION
 * @return abc cde
 * @see javadoc.xml
*/

package org.kde.itinerary;

import org.qtproject.qt5.android.bindings.QtActivity;
import java.io.*;
import java.util.*;

public class Activity extends QtActivity
{
    private static final String TAG = "org.kde.itinerary";

    public void launchViewIntentFromUri(String uri)
    {
        Uri mapIntentUri = Uri.parse(uri);
        Intent mapIntent = new Intent(Intent.ACTION_VIEW, mapIntentUri);
        startActivity(mapIntent);
    }

    @Override
    protected String stuff()
    {
        while (true) {
        }
        int num = 1;
        num = 0x2;
        double f = .4e+49;
    }
}
