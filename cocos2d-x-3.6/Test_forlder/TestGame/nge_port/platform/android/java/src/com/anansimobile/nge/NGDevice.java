package com.anansimobile.nge;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.Locale;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.graphics.Point;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Environment;
import android.provider.Settings.Secure;
import android.view.WindowManager;

public class NGDevice {
	
	public static final String documentRoot = "Documents";
	
	private static final int BUFFER_SIZE = 1024;
	
	private static String sDeviceId = "00112234455667";
	
	/**
	 * just call this method in Main::onCreate()
	 */
	public static void InitDeviceId() {
		/*
		 * The value may change if a factory reset is performed on the device.
		 * @see: http://developer.android.com/reference/android/provider/Settings.Secure.html#ANDROID_ID
		 */
		
		sDeviceId = Secure.getString(
				NextGenEngine.sMainActivity.getContentResolver(),
				Secure.ANDROID_ID);
		
		//or use this
		//Requires Permission: READ_PHONE_STATE
		/*
		TelephonyManager tm = (TelephonyManager) NextGenEngine.sMainActivity.getSystemService(Context.TELEPHONY_SERVICE);
		tm.getDeviceId();
		*/
		NextGenEngine.nge_logf("device id: %s", sDeviceId);
	}
	
	public static String getDeviceId() {
		
		return sDeviceId;
	}
	
	public static String getMacAddress(){
		
		WifiManager manager = (WifiManager) NextGenEngine.sMainActivity.getSystemService(Context.WIFI_SERVICE);
		WifiInfo wifiInfo = manager.getConnectionInfo();
		String MACAddress = wifiInfo.getMacAddress();

		/* FIXME: 在没有wifi的设备上暂时使用deviceid代替macid。[zhen.chen] */
		if (MACAddress == null) {
			MACAddress = getDeviceId();
		}
		
		return MACAddress;
	}
	
	public static String getModel() {
		return Build.MODEL;
	}
	
	public static String getDeviceVersion() {
		return Build.VERSION.RELEASE;
	}
	
	public static int getDeviceSDKVersion() {
		return Build.VERSION.SDK_INT;
	}

	public static String getFilePath(String filename) {
		
		//the file must be exist, in Documents/ or in assets
		
		File file = null;
		
		if (filename.endsWith(".res") || filename.endsWith(".res_")) {
			/* 先查找是否有obb格式数据 */
			final String startFlag = filename.endsWith(".res") ? "main" : "patch";
			File externalDocRoot = new File(getObbPath());
			if (externalDocRoot.exists()) {
				String[] obbs = externalDocRoot.list(new FilenameFilter() {
					@Override
					public boolean accept(File dir, String file) {
						if (file.startsWith(startFlag) && file.endsWith(".obb")) {
							return true;
						}
						return false;
					}
					
				});
				
				if (obbs.length > 0) {
					String targetObb = obbs[0];
					for (int i = 1; i < obbs.length; i++) {
						int vcStart = targetObb.indexOf(".") + 1;
						int vcEnd	= targetObb.indexOf(".", vcStart);
						int vc = Integer.valueOf(targetObb.substring(vcStart, vcEnd));
						
						int vcNewStart = obbs[i].indexOf(".") + 1;
						int vcNewEnd	= obbs[i].indexOf(".", vcNewStart);
						int vcNew = Integer.valueOf(obbs[i].substring(vcNewStart, vcNewEnd));
						
						if (vcNew > vc) {
							targetObb = obbs[i];
						}
					}
					String fullPath = String.format("%s/%s", externalDocRoot.getAbsolutePath(), targetObb);
					NextGenEngine.nge_logf("obb file: %s", fullPath);
					return fullPath;
				}
			}
		}
		/* 从外部存储中查找，目前来说外部存储中只保存打包资源。 */
		File externalDocRoot = getExternalDocumentRoot();
    	if (externalDocRoot != null) {
    		file = new File(externalDocRoot, filename);
        	if (file.exists()) {
        		return file.getAbsolutePath();
        	} else {
        		file = null;
        	}
    	}
		
		File docRoot = getDocumentRoot();
		file = new File(docRoot, filename);
		if (file.exists()) {
			return file.getAbsolutePath();
		}

		//file is in assets
		
		//TODO: 这里有两个问题，从assets里读出的文件是不是应该使用之后删除？什么时候删除？
		
		InputStream is;
		try {
			is = NextGenEngine.sMainActivity.getResources().getAssets().open(filename);
			FileOutputStream fos = new FileOutputStream(file);

		    byte[] buf = new byte[BUFFER_SIZE];    
		    int size = 0; 

			while (-1 != (size = is.read(buf))) {
				fos.write(buf, 0, size);
			}
			
			fos.close();
			is.close();
			
		} catch (IOException e) {
			//FIXME: if do nothing when code reached here, game will crash.[zhen.chen]
			NextGenEngine.nge_log("read file from assets failed!~");
		}
	
		return file.getAbsolutePath();

	}

