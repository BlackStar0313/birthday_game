package com.anansimobile.nge;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.LinkedList;

import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.pm.ApplicationInfo;
import android.content.res.AssetFileDescriptor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Paint.FontMetrics;
import android.graphics.Typeface;
import android.media.MediaPlayer;
import android.util.Log;

public class NextGenEngine {
	
	public static GL10 glInstance = null;
	
	public static void nge_log(String log) {
		if (isDebugMode()) {
			Log.i("native_project_lib", String.format("[jni_log] %s", log));
		}
	}
	
	public static void nge_logf(String format, Object...args) {
		if (isDebugMode()) {
			Log.i("native_project_lib", String.format("[jni_log] %s", String.format(format, args)));
		}
	}
	
	public static Activity sMainActivity = null;
	//port to cocos2dx, disable here.
	//public static RootService sMainService = null;
	
	public static boolean isAssetExist(String filename) {
		try {
			InputStream is = sMainActivity.getResources().getAssets().open(filename);
			is.close();
		} catch (IOException e) {
			return false;
		}
		
		return true;
	}
	
	/**
	 * 通知native code载入图片数据
	 * 
	 * @param texturePtr ngTexture指针
	 * @param bitmap 将被载入的Bitmap对象
	 */
	private static void loadBitmap(int texturePtr, Bitmap bitmap) {
		int contentWidth = bitmap.getWidth();
		int contentHeight = bitmap.getHeight();

		//生成2^n的贴图
        //<<
        int width = contentWidth;
        int height = contentHeight;

    	if((width != 1) && (width & (width - 1)) != 0) {
    		int i = 1;
    		while(i < width) {
    			i *= 2;
    		}
    		width = i;
    	}

    	if((height != 1) && (height & (height - 1)) != 0) {
    		int i = 1;
    		while(i < height) {
    			i *= 2;
    		}
    		height = i;
    	}

    	Bitmap wrapperBitmap = null;

    	if (width == contentWidth && height == contentHeight) {
    		wrapperBitmap = bitmap;
    	} else {
    		wrapperBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    		Canvas canvas = new Canvas(wrapperBitmap);
    		canvas.drawBitmap(bitmap, 0, 0, null);
    		bitmap.recycle();
    		bitmap = null;
    	}
    	
		//>>

        if (wrapperBitmap != null) {
	        bitmapDataDidLoad(texturePtr, wrapperBitmap, contentWidth, contentHeight);
	        wrapperBitmap.recycle();
	        wrapperBitmap = null;
        }
	}

	public static byte[] openFile(String filename) {
		nge_logf("will open %s", filename);
        /* for doodlefill only*/
		String surfix="";
		String name="";
		int pos = filename.lastIndexOf(".");
		if(pos>0 && pos<filename.length()){
			 surfix = filename.substring(pos+1,filename.length());
			 name = filename.substring(0,pos);
		}
		
		if(surfix.equals("pda") ){
			filename = name+"@2x.mp3";
		}
        /*for doodlefill only end*/
		
		File docRoot = NGDevice.getDocumentRoot();
		
		File file = new File(docRoot, filename);

		if (!file.exists()) {
			nge_log("not exist in Documents/, will find it in asset!");
			return ReadAsset(filename);
		}

		nge_log(file.getAbsolutePath());
		
		byte[] fileData = null;

		FileInputStream os;
		try {
			os = new FileInputStream(file);

			int dataSize = os.available();
			fileData = new byte[dataSize];
			os.read(fileData, 0, dataSize);

		} catch (FileNotFoundException e) {
			nge_logf("open file: %s failed, msg: %s!~", filename, e.getMessage());
		} catch (IOException e) {
			nge_logf("open file: %s failed, msg: %s!~", filename, e.getMessage());
		}
		
		
		
		return fileData;

	}
	
	public static native void bitmapDataDidLoad(int texturePtr, Bitmap bitmap, int width, int height);

