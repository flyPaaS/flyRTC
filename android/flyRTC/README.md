# FlyRTC 音视频通话

## 如何调试

### 重新生成编译文件

``` bash
./build_android.sh clean webrtc
./build_android.sh gen

```
### 修改webrtc/src/out_android_armv7/Release/obj/all.ninja
将文件93行内容修改如下：

``` makefile
93c93
<     -Os -g -fdata-sections -ffunction-sections -fomit-frame-pointer $
---
>     -Os -fno-ident -fdata-sections -ffunction-sections -fomit-frame-pointer $

```
即删除 -fno-indent，添加-g


### 编译webrtc
``` bash
./build_android.sh webrtc
```

### 修改app/build.gradle

``` gradle
android {
    externalNativeBuild {
            ndkBuild {
                path '../../flyrtc-webrtc/UGo/prj/android/jni/Android.mk'
        }
    }
}
```
其中path指向webrtc的实际目录

### 删除app/libs目录


### 用AS重新导入工程
