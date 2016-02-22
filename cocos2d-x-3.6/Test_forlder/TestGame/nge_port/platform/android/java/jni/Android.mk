LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := NextGenEngineAndroid
LOCAL_LDLIBS    := -lz -llog -lGLESv1_CM

#jnigraphics is a lib to provide a way to access java bitmap object data from native code.
LOCAL_LDLIBS += -ljnigraphics

LOCAL_CFLAGS := \
	-DANDROID \
	-DNGE_PLATFORM_ANDROID \
	-DNGE_NETWORK \
	-DTILED_NEW_EDITOR \
	-DUSE_FILE32API \
	-DGL_GLEXT_PROTOTYPES=1 \
	-DHAVE_PTHREADS \
	-DENABLE_RES_PACKAGE \
	-DENABLE_DEVICE_RULE \
	-DENABLE_STRING_COPYBUFFER \

#for load image module from device rule file.
#LOCAL_CFLAGS += -DALL_IMAGE_MODULE

ifdef ANDROID_DEBUG
	ifeq ($(ANDROID_DEBUG), 1)
		LOCAL_CFLAGS += -DDEBUG_LOG -DDEBUG
	endif
endif
	
NGE_INC_PATH := \
    $(LOCAL_PATH) \
	$(LOCAL_PATH)/../../../headers \
	$(LOCAL_PATH)/../../../inc \
	$(LOCAL_PATH)/../../../src/platform \
	$(LOCAL_PATH)/../../../src/platform/common \
	$(LOCAL_PATH)/../../../src/lib/lua \

#NGE_SRC_PATH := $(LOCAL_PATH)../../../src
NGE_SRC_PATH := ../../../src

NGE_SRC_CORE_FILES := \
	${NGE_SRC_PATH}/core/ngApp.cpp \
	${NGE_SRC_PATH}/core/ngApplet.cpp \
	${NGE_SRC_PATH}/core/ngDevice.cpp \
	${NGE_SRC_PATH}/core/ngSingleton.cpp

NGE_SRC_GAME_FILES := \
	${NGE_SRC_PATH}/game/ngActionInput.cpp \
	${NGE_SRC_PATH}/game/ngCamera.cpp \
	${NGE_SRC_PATH}/game/ngGameApp.cpp \
	${NGE_SRC_PATH}/game/ngGameMap.cpp \
	${NGE_SRC_PATH}/game/ngGameMapScrollController.cpp \
	${NGE_SRC_PATH}/game/ngGameTilesetsProperty.cpp \
	${NGE_SRC_PATH}/game/ngGameMapGridLayer.cpp \
	${NGE_SRC_PATH}/game/ngGameMapLayer.cpp \
	${NGE_SRC_PATH}/game/ngGameMapSpriteLayer.cpp \
	${NGE_SRC_PATH}/game/ngGameObject.cpp \
	${NGE_SRC_PATH}/game/ngGameObjectFactory.cpp \
	${NGE_SRC_PATH}/game/ngGraphicFontManager.cpp \
	${NGE_SRC_PATH}/game/ngKeyInput.cpp \
	${NGE_SRC_PATH}/game/ngStringManager.cpp \
	${NGE_SRC_PATH}/game/ngSystemFontManager.cpp \
	${NGE_SRC_PATH}/game/ngTouchInput.cpp \
	${NGE_SRC_PATH}/game/ngGameResManager.cpp \
	${NGE_SRC_PATH}/game/ngGameArchiveManager.cpp \
	${NGE_SRC_PATH}/game/ngStringLoader.cpp \