	public static void loadBitmapData(int texturePtr, String imagename) {
		
		NextGenEngine.nge_log(String.format("start load bitmap data, name(%s)!~", imagename));

		InputStream bitmapStream = null;
	    Bitmap bitmap = null;

	        try {
		        // 打开图片资源流
	        	if (NGDevice.isFileExist(imagename)) {
	        		//从Documents路径读取文件
	        		bitmapStream = new FileInputStream(new File(NGDevice.getDocumentRoot(), imagename));
	        	} else {
	        		/* port to cocos2dx
        			File exportedImageAsset = new File(
        					sMainActivity.getFilesDir() + File.separator +
        					"assets_" +	String.valueOf(sMainActivity.getExpansionFileVersionCode()) + File.separator +
        					imagename);
        			if (exportedImageAsset.exists()) {
        				bitmapStream = new FileInputStream(exportedImageAsset);
        			} else
        			*/
	        		{
		        		//从Assets读取文件
						bitmapStream = sMainActivity.getResources().getAssets().open(imagename);
        			}
	        	}
			} catch (IOException e) {
				NextGenEngine.nge_log("load bitmap failed!~");
			}
	        // 解码图片生成 Bitmap 实例
	        bitmap = BitmapFactory.decodeStream(bitmapStream);
	        loadBitmap(texturePtr, bitmap);
	}

	public static void loadPackageBitmapData(int texturePtr, byte[] imageData, int len) {
		
//		NextGenEngine.nge_log("start load bitmap data from package data!~");

	    Bitmap bitmap = null;

        // 解码图片生成 Bitmap 实例
        bitmap = BitmapFactory.decodeByteArray(imageData, 0, len);
        loadBitmap(texturePtr, bitmap);
	}

	/*
	public static long loadTexture(String imagename) {
		
		//TODO: code here to check texture file in Documents/.
		
		//load texture in assets
		
//		nge_log("load texture!");
		
		InputStream bitmapStream = null;
	    Bitmap bitmap = null;

	    try {

	        // 打开图片资源流
        	if (NGDevice.isFileExist(imagename)) {
        		//从Documents路径读取文件
        		bitmapStream = new FileInputStream(new File(NGDevice.getDocumentRoot(), imagename));
        	} else {
        		//从Assets读取文件
				bitmapStream = sMainActivity.getResources().getAssets().open(imagename);
        	}

        	// 解码图片生成 Bitmap 实例
	        bitmap = BitmapFactory.decodeStream(bitmapStream);
	        
	        int w = bitmap.getWidth();
	        int h = bitmap.getHeight();

//	        ByteBuffer bb = ByteBuffer.allocate(w * h * 4);
//	        bitmap.copyPixelsToBuffer(bb);
//	        bb.array();

	        int texture[] = new int[1];

	        GL10 gl = glInstance;

	        gl.glGenTextures(1, texture, 0);
	        gl.glBindTexture(GL10.GL_TEXTURE_2D, texture[0]);

	        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
	        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
	    	gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE); 
	    	gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);

	        GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0); 

//	        gl.glEnable(GL10.GL_TEXTURE_2D);
//	        gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
	        
	        return (((long)w) << 48) | (((long)h) << 32) | ((((long)texture[0]) << 32) >>> 32);
 
	    } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
			return 0;

		} finally {
	  
	        if (bitmap != null)
	            bitmap.recycle();
	  
	        if (bitmapStream != null) {
	            try {
	                bitmapStream.close();
	            } catch (IOException e) {
	            	
	            }
	        }
	    }
	}
	
	*/
	
