/*
 *  ngJNIUtils.cpp
 *  lib_nge_project
 *
 *  Modified by Chen Zhen on 12-10-10.
 *  Copyright 2011 Anansi Mobile. All rights reserved.
 *
 */

#include "ngJNIUtils.h"
#include <core/NGE_Types.h>
#include <core/NGE_NSL.h>
#include <android/log.h>
#include <pthread.h>

#if 0
#include <sys/time.h>
#include <GLES/gl.h>
#include <assert.h>

#include <extension/task/ngTaskManager.h>
#include <common/GLES/asContentScaleHelperGLES.h>
#endif

#pragma mark - java class

const char* JavaClassID[jclsNum] = {
    "com/anansimobile/nge/NextGenEngine",
    "com/anansimobile/nge/RootActivity",
    "com/anansimobile/nge/NGDevice",
    "com/anansimobile/nge/NGStringHelper",
    "com/anansimobile/nge/NGImagePicker",
    "com/anansimobile/nge/NGMediaPlayer",
    "com/anansimobile/nge/IAB/NGStoreKit",
    "com/anansimobile/nge/NGNotification",
    "com/anansimobile/nge/NGLabel",
    "com/anansimobile/nge/NGLocation",
    "com/anansimobile/nge/NGReachability",
    "com/anansimobile/nge/NGAlertDialog",
    "com/anansimobile/nge/NGTextInput",
    "com/anansimobile/nge/NGAppRater",
    "java/lang/String",
    "android/media/MediaPlayer",
};

typedef struct __T_JNIENVREF__ {
    pthread_t   m_threadID;
    JNIEnv*     m_env;
    int32       m_ref;
} JNIEnvRef;

static ngLinkedList sJNIEnvPool;

static JNIEnvRef* FindJNIEnv(pthread_t tid) {
    JNIEnvRef* pRetRef = NULL;
    ngIterator* itr = sJNIEnvPool.Iterator();
    while (itr->HasNext()) {
        JNIEnvRef* pRef = (JNIEnvRef*)itr->Next();
        if (pRef->m_threadID == tid) {
            pRetRef = pRef;
            break;
        }
    }
    itr->Destroy();
    
    return pRetRef;
}

JNIEnv* GetJNIEnv() {
#if 0
    JNIEnvRef* pRef = FindJNIEnv(pthread_self());
    return pRef->m_env;
#else
    void* pJNIEnv = NULL;
#if 0 //port to cocos2dx
    g_jvm->GetEnv(&pJNIEnv, JNI_VERSION_1_2);
#else
    g_jvm->GetEnv(&pJNIEnv, JNI_VERSION_1_4);
#endif
    NGASSERT(pJNIEnv != NULL);
    return (JNIEnv*)pJNIEnv;
#endif
}

static void RetainJNIEnv(JNIEnv* env) {
    pthread_t tid = pthread_self();
    JNIEnvRef* pRef = FindJNIEnv(tid);
    if (pRef == NULL) {
        pRef = DNEW(JNIEnvRef);
        pRef->m_threadID = tid;
        pRef->m_env = env;
        pRef->m_ref = 0;
        sJNIEnvPool.Add(pRef);

    } else {
        NGASSERT(env == pRef->m_env);
    }

    pRef->m_ref++;
}

static void ReleaseJNIEnv() {
    pthread_t tid = pthread_self();
    JNIEnvRef* pRef = FindJNIEnv(tid);
    if (pRef == NULL) {
        NGASSERT(0);
    } else {
        pRef->m_ref--;
        if (pRef->m_ref == 0) {
            sJNIEnvPool.Remove(pRef);
            SAFE_DELETE(pRef);
        }
    }
}

JavaVM* g_jvm = NULL;
ngArrayList g_jcls;

jclass getJavaClass(JavaClass jclsId) {
    if (jclsId <= jclsInvalid || jclsId >= jclsNum) {
        NGASSERT(0);
        return NULL;
    }
    
    return (jclass)g_jcls[jclsId];
}

void setJavaClass(JavaClass jclsId, jclass jcls) {
    
    /* jcls传入前需要调用NewGlobalRef。 */
    
    if (jclsId <= jclsInvalid || jclsId >= jclsNum) {
        NGASSERT(0);
        return;
    }
    
    JNIEnv* pEnv = GetJNIEnv();

    jclass old_cls = (jclass)g_jcls[jclsId];
    if (old_cls) {
        pEnv->DeleteGlobalRef(old_cls);
    }
    
    g_jcls[jclsId] = jcls;
}

void InitJavaClass() {
    NGASSERT(g_jcls.IsEmpty());

    JNIEnv* pEnv = GetJNIEnv();

    for (int32 i = 0; i < jclsNum; i++) {
        jclass cls = pEnv->FindClass(JavaClassID[i]);
        //NGASSERT(cls);
        if (cls == NULL) {

            if(pEnv->ExceptionCheck()) {
                pEnv->ExceptionClear();/* 清除异常 */
            }

            NG_DEBUG_LOG("[ngJNIUtils] init java class: %s (skipped)", JavaClassID[i]);
            g_jcls.Add(NULL);
        } else {
            NG_DEBUG_LOG("[ngJNIUtils] init java class: %s", JavaClassID[i]);
            g_jcls.Add(pEnv->NewGlobalRef(cls));
            pEnv->DeleteLocalRef(cls);
        }
    }
}