	public static String getFileWritePath(String filename) {
		
		if (filename.contains(".obb")) {
			//obb temp file
			return getObbFilePath(filename);
		}
		
		File docRoot = getDocumentRoot();
		File file = new File(docRoot, filename);
		
		return file.getAbsolutePath();
	}

	public static boolean isFileExist(String filename) {
		File docRoot = NGDevice.getDocumentRoot();		
		File file = new File(docRoot, filename);
		
		return file.exists();

		/**
		 * 暂时不考虑assets中的文件
		 */
		
//		if (!file.exists()) {
//			return (null != ReadAsset(filename));
//		}		
		
//		return false;
	}

	public static File getDocumentRoot() {
		
		//not use this code because it will add a "app_" prefix to dir.
//		File docRoot = m_mainActivity.getDir(documentRoot, Context.MODE_PRIVATE);
		
		/**
		 * here use a dir named "Documents" to store game data.
		 * we should direct use /files dir too.
		 */
		File packageRoot = NextGenEngine.sMainActivity.getFilesDir().getParentFile();
		File docRoot = new File(packageRoot, documentRoot);
		
		if (!docRoot.exists()) {
			docRoot.mkdir();
		}
		
		return docRoot;
	}
	
	public static File getExternalDocumentRoot() {
    	if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
    		File externalRoot = Environment.getExternalStorageDirectory();
    		File docRoot = new File(externalRoot,
    				String.format("%s%s%s",
    						//"Android", File.separator, 
    						"data",
    						File.separator, NextGenEngine.sMainActivity.getPackageName()
    						));
    		
    		if (!docRoot.exists()) {
    			docRoot.mkdirs();
    		}
    		
    		NextGenEngine.nge_logf("External: %s", docRoot.getAbsolutePath());
    		
    		return docRoot;
    	}
    	return null;
	}

	/*
	public static String getRelativeFilePath(String filename) {
		return String.format("%s%s%s", documentRoot, File.separator, filename);
	}
	*/
	
	public static long getCurrentTimeMS() {
		return System.currentTimeMillis();
	}
	
	public static String getLocaleLanguage() {
		return Locale.getDefault().getLanguage();
	}

	public static String getLocaleCountry() {
		return Locale.getDefault().getCountry();
	}
	
	public static int getScreenWidth() {
		if (getDeviceSDKVersion() < 13) {
	    	WindowManager wm = NextGenEngine.sMainActivity.getWindowManager();
	    	return wm.getDefaultDisplay().getWidth();
		} else {
			Point size = new Point(0, 0);
			getScreenSize(size);
			return size.x;
		}
	}
	
	public static int getScreenHeight() {
		if (getDeviceSDKVersion() < 13) {
	    	WindowManager wm = NextGenEngine.sMainActivity.getWindowManager();
	    	return wm.getDefaultDisplay().getHeight();
		} else {
			Point size = new Point(0, 0);
			getScreenSize(size);
			return size.y;
		}
	}

	@SuppressLint("NewApi")
	private static void getScreenSize(Point size) {
    	WindowManager wm = NextGenEngine.sMainActivity.getWindowManager();
		wm.getDefaultDisplay().getSize(size);
	}
	
	public static String getPackageName() {
		return NextGenEngine.sMainActivity.getPackageName();
	}
	
	public static String getObbPath() {
    	if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
    		File externalRoot = Environment.getExternalStorageDirectory();
    		String obbPath = String.format("%s/Android/obb/%s", externalRoot.getAbsolutePath(), getPackageName());
    		File obbPathFile = new File(obbPath);
    		if (!obbPathFile.exists()) {
    			obbPathFile.mkdirs();
    		}
    		return obbPath;
    	}
    	return null;
	}
	
	public static String getObbFilePath(String obbfile) {
    	if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
    		File externalRoot = Environment.getExternalStorageDirectory();
    		return String.format("%s/Android/obb/%s/%s", externalRoot.getAbsolutePath(), getPackageName(), obbfile);
    	}
    	return null;
	}

	/*
	public static void moveFile(String from, String to) {
		File fromFile = new File(from);
		File toFile = new File(to);

		fromFile.renameTo(toFile);
	}
	*/

	public static void launchUrl(String url) {
		Intent i = new Intent(Intent.ACTION_VIEW);
		i.setData(Uri.parse(url));
		if (url.contains("play.google.com")) {
			//temp solve to open google play.
			i.setClassName("com.android.vending", "com.android.vending.AssetBrowserActivity");
		}
		
		//port to cocos2dx, disable here.
		//NextGenEngine.sMainActivity.intentFilter(i);
		
		NextGenEngine.sMainActivity.startActivity(i); 
	}
	
	public static long getFreeMemory() {
		ActivityManager am = (ActivityManager)NextGenEngine.sMainActivity.getSystemService(Context.ACTIVITY_SERVICE);
		ActivityManager.MemoryInfo memInfo = new ActivityManager.MemoryInfo();
		am.getMemoryInfo(memInfo);
		return memInfo.availMem;
	}
}