NGE_SRC_GRAPHICS_FILES := \
	${NGE_SRC_PATH}/graphics/ngFont.cpp \
	${NGE_SRC_PATH}/graphics/ngGraphicFont.cpp \
	${NGE_SRC_PATH}/graphics/ngGraphics.cpp \
	${NGE_SRC_PATH}/graphics/ngGraphicsUtility.cpp \
	${NGE_SRC_PATH}/graphics/ngImage.cpp \
	${NGE_SRC_PATH}/graphics/ngSystemFont.cpp \
	${NGE_SRC_PATH}/graphics/ngTexture.cpp \
	${NGE_SRC_PATH}/graphics/ngTexturePool.cpp \

NGE_SRC_IO_FILES := \
	${NGE_SRC_PATH}/io/ngFileReader.cpp \
	${NGE_SRC_PATH}/io/ngFileWriter.cpp \
	${NGE_SRC_PATH}/io/ngFileManager.cpp \
	${NGE_SRC_PATH}/io/minizip/ioapi.c \
	${NGE_SRC_PATH}/io/minizip/mztools.c \
	${NGE_SRC_PATH}/io/minizip/unzip.c \
	${NGE_SRC_PATH}/io/minizip/zip.c \
	${NGE_SRC_PATH}/io/ngZip.cpp \
	${NGE_SRC_PATH}/io/ngResourcePool.cpp \
	${NGE_SRC_PATH}/io/ngResManager.cpp \
	${NGE_SRC_PATH}/io/ngResourcePackageReader.cpp \

NGE_SRC_UTILS_FILES := \
	${NGE_SRC_PATH}/utils/base64.c \
	${NGE_SRC_PATH}/utils/md5.c \
	${NGE_SRC_PATH}/utils/md5main.c \
	${NGE_SRC_PATH}/utils/HMACSHA1.c \
	${NGE_SRC_PATH}/utils/asUrl.cpp \

NGE_SRC_JSON_FILES := \
	${NGE_SRC_PATH}/json/ngJsonElement.cpp \
	${NGE_SRC_PATH}/json/ngJsonFactory.cpp \
	${NGE_SRC_PATH}/json/ngJsonRoot.cpp \
	${NGE_SRC_PATH}/lib/json-c/arraylist.c \
	${NGE_SRC_PATH}/lib/json-c/debug.c \
	${NGE_SRC_PATH}/lib/json-c/json_object.c \
	${NGE_SRC_PATH}/lib/json-c/json_tokener.c \
	${NGE_SRC_PATH}/lib/json-c/json_util.c \
	${NGE_SRC_PATH}/lib/json-c/linkhash.c \
	${NGE_SRC_PATH}/lib/json-c/printbuf.c \

NGE_SRC_MATH_FILES := \
	${NGE_SRC_PATH}/math/ngMath.cpp \

NGE_SRC_MEDIA_FILES := \
	${NGE_SRC_PATH}/media/ngMedia.cpp \
	${NGE_SRC_PATH}/media/ngMediaPlayer.cpp \

NGE_SRC_NCAL_FILES := \
	${NGE_SRC_PATH}/ncal/ngConnectionManager.cpp \
	${NGE_SRC_PATH}/ncal/ngConnectionRequest.cpp \
	${NGE_SRC_PATH}/ncal/ngConnectionSession.cpp \
	${NGE_SRC_PATH}/ncal/ngDownloader.cpp \
	${NGE_SRC_PATH}/ncal/ngHttpDownloader.cpp \
	${NGE_SRC_PATH}/ncal/ngHttpManager.cpp \
	${NGE_SRC_PATH}/ncal/ngHttpRequest.cpp \
	${NGE_SRC_PATH}/ncal/ngHttpResponse.cpp \
	${NGE_SRC_PATH}/ncal/ngHttpSession.cpp \
	${NGE_SRC_PATH}/ncal/ngSessionFactory.cpp \
	${NGE_SRC_PATH}/ncal/ngHttpTransporter.cpp \

