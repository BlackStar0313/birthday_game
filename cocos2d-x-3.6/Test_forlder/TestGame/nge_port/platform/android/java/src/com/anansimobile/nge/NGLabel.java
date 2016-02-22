package com.anansimobile.nge;

import android.graphics.Paint;
import android.graphics.Typeface;
import android.graphics.Paint.FontMetrics;

public class NGLabel {
	public static String fitTextToLabelWidth(String fontName, int fontSize, String text, int maxWidth, String suffix) {

		Paint textPaint = new Paint();
		
		textPaint.setAntiAlias(true);		//防止锯齿
		textPaint.setFilterBitmap(true);	//alpha

		Typeface tf = NGTypefacePool.getTypeface(fontName);
		textPaint.setTypeface(tf);
		textPaint.setTextSize(fontSize);
		
		FontMetrics fm = textPaint.getFontMetrics();
		
		float strW = textPaint.measureText(text);
		float strH = fm.bottom - fm.top;
		
		if (strH <= 0) {
			tf = Typeface.create(fontName, Typeface.BOLD);
			textPaint.setTypeface(tf);
			textPaint.setTextSize(fontSize);
			fm = textPaint.getFontMetrics();
			strW = textPaint.measureText(text);
			strH = fm.bottom - fm.top;
		}

	    if (strW < maxWidth) {
	        /* no need clip. */
	        return null;
	    }
	    
	    for (int i = text.length(); i > 0; i--) {
	    	String clippedStr = text.substring(0, i) + suffix;
			int clippedStrWidth = (int) textPaint.measureText(clippedStr);
	        if (clippedStrWidth < maxWidth) {
	            return clippedStr;	            
	        }
	    }

	    //must not be here.

		return null;
	}
}
