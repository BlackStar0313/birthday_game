package com.anansimobile.extra.social.weixin;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.widget.Toast;

import com.tencent.mm.sdk.openapi.BaseReq;
import com.tencent.mm.sdk.openapi.BaseResp;
import com.tencent.mm.sdk.openapi.ConstantsAPI;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.IWXAPIEventHandler;
import com.tencent.mm.sdk.openapi.SendMessageToWX;
import com.tencent.mm.sdk.openapi.WXAPIFactory;
import com.tencent.mm.sdk.openapi.WXImageObject;
import com.tencent.mm.sdk.openapi.WXMediaMessage;
import com.tencent.mm.sdk.openapi.WXTextObject;
import com.tencent.mm.sdk.openapi.WXWebpageObject;
import com.tencent.mm.sdk.openapi.SendAuth;

public class WXHelper {

	private static Activity sCtx = null;
	private static IWXAPI api = null;
	
	private static final int THUMB_SIZE = 150;	//thumb image size.
	
	public static void init(Activity ctx) {
		sCtx = ctx;
	}
	
	public static void registerWX(String appID/*, boolean checksignature*/) {
		api = WXAPIFactory.createWXAPI(sCtx, appID, false/*, checksignature*/);
		api.registerApp(appID);
	}
	
	private static native void OnSendMsgToWXDone();
	private static native void OnSendMsgToWXCanceled();
	private static native void OnSendMsgToWXFailed(int errCode, String errStr);
	
	private static IWXAPIEventHandler sEventHandler = new IWXAPIEventHandler() {

		@Override
		public void onReq(BaseReq req) {
			switch (req.getType()) {
			case ConstantsAPI.COMMAND_GETMESSAGE_FROM_WX:
				//goToGetMsg();
				//TODO: code here if need.
				break;
			case ConstantsAPI.COMMAND_SHOWMESSAGE_FROM_WX:
				//goToShowMsg((ShowMessageFromWX.Req) req);
				//TODO: code here if need.
				break;
			default:
				break;
			}
		}

		@Override
		public void onResp(BaseResp resp) {
			//int result = 0;
			String tip ="";
			
			switch (resp.errCode) {
			case BaseResp.ErrCode.ERR_OK:
				//result = R.string.errcode_success;
				//tip = "分享成功";
				WXHelper.OnSendMsgToWXDone();
				break;
			case BaseResp.ErrCode.ERR_USER_CANCEL:
				//result = R.string.errcode_cancel;
				//tip = "分享取消";
				WXHelper.OnSendMsgToWXCanceled();
				break;
			case BaseResp.ErrCode.ERR_AUTH_DENIED:
				//result = R.string.errcode_deny;
				//tip = "分享失败";
				WXHelper.OnSendMsgToWXFailed(resp.errCode, resp.errStr);
				break;
			default:
				//result = R.string.errcode_unknown;
				WXHelper.OnSendMsgToWXFailed(resp.errCode, resp.errStr);
				break;
			}
			
			//Toast.makeText(sCtx, result, Toast.LENGTH_LONG).show();
			//Toast.makeText(sCtx, tip, Toast.LENGTH_LONG).show();
		}
		
	};
	
	public static void handleIntent(Intent intent) {

		if (api == null) return;

        api.handleIntent(intent, sEventHandler);
	}
	
	public static boolean isWXAppInstalled() {
		
		if (api == null) return false;
		
		return api.isWXAppInstalled();
	}

	public static boolean isWXAppSupported() {

		if (api == null) return false;

		return api.isWXAppSupportAPI();
	}
	
	private static int getWXScene(int type) {
		switch (type) {
		case 0:
			return SendMessageToWX.Req.WXSceneSession;
		case 1:
			return SendMessageToWX.Req.WXSceneTimeline;
		default:
			return SendMessageToWX.Req.WXSceneSession;
		}
	}
	
	private static String buildTransaction(final String type) {
		return (type == null) ? String.valueOf(System.currentTimeMillis()) : type + System.currentTimeMillis();
	}