#if 1 //port to cocos2dx
void jni_init_nge_port(JNIEnv* env) {

    env->GetJavaVM(&g_jvm);

    RetainJNIEnv(env);

    NG_DEBUG_LOG("[ngJNIUtils] nativeInit");

    InitJavaClass();

    ReleaseJNIEnv();

}
#endif



#pragma mark - string

jstring stoJstring(const char* str, JNIEnv* jenv) {

    if (jenv == NULL) {
        jenv = GetJNIEnv();
    }
    
    if (str == NULL) {
        /* null str protect. */
        str = "";
    }

    static jclass strClass = getJavaClass(jclsString);
    jmethodID ctorID = jenv->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = jenv->NewByteArray(strlen(str));
    jenv->SetByteArrayRegion(bytes, 0, strlen(str), (jbyte*)str);
    jstring encoding = jenv->NewStringUTF("utf-8");
    jstring jstr = (jstring)(jenv->NewObject(strClass, ctorID, bytes, encoding));
    
    jenv->DeleteLocalRef(encoding);
    jenv->DeleteLocalRef(bytes);

    return jstr;
}

char* jstringTostring(jstring jstr, JNIEnv* jenv) {
    if (jenv == NULL) {
        jenv = GetJNIEnv();
    }
    
    char* rtn = NULL;
    static jclass strClass = getJavaClass(jclsString);
    jstring strencode = jenv->NewStringUTF("utf-8");
    jmethodID mid = jenv->GetMethodID(strClass, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)jenv->CallObjectMethod(jstr, mid, strencode);
    jsize alen = jenv->GetArrayLength(barr);
    jbyte* ba = jenv->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0)
    {
        rtn = (char*)DMALLOC(alen + 1);

        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    jenv->ReleaseByteArrayElements(barr, ba, 0);
    
    jenv->DeleteLocalRef(barr);
    jenv->DeleteLocalRef(strencode);
    
    return rtn;
}

jobject mainActivity() {
    JNIEnv* pEnv = GetJNIEnv();

    static jclass clsNGE = getJavaClass(jclsNextGenEngine);
    static jfieldID activityFieldID = pEnv->GetStaticFieldID(clsNGE,
                                                              "sMainActivity",
                                                              "Lcom/anansimobile/nge/RootActivity;");
    jobject activity = pEnv->GetStaticObjectField(clsNGE, activityFieldID);

    return activity;
}

jclass mainActivityClass() {
    return getJavaClass(jclsMainActivity);
}

JNIEnv* AttachCurrentJNIEnv() {
    JNIEnv* env = NULL;
    int32 ret = g_jvm->AttachCurrentThread(&env, NULL);
    if (ret < 0) {
        NG_DEBUG_LOG("attach env failed!~\n");
        NGASSERT(0);    //attach env failed!~
    }
    
    RetainJNIEnv(env);
    
    return env;
}

void DetachCurrentJNIEnv() {
    g_jvm->DetachCurrentThread();
    
#ifdef DEBUG
    JNIEnvRef* pRef = FindJNIEnv(pthread_self());
    NGASSERT(pRef->m_ref == 1);
#endif
    
    ReleaseJNIEnv();
}

#if 0

#pragma mark - 

/*
 * FIXME:
 *
 * this is a temp way to solve export java native interfce from static nge lib.
 * if I put this code in NDKImpl.cpp it will not export to final *.so lib, but if I put in this, it works. Curious! Maybe I made some mistake in Android.mk file or other, fix it later!
 *
 * [zhen.chen]
 */

#include <GLES/NGE_GLES.h>

#include <NGE_Core.h>
#include <game/ngActionInput.h>
#include <core/ngDevice.h>
#include <graphics/ngGraphics.h>
#include <game/ngTouchInput.h>
#include <graphics/ngTexturePool.h>
#include <game/ngSystemFontManager.h>

#define USE_CLOCK   0

#define USE_DEPTH_BUFFER 0
#define MAX_FRAME_TIME	100

#define RESUME_STATUS_INIT		-1
#define RESUME_STATUS_START		0
#define RESUME_STATUS_PAUSE		1

int     resumeStatus = RESUME_STATUS_INIT;
bool    running;
bool    isRetina;
bool    isPadRetina;
int     leftShiftBits;
bool    appInitilized;

bool getSnapshot = FALSE;
bool saveAlbum = FALSE;


#if USE_CLOCK
clock_t startFrameTime;
clock_t totalDuration;
#else
int64 startFrameTime;
int64 totalDuration;
#endif

int backingWidth = 0, backingHeight = 0;

#pragma mark - forward declare interface

void GetSnapshot() {
    //TODO: code here
}

