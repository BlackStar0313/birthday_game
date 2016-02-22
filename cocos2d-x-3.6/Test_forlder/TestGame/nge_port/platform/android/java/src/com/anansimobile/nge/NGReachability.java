package com.anansimobile.nge;

import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

public class NGReachability {

	private static ConnectivityManager connMng = null;
	private static Timer mTimer = null;
	private static int mStatus = 0;	//0: unavailable; 1: wifi; 2: mobile
	
	private static void startCheck() {
		
		mStatus = updateStatus();

		if (mTimer == null) {
			mTimer = new Timer();
		}
		
		mTimer.schedule(new TimerTask() {

			@Override
			public void run() {
				int st = updateStatus();
				if (mStatus != st) {
					mStatus = st;
					onNetworkStatusChange(mStatus);
				}				
			}
			
		}, 30000, 30000);
	}
	
	private static void stopCheck() {
		if (mTimer != null) {
			mTimer.cancel();
			mTimer = null;
		}
	}
	
	private static int updateStatus() {
		if (connMng == null) {
			connMng = (ConnectivityManager) NextGenEngine.sMainActivity.getSystemService(Context.CONNECTIVITY_SERVICE);
		}

		NetworkInfo ni = connMng.getActiveNetworkInfo();
		if (ni != null) {
			if (ni.getType() == ConnectivityManager.TYPE_WIFI) {
				return 1;
			} else {
				return 2;
			}
		} else {
			return 0;
		}
	}
	
	private static native void onNetworkStatusChange(int status);
}
