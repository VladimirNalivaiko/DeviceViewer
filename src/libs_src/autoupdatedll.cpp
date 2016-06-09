#include "autoupdatedll.h"

DWORD CM_Interface_Notification_Handler(HCMNOTIFICATION hNotify, PVOID Context, CM_NOTIFY_ACTION Action, PCM_NOTIFY_EVENT_DATA EventData, DWORD EventDataSize) {
    my_Notification_Handler();

    return 0;
}
__declspec(dllexport) bool Init_DeviceInterface_Notification(wchar_t *pathToLibrary) {
    HMODULE hLib;
    hLib = LoadLibraryW(pathToLibrary);
    if (hLib == NULL) {
        return false;
    }

    (FARPROC &)my_CM_Register_Notification = GetProcAddress(hLib, "CM_Register_Notification");
    if (my_CM_Register_Notification == NULL) {
        return false;
    }

    (FARPROC &)my_CM_Unregister_Notification = GetProcAddress(hLib, "CM_Unregister_Notification");
    if (my_CM_Unregister_Notification == NULL) {
        return false;
    }

    return true;

}
__declspec(dllexport) bool Register_DeviceInterface_Notification(void(*Notification_Handler)(void)) {
    my_Notification_Handler = Notification_Handler;

    CM_NOTIFY_FILTER cmFilter = { 0 };
    cmFilter.cbSize = sizeof(cmFilter);
    cmFilter.Flags = CM_NOTIFY_FILTER_FLAG_ALL_INTERFACE_CLASSES;
    cmFilter.FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEINTERFACE;
    BYTE Context[256] = { 0 };
    if (!(my_CM_Register_Notification(&cmFilter, (PVOID)&Context, (PCM_NOTIFY_CALLBACK)CM_Interface_Notification_Handler, &NotifyContext) == CR_SUCCESS)) {
        return false;
    }
    return true;
}
__declspec(dllexport) void Unregister_DeviceInterface_Notification() {
    my_CM_Unregister_Notification(NotifyContext);

    NotifyContext = { 0 };
    my_CM_Register_Notification = NULL;
    my_CM_Unregister_Notification = NULL;
    my_Notification_Handler = NULL;
}
