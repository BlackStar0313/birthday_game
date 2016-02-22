clear

echo_usage()
{
    echo "Usage: crash_stack.sh [-n][-s][-v][-h]"
    echo "Options:"
    echo "	-n : ndk root"
    echo "	-s : sdk root"
    echo "	-v : device serial number"
    echo "	-h : usage"
}

_ndk_root=$ANDROID_NDK_ROOT
_sdk_root=$ANDROID_SDK_ROOT

while getopts ":n:s:v:h" opt
do
        case $opt in
                n ) _ndk_root=$OPTARG;;
                s ) _sdk_root=$OPTARG;;
                v ) _device_sn=$OPTARG;;
                h ) echo_usage
                	exit 0;;
                ? ) echo "error"
                    exit 1;;
        esac
done

if [[ ! $_ndk_root || ! $_sdk_root ]]
then
	echo_usage;
	exit;
fi

#easy use absolute path.
#adb=/android/android-sdk-mac_86/platform-tools/adb

adb=$_sdk_root/platform-tools/adb

if [ $_device_sn ]
then
	$adb -s $1 logcat | $_ndk_root/ndk-stack -sym ./obj/local/armeabi/
else
	$adb logcat | $_ndk_root/ndk-stack -sym ./obj/local/armeabi/
fi