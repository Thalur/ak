package ak.ogltest.native_activity;

import android.app.NativeActivity;
import android.util.Log;

public class LoadLibraries extends NativeActivity
{
   static {
      Log.i("OGLtest", "Loading libraries...");

      // List of libraries to load in (e.g. "PLCore;PLMath" etc.)
      String sharedLibrariesToLoad = "z";//;png16";

      // Load in all shared libraries
      String [] libraries = sharedLibrariesToLoad.split(";");
      for (int i = 0; i < libraries.length; i++) {
         Log.d("OGLtest", "Loading library " + libraries[i]);
         System.loadLibrary(libraries[i]);
      }
   }
}