NGE_SRC_HTTPCLIENT_FILES := \
	${NGE_SRC_PATH}/httpclient/ngAppRater.cpp \
	${NGE_SRC_PATH}/httpclient/ngHttpClient.cpp \
	${NGE_SRC_PATH}/httpclient/ngHttpQueue.cpp \
	${NGE_SRC_PATH}/httpclient/ngPushAlert.cpp \
	${NGE_SRC_PATH}/httpclient/ngHttpHandler.cpp \
	${NGE_SRC_PATH}/httpclient/ngHttpClientFactory.cpp \

NGE_SRC_NSL_FILES := \
	${NGE_SRC_PATH}/nsl/ngArrayList.cpp \
	${NGE_SRC_PATH}/nsl/ngArrayListIterator.cpp \
	${NGE_SRC_PATH}/nsl/ngBoolean.cpp \
	${NGE_SRC_PATH}/nsl/ngBucket.cpp \
	${NGE_SRC_PATH}/nsl/ngBit.cpp \
	${NGE_SRC_PATH}/nsl/ngByteBuffer.cpp \
	${NGE_SRC_PATH}/nsl/ngCollection.cpp \
	${NGE_SRC_PATH}/nsl/ngComparator.cpp \
	${NGE_SRC_PATH}/nsl/ngDouble.cpp \
	${NGE_SRC_PATH}/nsl/ngHashMap.cpp \
	${NGE_SRC_PATH}/nsl/ngHashMapIterator.cpp \
	${NGE_SRC_PATH}/nsl/ngHashSet.cpp \
	${NGE_SRC_PATH}/nsl/ngHashSetIterator.cpp \
	${NGE_SRC_PATH}/nsl/ngInteger.cpp \
	${NGE_SRC_PATH}/nsl/ngIOHashMap.cpp \
	${NGE_SRC_PATH}/nsl/ngIterator.cpp \
	${NGE_SRC_PATH}/nsl/ngLinkedList.cpp \
	${NGE_SRC_PATH}/nsl/ngLinkedListIterator.cpp \
	${NGE_SRC_PATH}/nsl/ngNOHashMap.cpp \
	${NGE_SRC_PATH}/nsl/ngNVHashMap.cpp \
	${NGE_SRC_PATH}/nsl/ngQuad.cpp \
	${NGE_SRC_PATH}/nsl/ngQueue.cpp \
	${NGE_SRC_PATH}/nsl/ngRandom.cpp \
	${NGE_SRC_PATH}/nsl/ngRect.cpp \
	${NGE_SRC_PATH}/nsl/ngRectf.cpp \
	${NGE_SRC_PATH}/nsl/ngReverseLinkedListIterator.cpp \
	${NGE_SRC_PATH}/nsl/ngStack.cpp \
	${NGE_SRC_PATH}/nsl/ngStopwatch.cpp \
	${NGE_SRC_PATH}/nsl/ngString.cpp \
	${NGE_SRC_PATH}/nsl/ngStringV2.cpp \
	${NGE_SRC_PATH}/nsl/nslUtils.cpp \
	${NGE_SRC_PATH}/nsl/ngKeyFactory.cpp \
	
