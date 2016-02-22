package com.anansimobile.nge;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.media.SoundPool.OnLoadCompleteListener;

public class NGMediaPlayer {
	
	/*
	 * ==========================================================
	 * sfx
	 * ==========================================================
	 */

	private static final int INVALID_SOUND_STREAM_ID	= 0;
	private static final int MAX_SOUND_STREAM_NUM		= 10;	//meybe not enough, but just 3 now, fix it later.
	private static final int SOUND_QUALITY				= 100;	//no effect yet, before 4.2 [zhen.chen]
	private static final int SOUND_PRIORITY				= 1;	//no effect yet, before 4.2 [zhen.chen]

	private static float streamVolume = 0;	//default volume is 0,
	private static float soundStreamVolume = .0f;

	private static SoundPool soundPool = null; 
	private static HashMap<Integer, Integer> soundPoolMap = null;
	private static HashMap<Integer, Integer> soundPoolPlayMap = null;
	
	static class SoundPlayInfo {
		public int mStreamId;
		public int mLoopMode;
		public int mSID;
	}
	private static HashMap<Integer, SoundPlayInfo> soundPoolPlayFailedList = null;

	public static void initSounds() { 

		/* initialize sound pool info. */
		soundPoolMap = new HashMap<Integer, Integer>();
		soundPoolPlayMap = new HashMap<Integer, Integer>();
		soundPoolPlayFailedList = new HashMap<Integer, SoundPlayInfo>();

		soundPool = new SoundPool(MAX_SOUND_STREAM_NUM, AudioManager.STREAM_MUSIC, SOUND_QUALITY);
		
		/**	FIXME:
		 *  soundPool载入音效时看起来并不是阻塞完成的，所以有可能第一次播放时会无效，或许应该在载入资源时将音效资源load到soundPool里。
		 *  一个考虑：这里可以尝试在load完成时再将streamID放入map。
		 *  [zhen.chen]
		 **/
		soundPool.setOnLoadCompleteListener(new OnLoadCompleteListener() {

			@Override
			public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
				NextGenEngine.nge_logf("load sound complete: %d, %d", sampleId, status);
				synchronized (soundPoolPlayFailedList) {
					SoundPlayInfo info = soundPoolPlayFailedList.get(sampleId);
					if (info != null) {
						int playedStreamId =
							soundPool.play(
								info.mStreamId,
								/*streamVolume * */soundStreamVolume,
								/*streamVolume * */soundStreamVolume,
								1,
								info.mLoopMode,
								1f);
						if (playedStreamId == INVALID_SOUND_STREAM_ID) {
							NextGenEngine.nge_logf("play sound (%d) failed!~", info.mSID);
						} else {
							soundPoolPlayMap.put(info.mSID, playedStreamId);
						}
						soundPoolPlayFailedList.remove(sampleId);
					}
				}
			}
			
		});
		
