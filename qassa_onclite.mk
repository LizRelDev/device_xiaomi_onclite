#
# Copyright (C) 2019-2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/product_launched_with_p.mk)

# Inherit from onclite device
$(call inherit-product, device/xiaomi/onclite/device.mk)

# Inherit QASSA stuff.
$(call inherit-product, vendor/qassa/config/common_full_phone.mk)

QASSA_MAINTAINER := LizRelDev
TARGET_FACE_UNLOCK_SUPPORTED := true
TARGET_USE_PIXEL_CHARGER := true

# Boot Animation
TARGET_BOOT_ANIMATION_RES := 720


# Device identifier. This must come after all inclusions.
PRODUCT_NAME := qassa_onclite
PRODUCT_DEVICE := onclite
PRODUCT_BRAND := Xiaomi
PRODUCT_MODEL := Redmi 7
PRODUCT_MANUFACTURER := Xiaomi

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_NAME="onc" \
    TARGET_DEVICE="onc" \
    PRIVATE_BUILD_DESC="raven-user 13 TP1A.221005.002 9012097 release-keys"

BUILD_FINGERPRINT := "google/raven/raven:13/TP1A.221005.002/9012097:user/release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi
