LOCAL_PATH:= $(call my-dir)
CMD_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := libpcap
LOCAL_SRC_FILES := libpcap.a

include $(PREBUILT_STATIC_LIBRARY)

include src/main/cmds/client-core/Android.mk

LOCAL_PATH := $(CMD_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := login
LOCAL_SRC_FILES := main.cpp

#LOCAL_LDLIBS := -llog

LOCAL_SHARED_LIBRARIES := client_core

LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE

include $(BUILD_EXECUTABLE)
