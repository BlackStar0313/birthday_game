package com.anansimobile.nge;

import android.graphics.Paint.FontMetrics;
import android.graphics.Typeface;
import android.text.Layout.Alignment;
import android.text.StaticLayout;
import android.text.TextPaint;

public class NGStringHelper {
	
	private static String word_start = null;
	private static String word_end	 = null;
	
	private native static void AddStringLine(int ctxPtr, String line);
	private native static void UpdateLineHeight(int ctxPtr, int lineHeight);

	private native static String WordStartFlag(int ctxPtr);
	private native static String WordEndFlag(int ctxPtr);
	
	/**
	 * 解析文本，通过宽度将文本拆成多行。遍历文本，效率低。
	 * @param ctxPtr BreakTextContext指针，用于处理断行事件。
	 * @param width 限制宽度值
	 * @param text 完整文本
	 * @param fontName 字体名
	 * @param fontSize 字体大小
	 */
	/*
	public static void breakText(int ctxPtr, int width, String text, String fontName, int fontSize) {
		
		Paint textPaint = new Paint();
		textPaint.setAntiAlias(true);		//防止锯齿
		textPaint.setFilterBitmap(true);	//alpha

		Typeface tf = NGTypefacePool.getTypeface(fontName);
		textPaint.setTypeface(tf);
		textPaint.setTextSize(fontSize);
		
		//update line height
		FontMetrics fm = textPaint.getFontMetrics();
		UpdateLineHeight(ctxPtr, (int)(fm.bottom - fm.top));

		int strlen = text.length();
		if (strlen <= 0) {
			//empty
			AddStringLine(ctxPtr, "");
			return;
		}
		
		word_start = WordStartFlag(ctxPtr);
		word_end = WordEndFlag(ctxPtr);
		
//        String text = String.valueOf("Hello : 哈哈😄");
		
		int savedLineFlag = 0;		//换行标志，保存当前行第一个字符在text中的位置，AddStringLine()调用之后更新
		int savedWordStart = -1;	//分词标志，保存当前词的开始位置(在text中的位置)
 
		for (int i = 0; i < strlen; i++) {

			int codePoint = text.codePointAt(i);
			int unicodeExtra = Character.charCount(codePoint) - 1;

			//FIXME: 考虑把fix操作移到for末尾
			i += unicodeExtra;	//fix unicode extra position
			
			if (codePoint == '\n' && unicodeExtra == 0) {
				//换行符，换行
				AddStringLine(ctxPtr, text.substring(savedLineFlag, i - unicodeExtra));
				savedLineFlag = i - unicodeExtra + 1;	//+1略过\n
				savedWordStart = -1;					//换行之后重新开始检索词
				continue;
			}

			//width of current line text.
			int curWidth = (int) textPaint.measureText(text, savedLineFlag, i + 1);
			
			if (curWidth > width) {
				//长度超过限制，换行
				if (savedWordStart != -1 && savedLineFlag < savedWordStart) {
					//如果有未结束的词（并且词首不是行首），换行显示
					AddStringLine(ctxPtr, text.substring(savedLineFlag, savedWordStart));
					savedLineFlag = savedWordStart;
				} else {
					//正常换行，savedLineFlag设置为当前字符的位置，即当前字符在下一行显示
					AddStringLine(ctxPtr, text.substring(savedLineFlag, i - unicodeExtra));
					savedLineFlag = i - unicodeExtra;
					savedWordStart = savedLineFlag;
				}
			} else {
				char chr = text.charAt(i - unicodeExtra);
				if (word_start.indexOf(chr)  != -1) {
					savedWordStart = -1;
				} else if (word_end.indexOf(chr) != -1) {
					savedWordStart = -1;
				} else {
					//normal char
					if (savedWordStart == -1) {
						savedWordStart = i - unicodeExtra;
					}
				}
			}
		}
		
		//最后一行
		AddStringLine(ctxPtr, text.substring(savedLineFlag));

	}
	*/
	
//	private native void updateTextInfo(int ctxPtr, int lineNum, int lineHeight, int curPosX, int curPosY);

	/**
	 * 解析文本，使用android.text.StaticLayout。
	 * @param ctxPtr BreakTextContext指针，用于处理断行事件。
	 * @param width 限制宽度值
	 * @param text 完整文本
	 * @param fontName 字体名
	 * @param fontSize 字体大小
	 */
	private static void breakText(int ctxPtr, int width, String text, String fontName, int fontSize) {
		TextPaint textPaint = new TextPaint();
		textPaint.setAntiAlias(true);		//防止锯齿
		textPaint.setFilterBitmap(true);	//alpha

		Typeface tf = NGTypefacePool.getTypeface(fontName);
		textPaint.setTypeface(tf);
		textPaint.setTextSize(fontSize);

		//update line height
		FontMetrics fm = textPaint.getFontMetrics();
		UpdateLineHeight(ctxPtr, (int)(fm.bottom - fm.top));

		StaticLayout layout = new StaticLayout(text, textPaint, width, Alignment.ALIGN_NORMAL, 1.5f, 0, false);

		int lineNum = layout.getLineCount();
		int lineStart = 0;
		for (int i = 0; i < lineNum; i++) {
			int lineEnd = layout.getLineEnd(i);	//here lineEnd is the index of last char + 1.
			if (lineEnd == lineStart) {
				//there is no content, just empty line.
				AddStringLine(ctxPtr, text.substring(lineStart, lineEnd));
			} else {
				int lineEndOffset = 0;
				if (text.charAt(lineEnd - 1) == '\n') {
					lineEndOffset = -1;
				}
				AddStringLine(ctxPtr, text.substring(lineStart, lineEnd + lineEndOffset));
				lineStart = lineEnd;
			}
		}

		//use canvas to create cache image.
//		Bitmap bitmap = Bitmap.createBitmap(width, layout.getHeight(), Bitmap.Config.ARGB_8888);
//		Canvas canvas = new Canvas(bitmap);
//		layout.draw(canvas);
//        if (bitmap != null) {
//    		int w = bitmap.getWidth();
//    		int h = bitmap.getHeight();
////	        bitmapDataDidLoad(texturePtr, bitmap);
//	        bitmap.recycle();
//        }
	}
	
	static int getStringLen(String str) {
		if (str == null) {
			return 0;
		}

		return str.length();
	}

	static String getSubString(String str, int start, int end) {
		if (str == null || str.length() <= 0) {
			return "";
		}

		try {
			return str.substring(start, end);
		} catch (IndexOutOfBoundsException e) {
			return str;
		}
	}
}
