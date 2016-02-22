package com.anansimobile.nge;

import java.io.IOException;
import java.io.InputStream;
import java.util.Iterator;
import java.util.LinkedList;

import android.content.res.AssetManager;
import android.graphics.Typeface;

public class NGTypefacePool {
	
	private static LinkedList<TypefaceInfo> sTypefaceInfos = new LinkedList<TypefaceInfo>();
	
	public static void clearAllTypeface() {
		
	}

	public static Typeface getTypeface(String name) {

		TypefaceInfo tfInfo = null;
		
		Iterator<TypefaceInfo> itr = sTypefaceInfos.iterator();
		while (itr.hasNext()) {
			TypefaceInfo nodeTypefaceInfo = itr.next();
			if (nodeTypefaceInfo.getName().equals(name)) {
				tfInfo = nodeTypefaceInfo;
				break;
			}
		}
		
		if  (tfInfo == null) {
			tfInfo = new TypefaceInfo(name);
			sTypefaceInfos.add(tfInfo);
		}
		
		return tfInfo.getTypeface();
	}
	
}

class TypefaceInfo {
	private String mName = "";
//	private int mSize = 0;
//	private int mColor = 0xFFFFFFFF;
	private Typeface mTypeface = null;

	public TypefaceInfo(String mName/*, int mSize, int mColor*/) {

		this.mName = mName;
//		this.mSize = mSize;
//		this.mColor = mColor;
		
		initTypeface();
	}
	
	private void initTypeface() {
	
		AssetManager mgr = NextGenEngine.sMainActivity.getAssets();

		boolean customFontExist = true;
		try {
			InputStream is = mgr.open(mName + ".ttf");
			is.close();
			
		} catch (IOException e) {
			customFontExist = false;
		}
		
		if (customFontExist) {
			mTypeface = Typeface.createFromAsset(mgr, mName + ".ttf");
		} else {
			/* if not match font name, system default font may be given. */
			mTypeface = Typeface.create(mName, Typeface.BOLD);
		}

	}
	
	public String getName() {
		return mName;
	}
	
	public Typeface getTypeface() {
		return mTypeface;
	}

	@Override
	public boolean equals(Object o) {
		  if (this == o) {
			  return true;
		  }
		  
		  if (!(o instanceof TypefaceInfo)) {
			  return false;
		  }
		  
		  TypefaceInfo tfInfo = (TypefaceInfo)o;
		  return mName.equals(tfInfo.mName);
	}

	@Override
	public int hashCode() {
		return mName.hashCode();
	}

	@Override
	public String toString() {
		return String.format("TypefaceInfo, name: %s", mName);
	}
	
	
}