	/*
	public static long loadPackageTexture(byte[] imageData, int len) {
		//TODO: code here to check texture file in Documents/.
		
		//load texture in assets
		
//		nge_log("load texture!");
		
	    Bitmap bitmap = null;

       	// 解码图片生成 Bitmap 实例
	        bitmap = BitmapFactory.decodeByteArray(imageData, 0, len);
	        
	        int w = bitmap.getWidth();
	        int h = bitmap.getHeight();

//	        ByteBuffer bb = ByteBuffer.allocate(w * h * 4);
//	        bitmap.copyPixelsToBuffer(bb);
//	        bb.array();

	        int texture[] = new int[1];

	        GL10 gl = glInstance;

	        gl.glGenTextures(1, texture, 0);
	        gl.glBindTexture(GL10.GL_TEXTURE_2D, texture[0]);

	        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
	        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
	    	gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE); 
	    	gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);

	        GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0); 

//	        gl.glEnable(GL10.GL_TEXTURE_2D);
//	        gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);

	        if (bitmap != null) {
	        	bitmap.recycle();
	        }

	        return (((long)w) << 48) | (((long)h) << 32) | ((((long)texture[0]) << 32) >>> 32); 

	}
	
	*/

	public static final int SYSTEMFONT_MODE_FILL		 = 0;
	public static final int SYSTEMFONT_MODE_STROKE		 = 1;
	public static final int SYSTEMFONT_MODE_FILLSTROKE	 = 2;

	/**
	 * 
	 * @param str
	 * @param fontSize
	 * @param fontColor
	 * @param fontStroke
	 * @param fontShadow
	 * @param fontMode 字体模式，枚举(ESYSTEMFONT_MODE)
	 * @return
	 */
	public static long CreateTextureWithString(
			int texturePtr,
			String str,
			String fontName,
			int fontSize,
			int fontColor,
			int strokeColor,
			int shadowColor,
			int fontMode) {
		
		Paint textPaint = new Paint();
		
		textPaint.setAntiAlias(true);		//防止锯齿
		textPaint.setFilterBitmap(true);	//alpha
		
		Typeface tf = NGTypefacePool.getTypeface(fontName);
		textPaint.setTypeface(tf);
		textPaint.setTextSize(fontSize);
		
		FontMetrics fm = textPaint.getFontMetrics();
		
		float strW = textPaint.measureText(str);
		float strH = fm.bottom - fm.top;
		
		if (strH <= 0) {
			tf = Typeface.create(fontName, Typeface.BOLD);
			textPaint.setTypeface(tf);
			textPaint.setTextSize(fontSize);
			fm = textPaint.getFontMetrics();
			strW = textPaint.measureText(str);
			strH = fm.bottom - fm.top;
		}
		
		nge_logf("str (%s) size: %.2f x %.2f", str, strW, strH);
		
		//temp use ARGB_8888 here.
		Bitmap bitmap = Bitmap.createBitmap((int)(strW + 0.5f), (int)(strH + 0.5f), Bitmap.Config.ARGB_8888);
		Canvas canvas = new Canvas(bitmap);
		
		textPaint.setColor(fontColor);
		canvas.drawText(str, 0, -fm.top, textPaint);

		/* 描边的方法有待商榷。[zhen.chen] */
		
		if (
				(fontMode == SYSTEMFONT_MODE_STROKE || fontMode == SYSTEMFONT_MODE_FILLSTROKE)) {

			Paint strokePaint = new Paint(textPaint);
			strokePaint.setStyle(Paint.Style.STROKE);
			strokePaint.setStrokeCap(Paint.Cap.ROUND);
			strokePaint.setStrokeJoin(Paint.Join.ROUND);
			strokePaint.setStrokeWidth(1);
			strokePaint.setColor(strokeColor);
			
			canvas.drawText(str, 0, -fm.top, strokePaint);
			canvas.drawText(str, 0, -fm.top, textPaint);
		}

		loadBitmap(texturePtr, bitmap);

		return 0;

		/*
        int texture[] = new int[1];

        GL10 gl = glInstance;

        gl.glGenTextures(1, texture, 0);
        gl.glBindTexture(GL10.GL_TEXTURE_2D, texture[0]);

        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
    	gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE); 
    	gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);

        GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0); 

//        gl.glEnable(GL10.GL_TEXTURE_2D);
//        gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
        
        return (((long)strW) << 48) | (((long)strH) << 32) | ((((long)texture[0]) << 32) >>> 32);
        */
	}
	