		AudioManager mgr = (AudioManager)NextGenEngine.sMainActivity.getSystemService(Context.AUDIO_SERVICE);
		streamVolume = mgr.getStreamVolume(AudioManager.STREAM_MUSIC);
	}

	/**
	 * 通过文件名载入音效资源
	 * @param sid 音效id
	 * @param soundFile 音效文件名
	 */
	public static void loadSound(int sid, String soundFile) {
	  
		int streamId = INVALID_SOUND_STREAM_ID;

		//check file exist like NGDevice.isFileExist().
		File docRoot = NGDevice.getDocumentRoot();		
	  	File file = new File(docRoot, soundFile);

	  	if (file.exists()) {
	  		/* 直接载入Documents存在的音效文件 */
	  		streamId = soundPool.load(file.getAbsolutePath(), SOUND_PRIORITY);
	  	} else {
	  		/* 载入资源(Assets)中的音效文件 */
	  		AssetFileDescriptor assetFD = null;
	  		try {
	  			assetFD = NextGenEngine.sMainActivity.getAssets().openFd(soundFile);
	  		} catch (IOException e) {
	  			/* 载入出错，可能是不存在 */
	  			NextGenEngine.nge_logf("load asset (%s) failed!~ msg: %s", soundFile, e.getMessage());
	  		}
	  		if (assetFD != null) {
	  			streamId = soundPool.load(assetFD, SOUND_PRIORITY);
	  		}
	  	}

	  	if (streamId != INVALID_SOUND_STREAM_ID) {
	  		soundPoolMap.put(sid, streamId);
	  	}
	}
	
	/**
	 * 从资源包中载入音效
	 * @param sid 音效id
	 * @param packagePath 资源包完整路径
	 * @param offset 音效在资源包中的位置
	 * @param len 音效数据长度
	 */
	public static void loadSoundInPackage(int sid, String packagePath, int offset, int len) {

		FileInputStream fis = null;
		try {
			fis = new FileInputStream(packagePath);
			soundPool.load(fis.getFD(), offset, len, SOUND_PRIORITY);
			fis.close();
		} catch (FileNotFoundException e) {
			NextGenEngine.nge_logf("load package (%s) failed!~ msg: %s", packagePath, e.getMessage());
		} catch (IOException e) {
			NextGenEngine.nge_logf("load package (%s) failed!~ msg: %s", packagePath, e.getMessage());
		}
	}    

	/**
	 * 清除音效缓存
	 * @param sid 音效id
	 */
	public static void unloadSound(int sid) {
		int streamId = getSoundStreamId(sid);
		if (streamId != INVALID_SOUND_STREAM_ID) {
			soundPool.unload(streamId);
		}
	}

	/**
	 * 播放音效
	 * @param sid 音效id
	 * @param loop 是否循环
	 */
	public static void playSound(int sid, boolean loop) {
		int loopMode = loop ? -1 : 0;
		int streamId = getSoundStreamId(sid);
		if (streamId != INVALID_SOUND_STREAM_ID) {
			int playedStreamId =
				soundPool.play(
					streamId,
					/*streamVolume * */soundStreamVolume,
					/*streamVolume * */soundStreamVolume,
					1,
					loopMode,
					1f);
			if (playedStreamId == INVALID_SOUND_STREAM_ID) {
				NextGenEngine.nge_logf("play sound (%d) failed!~", sid);
				synchronized (soundPoolPlayFailedList) {
					SoundPlayInfo info = new SoundPlayInfo();
					info.mSID = sid;
					info.mStreamId = streamId;
					info.mLoopMode = loopMode;
					soundPoolPlayFailedList.put(streamId, info);
				}
			} else {
//				soundPoolMap.put(sid, playedStreamId);
				soundPoolPlayMap.put(sid, playedStreamId);
			}
		}
	}

	/**
	 * 停止音效
	 * @param sid 音效id
	 */
	public static void stopSound(int sid) {
		int streamId = getSoundPlayStreamId(sid);
		if (streamId != INVALID_SOUND_STREAM_ID) {
			soundPool.stop(streamId);
		}
	}
	
	/**
	 * 暂停音效
	 * @param sid 音效id
	 */
	public static void pauseSound(int sid) {
		int streamId = getSoundPlayStreamId(sid);
		if (streamId != INVALID_SOUND_STREAM_ID) {
			soundPool.pause(streamId);
		}
	}
	
	/**
	 * 恢复音效
	 * @param sid 音效id
	 */
	public static void resumeSound(int sid) {
		int streamId = getSoundPlayStreamId(sid);
		if (streamId != INVALID_SOUND_STREAM_ID) {
			soundPool.resume(streamId);
		}
	}
	
	/**
	 * 清空所有音效
	 */
	public static void clearSoundPool() {
		soundPool.release();
		soundPoolMap.clear();
		soundPoolPlayMap.clear();
		soundPoolPlayFailedList.clear();
	}
	
	public static void setSoundVolume(int sid, int volume) {
		{
			int streamId = getSoundStreamId(sid);
			if (streamId != INVALID_SOUND_STREAM_ID) {
				soundStreamVolume = (float) volume / 100;
				soundPool.setVolume(streamId, soundStreamVolume, soundStreamVolume);
			}
		}
		
		{
			//play stream
			int streamId = getSoundPlayStreamId(sid);
			if (streamId != INVALID_SOUND_STREAM_ID) {
				soundStreamVolume = (float) volume / 100;
				soundPool.setVolume(streamId, soundStreamVolume, soundStreamVolume);
			}
		}

	}
	
	public static void setSoundVolume(int volume) {
		{
			Iterator<Integer> itr = soundPoolMap.values().iterator();
			while (itr.hasNext()) {
				int streamId = itr.next();
				if (streamId != INVALID_SOUND_STREAM_ID) {
					soundStreamVolume = (float) volume / 100;
					soundPool.setVolume(streamId, soundStreamVolume, soundStreamVolume);
				}
			}
		}
		
		{
			Iterator<Integer> itr = soundPoolPlayMap.values().iterator();
			while (itr.hasNext()) {
				int streamId = itr.next();
				if (streamId != INVALID_SOUND_STREAM_ID) {
					soundStreamVolume = (float) volume / 100;
					soundPool.setVolume(streamId, soundStreamVolume, soundStreamVolume);
				}
			}
		}

	}
	
	private static int getSoundStreamId(int sid) {
		Integer streamId = soundPoolMap.get(sid);
		if (streamId != null) {
			return streamId.intValue();
		}
		
		return INVALID_SOUND_STREAM_ID;
	}
	
	private static int getSoundPlayStreamId(int sid) {
		Integer streamId = soundPoolPlayMap.get(sid);
		if (streamId != null) {
			return streamId.intValue();
		}
		
		return INVALID_SOUND_STREAM_ID;
	}
	
	/*
	 * ==========================================================
	 * background music
	 * ==========================================================
	 */
	
	public static MediaPlayer createBackgroundMusic() {

		//only new here now.
		return new MediaPlayer();

	}
	
	public static void loadBackgroundMusic(MediaPlayer bgMusicPlayer, String musicName) {
		//check file exist like NGDevice.isFileExist().
		File docRoot = NGDevice.getDocumentRoot();		
	  	File file = new File(docRoot, musicName);

  		try {
	  		if (file.exists()) {
		  		/* 直接载入Documents存在的音乐文件 */
		  		bgMusicPlayer.setDataSource(file.getAbsolutePath());
		  	} else {
		  		/* 载入资源(Assets)中的音效文件 */
		  		AssetFileDescriptor assetFD = NextGenEngine.sMainActivity.getAssets().openFd(musicName);
		  		bgMusicPlayer.setDataSource(assetFD.getFileDescriptor(), assetFD.getStartOffset(), assetFD.getLength());
		  	}
	  		
	  		bgMusicPlayer.prepare();

  		} catch (IOException e) {
  			/* 载入出错，可能是不存在 */
  			NextGenEngine.nge_logf("load asset (%s) failed!~ msg: %s", musicName, e.getMessage());
  		}
	}
	
	/**
	 * 从资源包中载入背景音乐
	 * @param bgMusicPlayer 用于播放背景音乐的MediaPlayer实例
	 * @param packagePath 资源包路径
	 * @param offset 背景音乐在资源包中的位置
	 * @param len 背景音乐数据长度
	 */
	public static void loadBackgroundMusicInPackage(MediaPlayer bgMusicPlayer, String packagePath, int offset, int len) {
		FileInputStream fis = null;
		try {
			
			fis = new FileInputStream(packagePath);
	  		bgMusicPlayer.setDataSource(fis.getFD(), offset, len);
			fis.close();

			bgMusicPlayer.prepare();
		
		} catch (FileNotFoundException e) {
			NextGenEngine.nge_logf("load package (%s) failed!~ msg: %s", packagePath, e.getMessage());
		} catch (IOException e) {
			NextGenEngine.nge_logf("load package (%s) failed!~ msg: %s", packagePath, e.getMessage());
		}
	}
	
	private static MediaPlayer sCurBackgroundMuisc = null;
	
	public static void unloadBackgroundMusic(MediaPlayer bgMusicPlayer) {
		bgMusicPlayer.reset();
	}
	
	public static void playBackgroundMusic(MediaPlayer player, boolean loop) {
		player.setLooping(loop);

		if (!player.isPlaying()) {
			player.start();
			
			sCurBackgroundMuisc = player;
		}
	}
	
	public static void stopBackgroundMusic(MediaPlayer player) {
//		player.stop();
		player.pause();
		player.seekTo(0);
	}

	public static void pauseBackgroundMusic(MediaPlayer player) {
		player.pause();
	}

	public static void resumeBackgroundMusic(MediaPlayer player) {
		player.start();
	}
	
	public static void releaseBackgroundMusic(MediaPlayer player) {
		player.release();
		
		if (sCurBackgroundMuisc == player) {
			sCurBackgroundMuisc = null;
		}
	}
	
	public static void setBackgroundMusicVolume(MediaPlayer player, int volume) {
		player.setVolume((float) volume / 100, (float) volume / 100);
	}
	
	/* here is a temp way to slove background music player on game pause/resume. */
	public static void pauseBackgroundMusic() {
		if (sCurBackgroundMuisc != null) {
			sCurBackgroundMuisc.pause();
		}
	}
	
	public static void resumeBackgroundMuisc() {
		if (sCurBackgroundMuisc != null) {
			sCurBackgroundMuisc.start();
		}
	}
}
