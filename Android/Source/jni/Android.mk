LOCAL_PATH := $(call my-dir)

$(shell scripts/copy_assets.sh)

include $(call all-subdir-makefiles)