	public static final int STRING_TEXTURE_CACHE_LIMIT = 2048;

	/**
	 * 创建一个文本贴图缓冲区，和CreateTextureWithString的区别在于限制贴图尺寸，防止出现渲染白屏的问题。
	 * @param texturePtr
	 * @param str
	 * @param fontName
	 * @param fontSize
	 * @param fontColor
	 * @param strokeColor
	 * @param shadowColor
	 * @param fontMode
	 * @return string cache image property
	 */
	public static int[] CreateStringTextureCache(
			int texturePtr,
			String str,
			String fontName,
			int fontSize,
			int fontColor,
			int strokeColor,
			int shadowColor,
			int fontMode) {
		
		Paint textPaint = new Paint();
		
		textPaint.setAntiAlias(true);		//防止锯齿
		textPaint.setFilterBitmap(true);	//alpha
		
		Typeface tf = NGTypefacePool.getTypeface(fontName);
		textPaint.setTypeface(tf);
		textPaint.setTextSize(fontSize);
		
		FontMetrics fm = textPaint.getFontMetrics();
		
		/* get text bounds, if str is empty, bounds will be 0. */
//		android.graphics.Rect bounds = new android.graphics.Rect();
//		textPaint.getTextBounds(str, 0, str.length(), bounds);

		float strW = textPaint.measureText(str);
		float strH = fm.bottom - fm.top;
		
		if (strH <= 0) {
			tf = Typeface.create(fontName, Typeface.BOLD);
			textPaint.setTypeface(tf);
			textPaint.setTextSize(fontSize);
			fm = textPaint.getFontMetrics();
			strW = textPaint.measureText(str);
			strH = fm.bottom - fm.top;
		}
		
		final float strokeWidth = fontSize >= 32 ? 8 : 6;
		
		if ((fontMode == SYSTEMFONT_MODE_STROKE || fontMode == SYSTEMFONT_MODE_FILLSTROKE)) {
			strW += strokeWidth;
			strH += strokeWidth;
		}

		nge_logf("str (%s) size: %.2f x %.2f", str, strW, strH);
		
		Bitmap bitmap = null;
		int[] lineWidths = null;
		
		if (strW <= STRING_TEXTURE_CACHE_LIMIT) {
			//temp use ARGB_8888 here.
			bitmap = Bitmap.createBitmap((int)(strW + 0.5f), (int)(strH + 0.5f), Bitmap.Config.ARGB_8888);
			Canvas canvas = new Canvas(bitmap);
			
			/*
			//test code to give all bitmap text a background color.
			Paint p = new Paint();
			p.setColor(0xaabbcc);
			p.setAlpha(255);
			canvas.drawRect(new android.graphics.Rect(0,0,(int)(strW + 0.5f), (int)(strH + 0.5f)), p);
			*/

			textPaint.setColor(fontColor);

			/* 描边的方法有待商榷。[zhen.chen] */
			
			if ((fontMode == SYSTEMFONT_MODE_STROKE || fontMode == SYSTEMFONT_MODE_FILLSTROKE)) {

				/*
				Paint strokePaint = new Paint(textPaint);
				strokePaint.setStyle(Paint.Style.STROKE);
				strokePaint.setStrokeCap(Paint.Cap.ROUND);
				strokePaint.setStrokeJoin(Paint.Join.ROUND);
				strokePaint.setStrokeWidth(1);
				strokePaint.setColor(strokeColor);
				
				canvas.drawText(str, 0, -fm.top, strokePaint);
				*/
				Paint strokePaint = new Paint(textPaint);
				strokePaint.setColor(strokeColor);

				float tx = strokeWidth / 2;
				float ty = strokeWidth / 2 + (-fm.top);
				for (int i = 0; i < strokeWidth / 2; i++) {
					for (int j = 0; j < strokeWidth / 2; j++) {
						canvas.drawText(str, tx - i, ty, strokePaint);
						canvas.drawText(str, tx + i, ty, strokePaint);
						canvas.drawText(str, tx, ty - j, strokePaint);
						canvas.drawText(str, tx, ty + j, strokePaint);
					
						canvas.drawText(str, tx - i, ty + j, strokePaint);
						canvas.drawText(str, tx + i, ty + j, strokePaint);
						canvas.drawText(str, tx - i, ty - j, strokePaint);
						canvas.drawText(str, tx + i, ty - j, strokePaint);
					}
				}
				
				canvas.drawText(str, tx, ty, textPaint);
			} else {
				canvas.drawText(str, 0, -fm.top, textPaint);
			}
			
			lineWidths = new int[3];
			lineWidths[0] = 1;
			lineWidths[1] = (int)(strH + 0.5f);
			lineWidths[2] = (int)(strW + 0.5f);

			loadBitmap(texturePtr, bitmap);

			return lineWidths;
		}

		//这里开始分行，主要可能超过限制的是滚动文本，目前限制为2048x2048(STRING_TEXTURE_CACHE_LIMIT)，应该比较富裕。
		int startAt = 0;
		int endAt = str.length();
		LinkedList<String> lineStr = new LinkedList<String>();
		
		float lineW = 0;
		while (true) {
			
			endAt = (endAt + startAt) / 2;
			lineW = textPaint.measureText(str, startAt, endAt + 1);
			
			if (lineW <= STRING_TEXTURE_CACHE_LIMIT) {
				lineStr.add(str.substring(startAt, endAt + 1));
				startAt = endAt + 1;
				endAt = str.length();
				
				if (endAt <= startAt) {
					//done
					break;
				} else {
					lineW = textPaint.measureText(str, startAt, endAt);

					if (lineW <= STRING_TEXTURE_CACHE_LIMIT) {
						//done
						lineStr.add(str.substring(startAt, endAt));
						break;
					}
				}
			}
		};

		int lineHeight = (int)(strH + 0.5f);
		lineWidths = new int[lineStr.size() + 2];
		lineWidths[0] = lineStr.size();
		lineWidths[1] = lineHeight;
		for (int i = 0; i < lineStr.size(); i++) {
			lineWidths[i + 2] = (int) textPaint.measureText(lineStr.get(i));
		}

		//temp use ARGB_8888 here.
		bitmap = Bitmap.createBitmap(STRING_TEXTURE_CACHE_LIMIT, (int)(strH + 0.5f) * lineStr.size(), Bitmap.Config.ARGB_8888);
		Canvas canvas = new Canvas(bitmap);

		textPaint.setColor(fontColor);
		Paint strokePaint = null;
		if ((fontMode == SYSTEMFONT_MODE_STROKE || fontMode == SYSTEMFONT_MODE_FILLSTROKE)) {
			/*
			strokePaint = new Paint(textPaint);
			strokePaint.setStyle(Paint.Style.STROKE);
			strokePaint.setStrokeCap(Paint.Cap.ROUND);
			strokePaint.setStrokeJoin(Paint.Join.ROUND);
			strokePaint.setStrokeWidth(1);
			strokePaint.setColor(strokeColor);
			*/
			strokePaint = new Paint(textPaint);
			strokePaint.setColor(strokeColor);
		}

		for (int i = 0; i < lineStr.size(); i++) {
			String line = lineStr.get(i);
			/* 描边的方法有待商榷。[zhen.chen] */
			if ((fontMode == SYSTEMFONT_MODE_STROKE || fontMode == SYSTEMFONT_MODE_FILLSTROKE)) {				
				float tx = strokeWidth / 2;
				float ty = strokeWidth / 2 + (-fm.top) + lineHeight * i;
				for (int _i = 0; _i < strokeWidth / 2; _i++) {
					for (int _j = 0; _j < strokeWidth / 2; _j++) {
						canvas.drawText(str, tx - _i, ty, strokePaint);
						canvas.drawText(str, tx + _i, ty, strokePaint);
						canvas.drawText(str, tx, ty - _j, strokePaint);
						canvas.drawText(str, tx, ty + _j, strokePaint);

						canvas.drawText(str, tx - _i, ty + _j, strokePaint);
						canvas.drawText(str, tx + _i, ty + _j, strokePaint);
						canvas.drawText(str, tx - _i, ty - _j, strokePaint);
						canvas.drawText(str, tx + _i, ty - _j, strokePaint);
					}
				}
				canvas.drawText(line, tx, ty, textPaint);
			} else {
				canvas.drawText(line, 0, -fm.top + lineHeight * i, textPaint);
			}

		}

		loadBitmap(texturePtr, bitmap);

		return lineWidths;

	}
	
