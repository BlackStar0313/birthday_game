#Talking Game SDK

##集成

###iOS

* 包含asTalkingDataGAInterface.h
* 包含ios目录下所有文件
		
###Android

* 包含asTalkingDataGAInterface.h
* 包含android目录下所有文件

###Windows Phone

 //TODO
 
##初始化

(如果不打算全平台使用，可以用宏NGE_PLATFORM_XXX限定)

####初始化

* iOS：

	1. 在templateAppDelegate的didFinishLaunchingWithOptions方法中添加以下代码：

			asTalkingDataGAInterface::Init(<app_id>, <channel_id>);
			
			//其中：
			//app_id从talkingGame后台获取
			//channel_id按需要填写，如不需要填""或"App Store"
		
* Android：

	1. 在AndroidManifest.xml中的<application>下添加(内容同iOS)：
		
			<meta-data android:value="xxxxxxxxxxxxxxx" android:name="TDGA_APP_ID"></meta-data>
			<meta-data android:value="channelId" android:name="TDGA_CHANNEL_ID"/>
			
	2. 在Main.onCreate()中调用:
			
			TalkingDataStatistics.init(this);
	
	3. 在Main.onResume()中调用:

		    TalkingDataStatistics.onResume(this);

	4. 在Main.onPause()中调用:
	
		    TalkingDataStatistics.onPause(this);

* Windows Phone

		//TODO