using System;
using MicroMsg.sdk;

namespace NextGenEngineMetro
{
    namespace Extra
    {
        public static class asWXHelperImplMetro
        {
            private static asWXHelperMetroImplRef mRef = asWXHelperMetroImplRef.GetInstance();
            private static IWXAPI mApi = null;
            private static bool mInitialized = false;

            /*
             * 初始化操作，参数为AppID
             * 
             * 如果参数留空，则必须在其他操作调用前执行asWXHelper::RegisterApp()。
             * 
             */
            public static void init(string appID = null)
            {
                if (appID != null)
                {
                    mApi = WXAPIFactory.CreateWXAPI(appID);
                    System.Diagnostics.Debugger.Log(0, "[asWXHelperImplMetro]", "init");
                }

                mRef.cb_RegisterApp += callback_RegisterApp;
                mRef.cb_IsWXAppInstalled += callback_IsWXAppInstalled;
                mRef.cb_IsWXAppSupportAPI += callback_IsWXAppSupportAPI;

                mRef.cb_ShareText += callback_ShareText;
                mRef.cb_ShareUrl += callback_ShareUrl;
                mRef.cb_ShareImage += callback_ShareImage;
                mRef.cb_ShareImageUrl += callback_ShareImageUrl;

                mInitialized = true;
            }

            public static void handleWXResp(SendMessageToWX.Resp resp)
            {
                mRef.OnSendToWXResp(resp.ErrCode, resp.ErrStr);
            }

            public static void callback_RegisterApp(string appID)
            {
                if (mApi == null)
                {
                    mApi = WXAPIFactory.CreateWXAPI(appID);
                    System.Diagnostics.Debugger.Log(0, "[asWXHelperImplMetro]", "callback_RegisterApp");
                }
            }

            public static bool callback_IsWXAppInstalled()
            {
                /* there is no interface to check wx app install status, so here always return true. */
                return true;
            }

            public static bool callback_IsWXAppSupportAPI()
            {
                return false;
            }

            public static void callback_ShareText(string title, string content, int type)
            {
                SendMessageToWX.Req req = new SendMessageToWX.Req(msg_text(title, content), getScene(type));
                SendReqInternal(req);
            }

            public static void callback_ShareUrl(string title, string desc, string url, byte[] thumbData, int type)
            {
                SendMessageToWX.Req req = new SendMessageToWX.Req(msg_url(title, desc, url, thumbData), getScene(type));
                SendReqInternal(req);
            }

            public static void callback_ShareImage(byte[] data, byte[] thumbData, string title, string desc, int type)
            {
                SendMessageToWX.Req req = new SendMessageToWX.Req(msg_image(title, desc, data, thumbData), getScene(type));
                SendReqInternal(req);
            }

            public static void callback_ShareImageUrl(string imgUrl, byte[] thumbData, string title, string desc, int type)
            {
                SendMessageToWX.Req req = new SendMessageToWX.Req(msg_image(title, desc, imgUrl, thumbData), getScene(type));
                SendReqInternal(req);
            }

            public static void SendReqInternal(SendMessageToWX.Req req)
            {
                try
                {
                    mApi.SendReq(req);
                }
                catch (WXException e)
                {
                    System.Diagnostics.Debugger.Log(0, "[asWXHelper]", e.ToString());
                }
            }

            public static int getScene(int type)
            {
                switch (type) {
                    case 0://asWXHelper::shareTypeFriend:
                        return SendMessageToWX.Req.WXSceneSession;
                    case 1://asWXHelper::shareTypeTimeLine:
                        return SendMessageToWX.Req.WXSceneTimeline;
                    default:
                        return SendMessageToWX.Req.WXSceneSession;
                }
            }

            //文本数据的组装
            public static WXBaseMessage msg_text(string title, string content)
            {
                WXTextMessage msg = new WXTextMessage();
                msg.Title = title;
                //msg.Description = "这是一个文本";
                msg.ThumbData = null;
                msg.Text = content;
                return msg;
            }

            //url数据
            public static WXBaseMessage msg_url(string title, string desc, string url, byte[] thumbData)
            {
                WXWebpageMessage msg = new WXWebpageMessage();
                msg.Title = title;
                msg.Description = desc;
                msg.WebpageUrl = url;

                if (thumbData != null)
                {
                    msg.ThumbData = thumbData;
                }

                return msg;
            }

            ///图片数据的组装
            public static WXBaseMessage msg_image(string title, string desc, byte[] data, byte[] thumbData)
            {
                WXImageMessage msg = new WXImageMessage();
                msg.Title = title;
                msg.Description = desc;
                msg.ThumbData = thumbData;

                //图片的网络链接，ImageUrl和ImageData二者取其一，不要同时都填值
                //msg.ImageUrl = "http://soso7.gtimg.cn/sosopic/0/4085165533549977883/150";
                msg.ImageData = data;

                return msg;
            }

            ///图片url数据的组装
            public static WXBaseMessage msg_image(string title, string desc, string imgUrl, byte[] thumbData)
            {
                WXImageMessage msg = new WXImageMessage();
                msg.Title = title;
                msg.Description = desc;
                msg.ThumbData = thumbData;

                //图片的网络链接，ImageUrl和ImageData二者取其一，不要同时都填值
                msg.ImageUrl = imgUrl;
                //msg.ImageData = data;

                return msg;
            }
        }
    }
}