	public static void setMediaPlayer(MediaPlayer mplayer, String mediaFile) {
		
		try {
			AssetFileDescriptor fd = sMainActivity.getAssets().openFd(mediaFile);
			mplayer.setDataSource(fd.getFileDescriptor(), fd.getStartOffset(), fd.getLength());
//			mplayer.prepare();
		} catch (IOException e) {
			nge_logf("set media player (\"%s\") failed!~", mediaFile);
		}

	}

	
	public static byte[] ReadAsset(String filename) {
		try {
			InputStream is = sMainActivity.getResources().getAssets().open(filename);
			int len = is.available();	//byte length
			
			if (len > 0) {
				
//				nge_log(String.format("got it!, len: %d", len));
				
				byte[] data = new byte[len];
				int offset = 0;
				while (offset < len) {
					int read = is.read(data, offset, (1024 > len - offset) ? (len - offset) : 1024);
					if (-1 == read) {
						break;
					} else {
						offset += read;
					}
					
				}

				is.close();

				return data;
			}

		} catch (IOException e) {
//			e.printStackTrace();
//			nge_log("read asset faild!");
			return null;
		}
		
		return null;
	}
	
	public static boolean isDebugMode() {
		if (sMainActivity == null) {
			return false;
		}
		ApplicationInfo info = sMainActivity.getApplicationInfo();
		return (info.flags & ApplicationInfo.FLAG_DEBUGGABLE) != 0;
	}
	