void InitGLES() {
    
    glShadeModel(GL_SMOOTH);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
    
    //    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    //    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    //    
    //    NG_DEBUG_LOG("background size, width : %d, height : %d\n", backingWidth, backingHeight);
    
    //    glScissor(0, 0, backingWidth, backingHeight);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    
}

int64 GetCurrentTimeMS() {
    
    JNIEnv* pEnv = GetJNIEnv(); //每帧调用，所以如果需要优化，这个值可以固定而不是动态获取。
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "getCurrentTimeMS", "()J");
    jlong ms = pEnv->CallStaticLongMethod(cls, mid);

    return ms;
}

void InitApp() {
    
    NG_DEBUG_LOG("InitApp: background size, width : %d, height : %d\n", backingWidth, backingHeight);
    
    ngDevice* pDevice = (ngDevice::CreateInstance());

#if ENABLE_CONTENT_SCALE
	ngApplet* pApplet = ngApplet::CreateInstance();
    InitContentSize(backingWidth, backingHeight);
#else
    //here init to origin screen dimession.
	pDevice->SetDeviceDimesion(backingWidth, backingHeight);
	
	ngApplet* pApplet = ngApplet::CreateInstance();
#endif
	pApplet->Initialize();
	
#if USE_CLOCK
    startFrameTime = clock() / CLOCK_PER_MS;
#else
	startFrameTime = GetCurrentTimeMS();
#endif
	totalDuration = 0;
	
	resumeStatus = RESUME_STATUS_START;
    
}

void StartApp() {
	
	if (resumeStatus == RESUME_STATUS_PAUSE) {
		ngApplet* pApplet = ngApplet::GetInstance();
		pApplet->HandleResume();
		resumeStatus = RESUME_STATUS_START;
	}
	
	running = TRUE;
}

void PauseApp() {
	running = FALSE;
    
	if (resumeStatus == RESUME_STATUS_START) {
		ngApplet* pApplet = ngApplet::GetInstance();
		pApplet->HandlePause();
		resumeStatus = RESUME_STATUS_PAUSE;
	}
}

void ResumeApp() {
    if (resumeStatus == RESUME_STATUS_PAUSE) {
		ngApplet* pApplet = ngApplet::GetInstance();
		pApplet->HandleResume();
		resumeStatus = RESUME_STATUS_START;
	}
    
    running = TRUE;
}

#define CLOCK_PER_MS    (CLOCKS_PER_SEC / 1000)

#if 0 //test code
#include "ngImage.h"
ngImage* pImg = NULL;
#endif

#pragma mark - Game Loop Render

void Render() {

//    if (!running) {
//        return;
//    }
    
#if USE_CLOCK
    clock_t curFrameTime = clock() / CLOCK_PER_MS;
	long deltaTime = (curFrameTime - startFrameTime) - totalDuration;
	totalDuration += deltaTime;
#else
    int64 curFrameTime = GetCurrentTimeMS();
	int deltaTime = (curFrameTime - startFrameTime - totalDuration);
	totalDuration += deltaTime;
#endif
    
//    NG_DEBUG_LOG("=============\nstartFrameTime: %16lld\ncurFrameTime: %16lld\ndeltaTime: %16d\ntotalDuration: %16lld\n============\n", startFrameTime, curFrameTime, deltaTime, totalDuration);
    
	ngApplet* pApplet = ngApplet::GetInstance();
	if (pApplet)
	{
		pApplet->HandleBackgroundTimer(deltaTime);
		if (deltaTime > MAX_FRAME_TIME) {
			deltaTime = MAX_FRAME_TIME;
#if USE_CLOCK
		} else if (deltaTime < 1000 / kFramePerSecond) {
            deltaTime = 1000 / kFramePerSecond;
#endif
        }
		pApplet->HandleTimer(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pApplet->HandleDraw();
	}

    if (getSnapshot) {
        GetSnapshot();
    }
}

#pragma mark - jni interface implement

#include <android/thread/ngThreadMessageHandler.h>

#pragma mark game pause/resume

class ngGamePauseMessage : public ngThreadMessage {
public:
    virtual void HandleMessageData() {
        PauseApp();
    }
};

class ngGameResumeMessage : public ngThreadMessage {
public:
    virtual void HandleMessageData() {
        ResumeApp();
    }
};

#include "jni/com_anansimobile_nge_RootActivity.h"
JNIEXPORT void JNICALL Java_com_anansimobile_nge_RootActivity_onGamePause
(JNIEnv *, jobject) {
    ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(ngGamePauseMessage));
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_RootActivity_onGameResume
(JNIEnv *, jobject) {
    ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(ngGameResumeMessage));
}

#pragma mark game system back event
#include <extension/ngSystemBackEventHook.h>
class ngSystemBackPressedEvent : public ngThreadMessage {
public:
    virtual void HandleMessageData() {
        if (!ngSystemBackEventHook::GetInstance()->HandleSystemBackEvent()) {
            //返回FALSE，没有处理返回事件，回调系统处理
            //<<
            JNIEnv* pEnv = GetJNIEnv();
            jobject jobjMainActivity = mainActivity();
            jclass jclsMainActivity = mainActivityClass();
            jmethodID jmidSystemBackEvent = pEnv->GetMethodID(jclsMainActivity, "handleBackPressed", "()V");
            pEnv->CallVoidMethod(jobjMainActivity, jmidSystemBackEvent);
            //>>
        }
    }
};

