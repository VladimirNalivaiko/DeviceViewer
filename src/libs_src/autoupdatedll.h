#ifndef AUTOUPDATEDLL_H
#define AUTOUPDATEDLL_H

#include "autoupdatedll_global.h"

#include <windows.h>
#include <Cfgmgr32.h>

#pragma data_seg(".SHARDATA")
    static HCMNOTIFICATION NotifyContext = { 0 };
    static CONFIGRET(*my_CM_Register_Notification)(PCM_NOTIFY_FILTER, PVOID, PCM_NOTIFY_CALLBACK, PHCMNOTIFICATION) = NULL;
    static CONFIGRET(*my_CM_Unregister_Notification)(HCMNOTIFICATION) = NULL;
    static void(*my_Notification_Handler)(void) = NULL;
#pragma data_seg()
#pragma comment(linker, "/section:.SHARDATA,rws")

extern "C" __declspec(dllexport) bool Init_DeviceInterface_Notification(wchar_t *);
extern "C" __declspec(dllexport) bool Register_DeviceInterface_Notification(void(*)(void));
extern "C" __declspec(dllexport) void Unregister_DeviceInterface_Notification();

#endif // AUTOUPDATEDLL_H