	public static void exportImageAssets(final String assetsImageName,final String exportSuffix,final float scalefactor, final String outputRoot) {
		try {
			if (scalefactor != 1.0f) {
				InputStream is = sMainActivity.getAssets().open(assetsImageName);
				Bitmap bm = BitmapFactory.decodeStream(is);
				
		    	Matrix m = new Matrix();
		    	m.postScale(scalefactor, scalefactor);

		    	Bitmap scaledBitmap = Bitmap.createBitmap(bm, 0, 0, bm.getWidth(), bm.getHeight(), m, true);
		    	
	    		File outputImageFile = new File(outputRoot, String.format("%s%s.png", assetsImageName.substring(0, assetsImageName.indexOf("@")), exportSuffix));
	    		FileOutputStream fos = new FileOutputStream(outputImageFile);
	        	/*boolean success = */scaledBitmap.compress(Bitmap.CompressFormat.PNG, 100, fos);
				fos.flush();
		    	fos.close();

		    	scaledBitmap.recycle();
			} else {
				InputStream is = sMainActivity.getAssets().open(assetsImageName);
	    		File outputImageFile = new File(outputRoot, String.format("%s%s.png", assetsImageName.substring(0, assetsImageName.indexOf("@")), exportSuffix));
	    		FileOutputStream fos = new FileOutputStream(outputImageFile);
	    		int nData = 0;
	    		byte buffer[] = new byte[4096];
	    		while (true) {
	    			nData = is.read(buffer);
	    			if (nData == -1) {
	    				is.close();
	    				fos.flush();
	    				fos.close();
	    				break;
	    			} else {
	    				fos.write(buffer, 0, nData);
	    			}
	    		}
			}

		} catch (IOException e) {
			NextGenEngine.nge_logf("exportImageAssets error, msg: %s", e.getMessage());
		} catch (IndexOutOfBoundsException e) {
			NextGenEngine.nge_logf("exportImageAssets error, msg: %s", e.getMessage());
		}
	}
}
