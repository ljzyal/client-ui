APP_PROJECT_PATH := $(NDK_PROJECT_PATH)

APP_ABI := armeabi armeabi-v7a

APP_STL := stlport_shared
APP_STL := stlport_static

#ifndef APP_BUILD_SCRIPT
#  ifeq (null,$(NDK_PROJECT_PATH))
#    $(call __ndk_info,NDK_PROJECT_PATH==null.  Please explicitly set APP_BUILD_SCRIPT.)
#    $(call __ndk_error,Aborting.)
#  endif
#  APP_BUILD_SCRIPT := $(APP_PROJECT_PATH)/jni/Android.mk
#endif
