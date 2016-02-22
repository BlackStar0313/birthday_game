#clear

echo_usage()
{
    echo "Usage: build.sh [-n][-s][-v][-d][-h]"
    echo "Options:"
    echo "	-n : ndk root"
    echo "	-s : sdk root"
    echo "	-v : device serial number"
    echo "	-d : debug mode"
    echo "	-h : usage"
}

#here need var ANDROID_NDK_ROOT and ANDROID_SDK_ROOT

_ndk_root=$ANDROID_NDK_ROOT
_sdk_root=$ANDROID_SDK_ROOT
_device_sn=''

debug_mode=0

while getopts ":n:s:v:dhx" opt
do
        case $opt in
                n ) _ndk_root=$OPTARG;;
                s ) _sdk_root=$OPTARG;;
                v ) _device_sn=$OPTARG;;
                d ) debug_mode=1;;
                h ) echo_usage
                	exit 0;;
                ? ) echo "error"
                    exit 1;;
        esac
done

if [ ! $_ndk_root ]
then
	echo_usage;
	exit;
fi

cur_path="$(dirname $0)"

rm -rf $cur_path/libs/armeabi/*
rm -rf $cur_path/obj/local/armeabi/lib*.a
$_ndk_root/ndk-build NDK_DEBUG=$debug_mode ANDROID_DEBUG=$debug_mode

if [ $? -ne 0 ]
then
	echo "build failed!~"
	exit 1
fi

rm -rf $cur_path/libs/armeabi/gdb*

if [[ -n $_device_sn && -n $_sdk_root ]]
then
	echo "push lib to device \"${2}\"..."
	$_sdk_root/platform-tools/adb -s $_device_sn push libs/armeabi/libproject_package.so /data/data/com.anansimobile.project/lib/
	$_sdk_root/platform-tools/adb -s $_device_sn shell am start -n com.anansimobile.project/com.anansimobile.project.Main
fi