	private static void shareText(String text, int type) {

		if (api == null) return;

		// 初始化一个WXTextObject对象
		WXTextObject textObj = new WXTextObject();
		textObj.text = text;

		// 用WXTextObject对象初始化一个WXMediaMessage对象
		WXMediaMessage msg = new WXMediaMessage();
		msg.mediaObject = textObj;
		// 发送文本类型的消息时，title字段不起作用
		// msg.title = "Will be ignored";
		msg.description = text;

		// 构造一个Req
		SendMessageToWX.Req req = new SendMessageToWX.Req();
		req.transaction = buildTransaction("text"); // transaction字段用于唯一标识一个请求
		req.message = msg;
		req.scene = getWXScene(type);
		
		// 调用api接口发送数据到微信
		api.sendReq(req);
	}
	
	private static void shareUrl(String title, String desc, String url, byte[] thumbData, int type) {

		if (api == null) return;

		WXWebpageObject obj = new WXWebpageObject();
		obj.webpageUrl = url;

		// 用WXWebpageObject对象初始化一个WXMediaMessage对象
		WXMediaMessage msg = new WXMediaMessage();
		msg.mediaObject = obj;
		msg.title = title;
		msg.description = desc;
		
		if (thumbData != null) {
			msg.thumbData = thumbData;
		}

		// 构造一个Req
		SendMessageToWX.Req req = new SendMessageToWX.Req();
		req.transaction = buildTransaction("url"); // transaction字段用于唯一标识一个请求
		req.message = msg;
		req.scene = getWXScene(type);
		
		// 调用api接口发送数据到微信
		try {
			api.sendReq(req);
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}
	
	private static void shareImage(int[] rgbData, int width, int height, String title, String desc, int type) {

		if (api == null) return;

		Bitmap bmp = Bitmap.createBitmap(rgbData, width, height, Config.ARGB_8888);
		WXImageObject imgObj = new WXImageObject(bmp);
		
		WXMediaMessage msg = new WXMediaMessage();
		msg.mediaObject = imgObj;
		
		Bitmap thumbBmp = Bitmap.createScaledBitmap(bmp, THUMB_SIZE, THUMB_SIZE, true);
		bmp.recycle();
		msg.thumbData = Util.bmpToByteArray(thumbBmp, true);  // 设置缩略图

		SendMessageToWX.Req req = new SendMessageToWX.Req();
		req.transaction = buildTransaction("img");
		req.message = msg;
		req.scene = getWXScene(type);

		boolean ret = api.sendReq(req);

	}

	private static void shareImage(byte[] data, String title, String desc, int type) {

		if (api == null) return;

		WXImageObject imgObj = new WXImageObject();
		imgObj.imageData = data;
		
		WXMediaMessage msg = new WXMediaMessage();
		msg.mediaObject = imgObj;
		
		Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
		Bitmap thumbBmp = Bitmap.createScaledBitmap(bmp, THUMB_SIZE, THUMB_SIZE, true);
		bmp.recycle();
		msg.thumbData = Util.bmpToByteArray(thumbBmp, true);  // 设置缩略图

		SendMessageToWX.Req req = new SendMessageToWX.Req();
		req.transaction = buildTransaction("img");
		req.message = msg;
		req.scene = getWXScene(type);

		api.sendReq(req);


	}

	private static void shareImage(String imgUrl, String title, String desc, byte[] thumbData, int type) {

		if (api == null) return;

		WXImageObject imgObj = new WXImageObject();
		imgObj.imageUrl = imgUrl;
		
		WXMediaMessage msg = new WXMediaMessage();
		msg.mediaObject = imgObj;
		
		if (thumbData != null) {
			msg.thumbData = thumbData;
		}

		SendMessageToWX.Req req = new SendMessageToWX.Req();
		req.transaction = buildTransaction("img_url");
		req.message = msg;
		req.scene = getWXScene(type);

		api.sendReq(req);

	}
}
