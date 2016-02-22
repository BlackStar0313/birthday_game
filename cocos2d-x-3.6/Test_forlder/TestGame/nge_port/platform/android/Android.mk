LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := nge_port_static

LOCAL_MODULE_FILENAME := libnge_port

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../core/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../extension/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../game/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../httpclient/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../io/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../io/minizip/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../json/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../json/json-c/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../lib/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../lib/lua/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../math/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../ncal/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../nsl/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../platform/android/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../platform/android/network/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../platform/android/network/curl/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../system/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../ui/*.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../../utils/*.cpp)) \

# $(NGE_PORT_INC_PATH)/ui is a temp fix here, remove it later. [zhen.chen]

NGE_PORT_INC_PATH := $(LOCAL_PATH)/../..
LOCAL_EXPORT_C_INCLUDES := \
	$(NGE_PORT_INC_PATH) \
	$(NGE_PORT_INC_PATH)/ui \
	$(NGE_PORT_INC_PATH)/platform \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/external/chipmunk/include/constraints \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/external/chipmunk/include/chipmunk \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/extensions \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx/include \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx/platform/android \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx/kazmath/include \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx/platform/third_party/android/prebuilt/libcurl/include \

LOCAL_C_INCLUDES := \
	$(NGE_PORT_INC_PATH) \
	$(NGE_PORT_INC_PATH)/ui \
	$(NGE_PORT_INC_PATH)/platform \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/external/chipmunk/include/constraints \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/external/chipmunk/include/chipmunk \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/extensions \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx/include \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx/platform/android \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx/kazmath/include \
	$(LOCAL_PATH)/../../../cocos2d-x-2.2.2/cocos2dx/platform/third_party/android/prebuilt/libcurl/include \
	
LOCAL_EXPORT_CFLAGS := \
	-Wno-psabi\
	-DNGE_PLATFORM_ANDROID \
	-DUSE_FILE32API \
	
#判断debug参数
ifdef ANDROID_DEBUG
	ifeq ($(ANDROID_DEBUG), 1)
		LOCAL_EXPORT_CFLAGS += -DDEBUG_LOG -DDEBUG
	endif
endif

LOCAL_CFLAGS := $(LOCAL_EXPORT_CFLAGS)


LOCAL_LDLIBS := -lGLESv2 \
                -llog \
                -lz

LOCAL_EXPORT_LDLIBS := -lGLESv2 \
                       -llog \
                       -lz

#LOCAL_CFLAGS := -Wno-psabi -DUSE_FILE32API
#LOCAL_EXPORT_CFLAGS := -Wno-psabi -DUSE_FILE32API

include $(BUILD_STATIC_LIBRARY)

$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)