NGE_SRC_PLATFORM_COMMON_FILES := \
	$(addprefix ../, $(wildcard ../../src/platform/common/GLES/*.cpp)) \

NGE_SRC_PLATFORM_FILES := \
	${NGE_SRC_PATH}/platform/android/http_lib.c \
	$(addprefix ../, $(wildcard ../../src/platform/android/thread/*.cpp)) \
	${NGE_SRC_PATH}/platform/android/AppRater/ngAppRaterAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngReachabilityAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngWebViewAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngDeviceAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngMediaAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngMediaPlayerAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngTextureAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngAlertDialogAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngHttpSessionAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngStdUtilAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngTextFieldImplAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngIndicatorImplAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngTextViewImplAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngStoreKitAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngLoggerAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngLoggerImpl.cpp \
	${NGE_SRC_PATH}/platform/android/ngLabelImplAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngMultiLineLabelImplAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/task/ngTaskManagerAndroid.cpp \
	$(addprefix ../, $(wildcard ../../src/platform/android/ImagePicker/*.cpp)) \
	${NGE_SRC_PATH}/platform/android/Location/ngLocationManagerAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngPushAlertAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/flurry/ngFlurryAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngJNIUtils.cpp \
	${NGE_SRC_PATH}/platform/android/ngStringHelper.cpp \
	${NGE_SRC_PATH}/platform/android/ngCURLHttpSessionAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/curl/ngCURL.cpp \
	${NGE_SRC_PATH}/platform/android/curl/ngCURLManager.cpp \
	${NGE_SRC_PATH}/platform/android/ngTextInputAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngAdViewImplAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ngFileReaderImplAndroid.cpp \
	${NGE_SRC_PATH}/platform/android/ShareKit/ngShareKitAndroid.cpp \
#	${NGE_SRC_PATH}/platform/android/nge_jni.cpp \

NGE_SRC_SOCIAL_FILES := \

NGE_SRC_SPRITE_FILES := \
	${NGE_SRC_PATH}/sprite/ngAnimation.cpp \
	${NGE_SRC_PATH}/sprite/ngFrame.cpp \
	${NGE_SRC_PATH}/sprite/ngLayer.cpp \
	${NGE_SRC_PATH}/sprite/ngModule.cpp \
	${NGE_SRC_PATH}/sprite/ngSequentialFrame.cpp \
	${NGE_SRC_PATH}/sprite/ngSequentialLayer.cpp \
	${NGE_SRC_PATH}/sprite/ngSequentialModule.cpp \
	${NGE_SRC_PATH}/sprite/ngLayerMapping.cpp \
	${NGE_SRC_PATH}/sprite/ngSprite.cpp \
	${NGE_SRC_PATH}/sprite/ngSpriteManager.cpp \
	${NGE_SRC_PATH}/sprite/ngSpritePlayer.cpp \
	${NGE_SRC_PATH}/sprite/ngSpritePlayerFactory.cpp \

NGE_SRC_UI_FILES := \
	${NGE_SRC_PATH}/ui/ngAlertDialog.cpp \
	${NGE_SRC_PATH}/ui/view/ngButton.cpp \
	${NGE_SRC_PATH}/ui/view/ngImageView.cpp \
	${NGE_SRC_PATH}/ui/view/ngLabel.cpp \
	${NGE_SRC_PATH}/ui/view/ngLabelImpl.cpp \
	${NGE_SRC_PATH}/ui/view/ngScrollView.cpp \
	${NGE_SRC_PATH}/ui/view/ngSpriteView.cpp \
	${NGE_SRC_PATH}/ui/view/ngTableView.cpp \
	${NGE_SRC_PATH}/ui/view/ngTextField.cpp \
	${NGE_SRC_PATH}/ui/view/ngTextFieldImpl.cpp \
	${NGE_SRC_PATH}/ui/view/ngTextView.cpp \
	${NGE_SRC_PATH}/ui/view/ngTextViewImpl.cpp \
	${NGE_SRC_PATH}/ui/view/ngComboBox.cpp \
	${NGE_SRC_PATH}/ui/view/ngAdView.cpp \
	${NGE_SRC_PATH}/ui/view/ngTabView.cpp \
	${NGE_SRC_PATH}/ui/view/ngFlowTableView.cpp \
	${NGE_SRC_PATH}/ui/view/ngStyleTabView.cpp \
	${NGE_SRC_PATH}/ui/view/ngSwitch.cpp \
	${NGE_SRC_PATH}/ui/view/ngProgressBar.cpp \
	${NGE_SRC_PATH}/ui/view/ngSpinBox.cpp \
	${NGE_SRC_PATH}/ui/view/ngPickerView.cpp \
	${NGE_SRC_PATH}/ui/view/ngViewContainer.cpp \
	${NGE_SRC_PATH}/ui/view/ngStretchImagePanel.cpp \
	${NGE_SRC_PATH}/ui/view/ngTableViewV2.cpp \
	${NGE_SRC_PATH}/ui/view/ngTableNaviButton.cpp \
	${NGE_SRC_PATH}/ui/view/ngMultiLineLabel.cpp \
	${NGE_SRC_PATH}/ui/view/ngMultiLineLabelImpl.cpp \
	${NGE_SRC_PATH}/ui/view/ngScrollBar.cpp \
	${NGE_SRC_PATH}/ui/view/ngView.cpp \
	${NGE_SRC_PATH}/ui/view/ngIndicator.cpp \
	${NGE_SRC_PATH}/ui/view/ngSlideBox.cpp \
	${NGE_SRC_PATH}/ui/ngUIEventManager.cpp \
	${NGE_SRC_PATH}/ui/ngUIViewReader.cpp \
	${NGE_SRC_PATH}/ui/ngImageModuleProvider.cpp \
	${NGE_SRC_PATH}/ui/ngImageModule.cpp \
	${NGE_SRC_PATH}/ui/ngViewEventListenerList.cpp \
	${NGE_SRC_PATH}/ui/ngViewManager.cpp \
	${NGE_SRC_PATH}/ui/ngViewTransition.cpp \
	${NGE_SRC_PATH}/ui/screen/ngComboBoxScreen.cpp \
	${NGE_SRC_PATH}/ui/screen/ngScreen.cpp \
	${NGE_SRC_PATH}/ui/screen/ngScreenNavigator.cpp \
	${NGE_SRC_PATH}/ui/screen/ngScreenPopupStack.cpp \
	${NGE_SRC_PATH}/ui/screen/ngScreenTranslateNode.cpp \
	${NGE_SRC_PATH}/ui/screen/ngTableScreen.cpp \
	${NGE_SRC_PATH}/ui/device_rule/ngUIDeviceRule.cpp \
	${NGE_SRC_PATH}/ui/device_rule/ngUIDeviceRuleHandler.cpp \
	${NGE_SRC_PATH}/ui/device_rule/ngUIDeviceRuleInfo.cpp \
	${NGE_SRC_PATH}/ui/device_rule/ngUIDeviceRuleManager.cpp \
	$(addprefix ../, $(wildcard ../../src/ui/delegate/*.cpp)) \
	
NGE_SRC_ACTION_FILES := \
	$(addprefix ../, $(wildcard ../../src/action/*.cpp)) \

NGE_SRC_TASK_FILES := \
	${NGE_SRC_PATH}/extension/task/ngTask.cpp \
	${NGE_SRC_PATH}/extension/task/ngTaskManager.cpp

NGE_SRC_GESTURE_FILES := \
	${NGE_SRC_PATH}/extension/gesture/GeometricRecognizer.cpp \
	${NGE_SRC_PATH}/extension/gesture/ngGestureManager.cpp

NGE_SRC_EXTENSION_FILES := \
	${NGE_SRC_PATH}/extension/ngStoreKit.cpp \
	${NGE_SRC_PATH}/extension/ngShareKit.cpp \
	${NGE_SRC_PATH}/extension/ngStat.cpp \
	${NGE_SRC_PATH}/extension/ngImagePicker.cpp \
	${NGE_SRC_PATH}/extension/ngLocationManager.cpp \
	${NGE_SRC_PATH}/extension/ngWebView.cpp \
	${NGE_SRC_PATH}/extension/ngReachability.cpp \
	${NGE_SRC_PATH}/extension/ngStatistics.cpp \
	${NGE_SRC_PATH}/extension/ngSystemBackEventHook.cpp \

NGE_SRC_PARTICLE_FILES := \
	${NGE_SRC_PATH}/lib/ngParticleHost.cpp \
	${NGE_SRC_PATH}/lib/ngParticleManager.cpp \
	${NGE_SRC_PATH}/lib/hge/hgecolor.cpp \
	${NGE_SRC_PATH}/lib/hge/hgeparticle.cpp \
	${NGE_SRC_PATH}/lib/hge/hgerect.cpp \
	${NGE_SRC_PATH}/lib/hge/hgevector.cpp

#should use lua lib.
NGE_SRC_LUA_FILES := \
	${NGE_SRC_PATH}/lib/lua/lapi.c \
	${NGE_SRC_PATH}/lib/lua/lauxlib.c \
	${NGE_SRC_PATH}/lib/lua/lbaselib.c \
	${NGE_SRC_PATH}/lib/lua/lbitlib.c \
	${NGE_SRC_PATH}/lib/lua/lcode.c \
	${NGE_SRC_PATH}/lib/lua/lcorolib.c \
	${NGE_SRC_PATH}/lib/lua/lctype.c \
	${NGE_SRC_PATH}/lib/lua/ldblib.c \
	${NGE_SRC_PATH}/lib/lua/ldebug.c \
	${NGE_SRC_PATH}/lib/lua/ldo.c \
	${NGE_SRC_PATH}/lib/lua/ldump.c \
	${NGE_SRC_PATH}/lib/lua/lfunc.c \
	${NGE_SRC_PATH}/lib/lua/lgc.c \
	${NGE_SRC_PATH}/lib/lua/linit.c \
	${NGE_SRC_PATH}/lib/lua/liolib.c \
	${NGE_SRC_PATH}/lib/lua/llex.c \
	${NGE_SRC_PATH}/lib/lua/lmathlib.c \
	${NGE_SRC_PATH}/lib/lua/lmem.c \
	${NGE_SRC_PATH}/lib/lua/loadlib.c \
	${NGE_SRC_PATH}/lib/lua/lobject.c \
	${NGE_SRC_PATH}/lib/lua/lopcodes.c \
	${NGE_SRC_PATH}/lib/lua/loslib.c \
	${NGE_SRC_PATH}/lib/lua/lparser.c \
	${NGE_SRC_PATH}/lib/lua/lstate.c \
	${NGE_SRC_PATH}/lib/lua/lstring.c \
	${NGE_SRC_PATH}/lib/lua/lstrlib.c \
	${NGE_SRC_PATH}/lib/lua/ltable.c \
	${NGE_SRC_PATH}/lib/lua/ltablib.c \
	${NGE_SRC_PATH}/lib/lua/ltm.c \
	${NGE_SRC_PATH}/lib/lua/lundump.c \
	${NGE_SRC_PATH}/lib/lua/lvm.c \
	${NGE_SRC_PATH}/lib/lua/lzio.c \
	${NGE_SRC_PATH}/extension/ngLuaEngine.cpp \
	${NGE_SRC_PATH}/extension/ngLuaObject.cpp \
	${NGE_SRC_PATH}/extension/ngLuaDataTableParser.cpp \
	${NGE_SRC_PATH}/extension/ngLuaTypeParser.cpp \
	${NGE_SRC_PATH}/extension/asLuaObject.cpp \
	${NGE_SRC_PATH}/extension/ngLuaVar.cpp \

NGE_SRC_DEBUG_FILES := \
	${NGE_SRC_PATH}/debug/asDebugInfoPanel.cpp \
	
#$(warning ${LOCAL_PATH}, ${GAME_PROJECT_SRC_FILES})

LOCAL_SRC_FILES := \
	${NGE_SRC_CORE_FILES} ${NGE_SRC_GAME_FILES} ${NGE_SRC_GRAPHICS_FILES} \
	${NGE_SRC_IO_FILES} ${NGE_SRC_LANG_FILES} ${NGE_SRC_MATH_FILES} ${NGE_SRC_MEDIA_FILES} \
	${NGE_SRC_JSON_FILES} ${NGE_SRC_NCAL_FILES} ${NGE_SRC_NSL_FILES} ${NGE_SRC_HTTPCLIENT_FILES} ${NGE_SRC_SOCIAL_FILES} \
	${NGE_SRC_SPRITE_FILES} ${NGE_SRC_UI_FILES} ${NGE_SRC_UTILS_FILES} \
	${NGE_SRC_PLATFORM_COMMON_FILES} ${NGE_SRC_PLATFORM_FILES} ${NGE_SRC_ACTION_FILES} ${NGE_SRC_EXTENSION_FILES} ${NGE_SRC_GESTURE_FILES} ${NGE_SRC_TASK_FILES} \
	${NGE_SRC_PARTICLE_FILES} ${NGE_SRC_LUA_FILES} \
	${NGE_SRC_DEBUG_FILES} \


LOCAL_C_INCLUDES := \
	$(NGE_INC_PATH)	\

#use c99 for .c file
LOCAL_C99_FILES := $(filter %.c, $(LOCAL_SRC_FILES))
TARGET-process-src-files-tags += $(call add-src-files-target-cflags, $(LOCAL_C99_FILES), -std=c99)

#LOCAL_STATIC_LIBRARIES := libcurl
#curl start
CURL_CFLAGS := -Wpointer-arith -Wwrite-strings -Wunused -Winline \
 -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -Wno-long-long \
 -Wfloat-equal -Wno-multichar -Wsign-compare -Wno-format-nonliteral \
 -Wendif-labels -Wstrict-prototypes -Wdeclaration-after-statement \
 -Wno-system-headers -DHAVE_CONFIG_H

CURL_PATH := ../../../src/platform/android
CURL_INC_PATH := $(LOCAL_PATH)/$(CURL_PATH)

include ${CURL_INC_PATH}/curl/lib/Makefile.inc
CURL_SRC_FILES := $(addprefix $(CURL_PATH)/curl/lib/,$(CSOURCES))
#LOCAL_CFLAGS += $(CURL_CFLAGS)
#add cflag to all curl src files.
TARGET-process-src-files-tags += $(call add-src-files-target-cflags, $(CURL_SRC_FILES), $(CURL_CFLAGS))
LOCAL_SRC_FILES += $(CURL_SRC_FILES)
LOCAL_C_INCLUDES += $(CURL_INC_PATH) $(CURL_INC_PATH)/curl/include $(CURL_INC_PATH)/curl/lib
#curl end
	
include $(BUILD_STATIC_LIBRARY)

#make libcurl #use code below, if need to build curl alone.
#include $(CLEAR_VARS)
#
#LOCAL_MODULE:= libcurl
#
#CURL_CFLAGS := -Wpointer-arith -Wwrite-strings -Wunused -Winline \
# -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -Wno-long-long \
# -Wfloat-equal -Wno-multichar -Wsign-compare -Wno-format-nonliteral \
# -Wendif-labels -Wstrict-prototypes -Wdeclaration-after-statement \
# -Wno-system-headers -DHAVE_CONFIG_H
#
#CURL_PATH := ../../../src/platform/android
#CURL_INC_PATH := $(LOCAL_PATH)/$(CURL_PATH)

#include ${CURL_INC_PATH}/curl/lib/Makefile.inc
#LOCAL_SRC_FILES := $(addprefix $(CURL_PATH)/curl/lib/,$(CSOURCES))
#LOCAL_CFLAGS += $(CURL_CFLAGS)
#LOCAL_C_INCLUDES += $(CURL_INC_PATH) $(CURL_INC_PATH)/curl/include $(CURL_INC_PATH)/curl/lib
#
#LOCAL_COPY_HEADERS_TO := libcurl
#LOCAL_COPY_HEADERS := $(addprefix $(CURL_PATH)/curl/include/curl/,$(HHEADERS))
#
#include $(BUILD_STATIC_LIBRARY)
#make libcurl done!~
