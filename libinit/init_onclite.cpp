/*
   Copyright (c) 2016, The CyanogenMod Project
   Copyright (C) 2019 The LineageOS Project.
   Copyright (C) 2020-2021 The MoKee Open Source Project
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <cstdlib>
#include <fstream>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/_system_properties.h>

#include "vendor_init.h"
#include "property_service.h"

using android::base::GetProperty;
using std::string;
int property_set(const char *key, const char *value) {
    return __system_property_set(key, value);
}

void property_override(char const prop[], char const value[], bool add = true)
void property_override(string prop, string value)
{
    pi = (prop_info *) __system_property_find(prop);

    if (pi != nullptr)
        __system_property_update(pi, value.c_str(), value.size());
    else if (add)
        __system_property_add(prop.c_str(), prop.size(), value.c_str(), value.size());
}
/* From Magisk@native/jni/magiskhide/hide_utils.c */
static const char *cts_prop_key[] =
        { "ro.boot.vbmeta.device_state", "ro.boot.verifiedbootstate", "ro.boot.flash.locked",
          "ro.boot.veritymode", "ro.boot.warranty_bit", "ro.warranty_bit",
          "ro.debuggable", "ro.secure", "ro.build.type", "ro.build.tags",
          "ro.vendor.boot.warranty_bit", "ro.vendor.warranty_bit",
          "vendor.boot.vbmeta.device_state", nullptr };

static const char *cts_prop_val[] =
        { "locked", "green", "1",
          "enforcing", "0", "0",
          "0", "1", "user", "release-keys",
          "0", "0",
          "locked", nullptr };

static const char *cts_late_prop_key[] =
        { "vendor.boot.verifiedbootstate", nullptr };

static const char *cts_late_prop_val[] =
        { "green", nullptr };

static void workaround_cts_properties() {

    // Hide all sensitive props
    for (int i = 0; cts_prop_key[i]; ++i) {
		property_override(cts_prop_key[i], cts_prop_val[i]);
	}
	for (int i = 0; cts_late_prop_key[i]; ++i) {
		property_override(cts_late_prop_key[i], cts_late_prop_val[i]);
    }

}


void vendor_load_properties()
{
    string device, model, desc;

    string region = GetProperty("ro.boot.hwcountry", "");
    string hwversion = GetProperty("ro.boot.hwversion", "");

    if (region == "India" && hwversion == "1.19.0") {
        device = "onc";
        model = "Redmi Y3";
    } else {
        device = "onclite";
        model = "Redmi 7";
    }

    // Override all partitions' props
    string prop_partitions[] = { "", "odm.", "product.", "system.",
                    "system_ext.", "bootimage.", "vendor." };

    for (const string &prop : prop_partitions) {
        property_override(string("ro.product.") + prop + string("name"), device);
        property_override(string("ro.product.") + prop + string("device"), device);
        property_override(string("ro.product.") + prop + string("model"), model);
        property_override(string("ro.") + prop + string("build.product"), device);
    }

    // Set dalvik heap configuration
    string heapstartsize, heapgrowthlimit, heapsize, heapminfree,
            heapmaxfree, heaptargetutilization;

    struct sysinfo sys;
    sysinfo(&sys);

    if (sys.totalram > 3072ull * 1024 * 1024) {
        // Set for 4GB RAM
        // from - phone-xxhdpi-4096-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "256m";
        heapsize = "512m";
        heaptargetutilization = "0.6";
        heapminfree = "4m";
        heapmaxfree = "16m";
    } else if (sys.totalram > 2048ull * 1024 * 1024) {
        // Set for 3GB RAM
        // from - phone-xhdpi-2048-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "192m";
        heapsize = "512m";
        heaptargetutilization = "0.75";
        heapminfree = "2m";
        heapmaxfree = "8m";
    } else {
        // Set for 2GB RAM
        // from go_defaults_common.prop
        heapstartsize = "8m";
        heapgrowthlimit = "128m";
        heapsize = "256m";
        heaptargetutilization = "0.75";
        heapminfree = "2m";
        heapmaxfree = "8m";
    }

    property_override("dalvik.vm.heapstartsize", heapstartsize);
    property_override("dalvik.vm.heapgrowthlimit", heapgrowthlimit);
    property_override("dalvik.vm.heapsize", heapsize);
    property_override("dalvik.vm.heaptargetutilization", heaptargetutilization);
    property_override("dalvik.vm.heapminfree", heapminfree);
    property_override("dalvik.vm.heapmaxfree", heapmaxfree);

    // SafetyNet Workaround
    property_override("ro.boot.verifiedbootstate", "green");

    property_override("ro.oem_unlock_supported", "0");
 
   // Workaround SafetyNet
    workaround_cts_properties();

    // Override ro.control_privapp_permissions
    property_override("ro.control_privapp_permissions", "log");
}