JNIEXPORT void JNICALL Java_com_anansimobile_nge_RootActivity_HandleSystemBackEvent
(JNIEnv *, jobject) {
    ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(ngSystemBackPressedEvent));
}

#pragma mark game loop
#include "jni/com_anansimobile_nge_NDKRenderer.h"

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NDKRenderer_nativeInit(JNIEnv* env, jobject cls) {
    
    env->GetJavaVM(&g_jvm);
    
    RetainJNIEnv(env);
    
    NG_DEBUG_LOG("nativeInit\n");
    
    InitGLES();
    
    if (!appInitilized) {
        InitJavaClass();
    } else {
        //re create texture data if need
        //this is because Androind "EGL context lost" problem.
        ngTexturePool::GetInstance()->ReCreateAllTexture();
        ngSystemFontManager::GetInstance()->ClearAllFontCache();
    }
    
    ReleaseJNIEnv();
    
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NDKRenderer_nativeResize(JNIEnv* env, jobject cls, jint w, jint h) {
    
    RetainJNIEnv(env);

    NG_DEBUG_LOG("nativeResize, width : %d, height : %d\n", w, h);
    
    backingWidth    = w;
    backingHeight   = h;
    glScissor(0, 0, backingWidth, backingHeight);
    
    if (!appInitilized) {

        InitApp();
        StartApp();
        
        appInitilized = TRUE;
        
    } else {

        _INIT_CONTENT_SIZE(backingWidth, backingHeight);
    }
    
    /* temp set keyboard height. */
    //<<
    if (backingWidth > backingHeight) {
        //landscape
        ngDevice::GetInstance()->SetKeyboardHeight(_KEYBOARD_POS((float)backingHeight * 0.7f));
    } else {
        //portrait
        ngDevice::GetInstance()->SetKeyboardHeight(_KEYBOARD_POS((float)backingHeight * 0.5f));
    }
    //>>
    
    ngGraphics::GetInstance()->Initialize();
    
    ReleaseJNIEnv();
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NDKRenderer_nativeRender(JNIEnv* env, jobject cls) {
    
    RetainJNIEnv(env);
    //NG_DEBUG_LOG("nativeRender");

    _RENDER_CONTENT(Render);

    ReleaseJNIEnv();

}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NDKRenderer_nativeDone(JNIEnv* env, jobject cls) {
    
//        RetainJNIEnv(env);
    NG_DEBUG_LOG("nativeDone");
//        ReleaseJNIEnv();
    
}
    

#pragma mark touch event
#include "jni/com_anansimobile_nge_NGGLSurfaceView.h"

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGGLSurfaceView_onTouchBegin
(JNIEnv* env, jobject cls, jint x, jint y) {
//    NG_DEBUG_LOG("[touch] touch begin (%d x %d)!~", x, y);
    
    //TODO: here should queue message to game loop.
    ngTouchInput* pTouchInput = ngTouchInput::GetInstance();
	if (pTouchInput) {
        pTouchInput->AddTouch(-1, //x,
                              -1, //y,
                              _TOUCH_POINT_X(x),
                              _TOUCH_POINT_Y(y),
                              TOUCH_EVENT_PRESSED);
	}
    
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGGLSurfaceView_onTouchMoved
(JNIEnv* env, jobject cls, jint prevX, jint prevY, jint x, jint y) {
    
//    NG_DEBUG_LOG("[touch] touch move (%d x %d) from (%d x %d)!~", x, y, nPrevTouchedX, nPrevTouchedY);
    
    //TODO: here should queue message to game loop.
    ngTouchInput* pTouchInput = ngTouchInput::GetInstance();
	if (pTouchInput) {
        pTouchInput->AddTouch(_TOUCH_POINT_X(prevX), //nPrevTouchedX,
                              _TOUCH_POINT_Y(prevY), //nPrevTouchedY,
                              _TOUCH_POINT_X(x),
                              _TOUCH_POINT_Y(y),
                              TOUCH_EVENT_MOVE);
	}
    
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGGLSurfaceView_onTouchEnd
(JNIEnv* env, jobject cls, jint x, jint y) {
//    NG_DEBUG_LOG("[touch] touch end (%d x %d)!~", x, y);
    
    //TODO: here should queue message to game loop.
    ngTouchInput* pTouchInput = ngTouchInput::GetInstance();
	if (pTouchInput) {
        pTouchInput->AddTouch(_TOUCH_POINT_X(x),
                              _TOUCH_POINT_Y(y),
                              _TOUCH_POINT_X(x),
                              _TOUCH_POINT_Y(y),
                              TOUCH_EVENT_RELEASED);
	}
    
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGGLSurfaceView_onTouchCanceled
(JNIEnv* env, jobject cls) {
//    NG_DEBUG_LOG("[touch] touch canceled!~");
    //TODO: here should queue message to game loop if need.
}

#pragma mark texture
#include "jni/com_anansimobile_nge_NextGenEngine.h"
#include <android/ngTextureAndroid.h>
JNIEXPORT void JNICALL Java_com_anansimobile_nge_NextGenEngine_bitmapDataDidLoad
(JNIEnv* env, jclass cls, jint texturePtr, jobject bitmap, jint width, jint height) {
    RetainJNIEnv(env);
    ngTextureAndroid* pTexture = (ngTextureAndroid*)texturePtr;
    pTexture->RetainBitmapData(env, bitmap, width, height);
    ReleaseJNIEnv();
}

//#pragma mark textfield
//#include "ngTextFieldImplAndroid.h"
//void project_Main_BindTextField
//(JNIEnv* env, jobject cls, jint ptr, jobject jedittext) {
//    
//    jobject jobj = env->NewGlobalRef(jedittext);
//    ((ngTextFieldImplAndroid*)ptr)->BindTextField(jobj);
//}

#pragma mark image picker

#include <android/ImagePicker/ngImagePickerAndroid.h>
#include <android/bitmap.h>
#include "jni/com_anansimobile_nge_NGImagePicker.h"

class CBitmapDidLoadMessage : public ngThreadMessage {
public:
    CBitmapDidLoadMessage(ngImagePickerAndroid* pPicker, void* pData, int32 datalen, int32 w, int32 h)
    : m_pPicker(pPicker)
    , m_pData(pData)
    , m_datalen(datalen)
    , m_width(w)
    , m_height(h) {}
    virtual ~CBitmapDidLoadMessage() {
        SAFE_FREE(m_pData);
    }
private:
    void HandleMessageData() {
        m_pPicker->OnImageSelected(m_pData, m_datalen, m_width, m_height);
    }
private:
    ngImagePickerAndroid* m_pPicker;
    void* m_pData;
    int32 m_datalen;
    int32 m_width;
    int32 m_height;
};

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGImagePicker_bitmapDidLoad
(JNIEnv* env, jclass cls, jint listener, jobject bitmap) {
    
    RetainJNIEnv(env);
    AndroidBitmapInfo info;
    int32 ret;
    
    if (ANDROID_BITMAP_RESUT_SUCCESS > (ret = AndroidBitmap_getInfo(env, bitmap, &info))) {
        NG_DEBUG_LOG("[image picker] get bitmap info failed, code(%d)!~\n", ret);
        NGASSERT(0);
    }
    
    NG_DEBUG_LOG("[image picker] bitmap info: w(%d), h(%d), format(%d)\n", info.width, info.height, info.format);
    
    void* pBitmapData = NULL;
    
    int32 bitmapWidth = info.width;
    int32 bitmapHeight = info.height;
    int32 bitmapFormat = info.format;
    
    int32 dataSize = 0;
    
    switch(bitmapFormat) {		
        case ANDROID_BITMAP_FORMAT_RGBA_8888:
        case ANDROID_BITMAP_FORMAT_RGBA_4444:
            dataSize = bitmapWidth * bitmapHeight * 4;
            break;
        case ANDROID_BITMAP_FORMAT_RGB_565:
            dataSize = bitmapWidth * bitmapHeight * 4;
            break;
        case ANDROID_BITMAP_FORMAT_A_8:
            dataSize = bitmapWidth * bitmapHeight;
            break;				
        default:
            NG_DEBUG_LOG("Invalid pixel format!~\n");
			NGASSERT(0);
    }
    
    pBitmapData = DMALLOC(dataSize);
    
    void* tempData = NULL;
    
    if (ANDROID_BITMAP_RESUT_SUCCESS > (ret = AndroidBitmap_lockPixels(env, bitmap, &tempData))) {
        NG_DEBUG_LOG("[image picker] lock bitmap info failed, code(%d)!~\n", ret);
        NGASSERT(0);
    }
    
    memcpy(pBitmapData, tempData, dataSize);
    
    if (ANDROID_BITMAP_RESUT_SUCCESS > (ret = AndroidBitmap_unlockPixels(env, bitmap))) {
        NG_DEBUG_LOG("[image picker] unlock bitmap info failed, code(%d)!~\n", ret);
        NGASSERT(0);
    }
    
    NG_DEBUG_LOG("[image picker] pBitmapData: 0x%X, len: %d, %d x %d\n", pBitmapData, dataSize, bitmapWidth, bitmapHeight);

#if 0
    ngImagePickerAndroid* pImagePicker = (ngImagePickerAndroid*)listener;
    pImagePicker->OnImageSelected(pBitmapData, dataSize, bitmapWidth, bitmapHeight);
    SAFE_FREE(pBitmapData);
#else
    ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(CBitmapDidLoadMessage)((ngImagePickerAndroid*)listener,
                                                                                    pBitmapData,
                                                                                    dataSize,
                                                                                    bitmapWidth,
                                                                                    bitmapHeight));
#endif
    
    ReleaseJNIEnv();
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGImagePicker_bitmapDataDidLoad
(JNIEnv* env, jclass cls, jint listener, jbyteArray bytedata, jint datalen, jint width, jint height) {
    RetainJNIEnv(env);
	jbyte* pBytesArray = env->GetByteArrayElements(bytedata, 0);
	void* pData = DMALLOC(datalen);
	memcpy(pData, pBytesArray, datalen);
    
    ngImagePickerAndroid* pImagePicker = (ngImagePickerAndroid*)listener;
    pImagePicker->OnImageSelected(pData, datalen, width, height);
    SAFE_FREE(pData);
    ReleaseJNIEnv();
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGImagePicker_bitmapDidScaled
(JNIEnv* env, jclass cls, jint listener, jobject bitmap) {
    RetainJNIEnv(env);
    AndroidBitmapInfo info;
    int32 ret;
    
    if (ANDROID_BITMAP_RESUT_SUCCESS > (ret = AndroidBitmap_getInfo(env, bitmap, &info))) {
        NG_DEBUG_LOG("[image picker] get bitmap info failed, code(%d)!~\n", ret);
        NGASSERT(0);
    }
    
    NG_DEBUG_LOG("[image picker] bitmap info: w(%d), h(%d), format(%d)\n", info.width, info.height, info.format);
    
    void* pBitmapData = NULL;
    
    int32 bitmapWidth = info.width;
    int32 bitmapHeight = info.height;
    int32 bitmapFormat = info.format;
    
    int32 dataSize = 0;
    
    switch(bitmapFormat) {		
        case ANDROID_BITMAP_FORMAT_RGBA_8888:
        case ANDROID_BITMAP_FORMAT_RGBA_4444:
            dataSize = bitmapWidth * bitmapHeight * 4;
            break;
        case ANDROID_BITMAP_FORMAT_RGB_565:
            dataSize = bitmapWidth * bitmapHeight * 4;
            break;
        case ANDROID_BITMAP_FORMAT_A_8:
            dataSize = bitmapWidth * bitmapHeight;
            break;				
        default:
            NG_DEBUG_LOG("Invalid pixel format!~\n");
			NGASSERT(0);
    }
    
    pBitmapData = malloc(dataSize);
    
    void* tempData = NULL;
    
    if (ANDROID_BITMAP_RESUT_SUCCESS > (ret = AndroidBitmap_lockPixels(env, bitmap, &tempData))) {
        NG_DEBUG_LOG("[image picker] lock bitmap info failed, code(%d)!~\n", ret);
        NGASSERT(0);
    }
    
    memcpy(pBitmapData, tempData, dataSize);
    
    if (ANDROID_BITMAP_RESUT_SUCCESS > (ret = AndroidBitmap_unlockPixels(env, bitmap))) {
        NG_DEBUG_LOG("[image picker] unlock bitmap info failed, code(%d)!~\n", ret);
        NGASSERT(0);
    }
    
    //FIXME: should use tempData directly. [zhen.chen]
    ngImagePickerAndroid* pImagePicker = (ngImagePickerAndroid*)listener;
    pImagePicker->OnImageScaled(pBitmapData, dataSize, bitmapWidth, bitmapHeight);
    SAFE_FREE(pBitmapData);
    ReleaseJNIEnv();
}

class CBitmapDidScaledMessage : public ngThreadMessage {
public:
    CBitmapDidScaledMessage(ngImagePickerAndroid* pPicker, void* pData, int32 datalen, int32 w, int32 h)
    : m_pPicker(pPicker)
    , m_pData(pData)
    , m_datalen(datalen)
    , m_width(w)
    , m_height(h) {}
    virtual ~CBitmapDidScaledMessage() {
        SAFE_FREE(m_pData);
    }
private:
    void HandleMessageData() {
        m_pPicker->OnImageScaled(m_pData, m_datalen, m_width, m_height);
    }
private:
    ngImagePickerAndroid* m_pPicker;
    void* m_pData;
    int32 m_datalen;
    int32 m_width;
    int32 m_height;
};

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGImagePicker_bitmapDataDidScaled
(JNIEnv* env, jclass cls, jint listener, jbyteArray bytedata, jint datalen, jint width, jint height) {
    RetainJNIEnv(env);
	jbyte* pBytesArray = env->GetByteArrayElements(bytedata, 0);
	void* pData = DMALLOC(datalen);
	memcpy(pData, pBytesArray, datalen);
    
#if 0
    ngImagePickerAndroid* pImagePicker = (ngImagePickerAndroid*)listener;
    pImagePicker->OnImageScaled(pData, datalen, width, height);
    SAFE_FREE(pData);
#else    
    ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(CBitmapDidScaledMessage)((ngImagePickerAndroid*)listener,
                                                                                      pData,
                                                                                      datalen,
                                                                                      width,
                                                                                      height));
#endif
    
    ReleaseJNIEnv();
}

#pragma mark - multi-line label
#include <android/ngStringHelper.h>
#include "jni/com_anansimobile_nge_NGStringHelper.h"

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGStringHelper_AddStringLine
(JNIEnv *env, jclass, jint pContext, jstring jstrText) {

    if (pContext == 0x0) return;
    
    RetainJNIEnv(env);

    BreakTextContext* pCtx = (BreakTextContext*)pContext;
    char* str = jstringTostring(jstrText, env);
    (pCtx->m_cbAddLine)(pCtx->m_pHandler, str);
    DFREE(str);

    ReleaseJNIEnv();
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGStringHelper_UpdateLineHeight
(JNIEnv *env, jclass, jint pContext, jint height) {

    if (pContext == 0x0) return;
    
    RetainJNIEnv(env);

    BreakTextContext* pCtx = (BreakTextContext*)pContext;
    (pCtx->m_cbUpdateLineHeight)(pCtx->m_pHandler, (int32)height);

    ReleaseJNIEnv();
}

JNIEXPORT jstring JNICALL Java_com_anansimobile_nge_NGStringHelper_WordStartFlag
(JNIEnv *env, jclass, jint pContext) {

    if (pContext == 0x0) return 0x0;

    RetainJNIEnv(env);
    
    BreakTextContext* pCtx = (BreakTextContext*)pContext;
    NGCSTR wsf = (pCtx->m_cbWordStartFlag)();
    jstring jstrWSF = stoJstring(wsf, env);

    ReleaseJNIEnv();

    return jstrWSF;
}

JNIEXPORT jstring JNICALL Java_com_anansimobile_nge_NGStringHelper_WordEndFlag
(JNIEnv *env, jclass, jint pContext) {

    if (pContext == 0x0) return 0x0;
    
    RetainJNIEnv(env);
    
    BreakTextContext* pCtx = (BreakTextContext*)pContext;
    NGCSTR wef = (pCtx->m_cbWordEndFlag)();
    jstring jstrWEF = stoJstring(wef, env);
    
    ReleaseJNIEnv();
    
    return jstrWEF;
}

#pragma mark - web view
#include "jni/com_anansimobile_nge_NGWebView.h"
#include <android/ngWebViewAndroid.h>
JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGWebView_OnWebViewLoaded
(JNIEnv *env, jobject, jint ptr) {
    RetainJNIEnv(env);
    ngWebViewAndroid* pWebView = (ngWebViewAndroid*)ptr;
    pWebView->OnLoaded();
    ReleaseJNIEnv();
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGWebView_OnWebViewDismissed
(JNIEnv *env, jobject, jint ptr) {
    RetainJNIEnv(env);
    ngWebViewAndroid* pWebView = (ngWebViewAndroid*)ptr;
    pWebView->OnDismissed();
    ReleaseJNIEnv();
}

#pragma mark - storekit
#include "jni/com_anansimobile_nge_IAB_NGStoreKit.h"
#include <android/ngStoreKitAndroid.h>
JNIEXPORT jboolean JNICALL Java_com_anansimobile_nge_IAB_NGStoreKit_shouldConsumeProduct
(JNIEnv * env, jclass, jstring jstrProductId) {
    RetainJNIEnv(env);
    
    char* strId = jstringTostring(jstrProductId, env);
    jboolean ret = (jboolean)((ngStoreKitAndroid*)ngStoreKit::GetInstance())->ShouldConsumeProduct(strId);
    SAFE_FREE(strId);
    
    ReleaseJNIEnv();

    return ret;
}

#include <json/ngJsonElement.h>
#include <json/ngJsonRoot.h>

JNIEXPORT void JNICALL Java_com_anansimobile_nge_IAB_NGStoreKit_OnPurchaseFinished
(JNIEnv * env, jclass, jstring jstrReceipt, jstring jstrProductId) {
    RetainJNIEnv(env);
    
    char* receipt = jstringTostring(jstrReceipt, env);
    char* productId = jstringTostring(jstrProductId, env);
    
    /* here parse payment info from receipt.
     * try to use receipt directly later.
     */
    ngJsonRoot receiptJson;
    receiptJson.Parse(receipt);
    NGASSERT(receiptJson.GetType() == JSON_HASH);
    ngJsonHash* pHash = (ngJsonHash*)receiptJson.GetNode();
    ngJsonString *pStrPayload = (ngJsonString *)pHash->Get("payload");
    NGASSERT(pStrPayload != NULL);

    ngStoreKitAndroid* pStoreKit = (ngStoreKitAndroid*)ngStoreKit::GetInstance();
    if (pStoreKit->VerifyPayload(pStrPayload->GetCString())) {
        pStoreKit->OnPaymentFinished(receipt, productId);
    } else {
        pStoreKit->OnPaymentFailed();
    }
    
    SAFE_FREE(receipt);
    SAFE_FREE(productId);
    
    ReleaseJNIEnv();
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_IAB_NGStoreKit_OnPaymentFailed
(JNIEnv *env, jclass) {
    RetainJNIEnv(env);
    ngStoreKit::GetInstance()->OnPaymentFailed();
    ReleaseJNIEnv();
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_IAB_NGStoreKit_OnProduct
(JNIEnv *env, jclass, jstring jstrPrice) {
    RetainJNIEnv(env);
    char* price = jstringTostring(jstrPrice, env);
    ngStoreKit::GetInstance()->OnProduct(price);
    NG_DEBUG_LOG("[storekit] got product price: %s!~\n", price);
    SAFE_FREE(price);
    ReleaseJNIEnv();
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_IAB_NGStoreKit_OnProductListFinished
(JNIEnv * env, jclass) {
    RetainJNIEnv(env);
    ngStoreKit::GetInstance()->OnProductListFinished();
    ReleaseJNIEnv();
}

#pragma mark - UI Device Rule
#include "jni/com_anansimobile_nge_RootActivity.h"
#include <ui/device_rule/ngUIDeviceRuleManager.h>
JNIEXPORT void JNICALL Java_com_anansimobile_nge_RootActivity_setSystemUIFrame
(JNIEnv *env, jobject, jint w, jint h) {
    RetainJNIEnv(env);
    ngUIDeviceRuleManager::GetInstance()->InitSystemUIFrame(w, h);
    ReleaseJNIEnv();
}

#pragma mark - location
#include "jni/com_anansimobile_nge_NGLocation.h"
#include <android/location/ngLocationManagerAndroid.h>
JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGLocation_requestLocationSuccess
(JNIEnv *, jclass, jdouble latitude, jdouble longitude) {
    //TODO: send to main thread.
    ngLocationManager::GetInstance()->OnUpdated(latitude, longitude);
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGLocation_requestLocationFailed
(JNIEnv *, jclass, jint errCode) {
    //TODO: send to main thread.
    ngLocationManager::GetInstance()->OnFailed(errCode);
}

#pragma mark - reachability
#include "jni/com_anansimobile_nge_NGReachability.h"
#include <android/ngReachabilityAndroid.h>
class CReachabilityMessage : public ngThreadMessage {
public:
    CReachabilityMessage(int32 st) {
        m_status = st;
    }

    virtual ~CReachabilityMessage() {}
    
private:
    void HandleMessageData() {
        ((ngReachabilityAndroid*)ngReachability::GetInstance())->OnStatus(m_status);
    }
private:
    int32 m_status;
};

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGReachability_onNetworkStatusChange
(JNIEnv *, jclass, jint status) {
    ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(CReachabilityMessage)(status));
}

#pragma mark - alertdialog
#include "jni/com_anansimobile_nge_NGAlertDialog.h"
#include <ui/ngAlertDialog.h>
JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGAlertDialog_onAlertDialogButtonClick
(JNIEnv *, jclass, jint idx) {
    ngAlertDialog::GetInstance()->QueueEvent(NULL, EVENT_ID_ALERT_CLIKED, idx, 0);
}

#pragma mark - text input
#include "jni/com_anansimobile_nge_NGTextInputConn.h"
#include <android/ngTextInputAndroid.h>

class CTextInputMessage : public ngThreadMessage {
public:
    CTextInputMessage(int32 responder, NGCSTR text, int32 newCursorPos) {
        m_responder = responder;
        m_text = text;
        m_newCursorPos = newCursorPos;
        
        m_event = -1;
    }

    CTextInputMessage(int32 responder, int32 event) {
        m_responder = responder;
        m_event = event;
    }

    virtual ~CTextInputMessage() {

    }
    
private:
    void HandleMessageData() {
        if (m_event == -1) {
            ngTextInputAndroid* pTextInput = (ngTextInputAndroid*)m_responder;
            pTextInput->AppendText(m_text);
        } else {
            ngTextInputAndroid* pTextInput = (ngTextInputAndroid*)m_responder;
            pTextInput->HandleInputKeyEvent((TextInputKeyEvent)m_event);
        }
    }
private:
    int32   m_responder;
    ngStringV2 m_text;
    int32 m_newCursorPos;
    int32 m_event;
};

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGTextInputConn_nativeCommitText
(JNIEnv * pEnv, jobject, jint responder, jstring text, jint newCorsorPos) {
    char* pText = jstringTostring(text, pEnv);
    ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(CTextInputMessage)(responder, pText, newCorsorPos));
    DFREE(pText);
}

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGTextInputConn_nativeSendKeyEvent
(JNIEnv *, jobject, jint responder, jint event) {
    ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(CTextInputMessage)(responder, event));
}

#pragma mark - app rater
#include "jni/com_anansimobile_nge_NGAppRater.h"
#include <httpclient/ngAppRater.h>
class CAppRaterMessage : public ngThreadMessage {
public:
    CAppRaterMessage(int32 rater) {
        m_pRater = rater;
    }
    
    virtual ~CAppRaterMessage() {
        
    }
    
private:
    void HandleMessageData() {
        if (m_pRater) {
//            ngAppRater* pRater = (ngAppRater*)ctxPtr;
//            pRater->OnTapRate();
            ngAppRater::GetInstance()->OnTapRate();
        }
    }
private:
    int32   m_pRater;
};

JNIEXPORT void JNICALL Java_com_anansimobile_nge_NGAppRater_onTapRate
(JNIEnv *env, jclass, jint ctxPtr) {
    RetainJNIEnv(env);

    ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(CAppRaterMessage)(ctxPtr));
    
    ReleaseJNIEnv();
}

#endif