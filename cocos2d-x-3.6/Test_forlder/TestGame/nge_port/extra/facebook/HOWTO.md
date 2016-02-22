#facebook的SDK集成(iOS)

##库和资源

* 安装pkg文件，在文稿里找到解压好的FacebookSDK文件夹
* 将FacebookSDK.framework添加到工程项目中

##接口代码

###在项目中包含以下文件：

* ngFacebookShare.h/ngFacebookShare.cpp
* ngFacebookShareCocoa.h/ngFacebookShareCocoa.cpp

###主要接口实现

* 实现了调用facebook内置浏览器分享或者调用facebook应用分享的接口（取决于用户是否安装facebook应用）
* 调用分享接口前需要先设置一些基本信息（包括从facebook官方网址review通过的action和object，我们提供的imageURL供facebook访问图片显示等）
