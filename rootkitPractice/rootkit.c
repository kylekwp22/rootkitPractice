#include <ntddk.h>

//Callback routine executed on process creation or exit.
void ProcessNotifyCallBackRoutine(
	IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create
)
{
	UNREFERENCED_PARAMETER(ParentId); //Unreference ParentId which is not being used.
	switch (Create)
	{
	case TRUE: //on process creation
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "(ID:0x%X) PROCESS is being created\n", ProcessId);
		break;
	case FALSE: //on process exit
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "(ID:0x%X) PROCESS is being terminated\n", ProcessId);
		break;


	}
}
//Callback2 , used when the driver or libraries are being loaded.
void LoadImageCallBackCallBackRoutine(
IN PUNICODE_STRING FullImageName,
IN HANDLE ProcessId,
IN PIMAGE_INFO ImageInfo
)
{
	WCHAR *pwsName = NULL;
	if (FullImageName == NULL) {
		goto exit;
	}
	// allocate memory to store strings
	// it has to be nonpagedpool : kernel level reference 
	pwsName = (WCHAR *)ExAllocatePool(NonPagedPool,
		FullImageName->Length + sizeof(WCHAR));

	//copying string
	memcpy(pwsName, FullImageName->Buffer, FullImageName->Length);

	pwsName[FullImageName->Length / sizeof(WCHAR)] = 0; //NULL

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "PROCESS  0x%X is loading (%ws) ", ProcessId,pwsName);

	if (ImageInfo->SystemModeImage) 
		//if it's not 0 then
		//the thing being loaded is kernel driver
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "PROCESS  0x%X is loading Driver (%ws) ", ProcessId, pwsName);

	}
	else { // if it's 0 user dll
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "PROCESS  0x%X is loading dll (%ws) ", ProcessId, pwsName);
	}

exit:
	if (pwsName) {
		ExFreePool(pwsName); //counter part of Exallocatepool
	}
}

//unload Routine
void MyDriverUnload
(
	IN PDRIVER_OBJECT DriverObject
)
{
	UNREFERENCED_PARAMETER(DriverObject);

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "Unloading Driver\n");

	//if I unload driver without unregistering callbacks, bsod..
	PsSetCreateProcessNotifyRoutine(ProcessNotifyCallBackRoutine, TRUE);
	PsRemoveLoadImageNotifyRoutine(LoadImageCallBackCallBackRoutine);
}

NTSTATUS DriverEntry
(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath

)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "Driver Loading\n");
	DriverObject->DriverUnload = MyDriverUnload; //unload routine, if I don't define this, the driver will never unload itself.
	PsSetCreateProcessNotifyRoutine(ProcessNotifyCallBackRoutine, FALSE);
	PsSetLoadImageNotifyRoutine(LoadImageCallBackCallBackRoutine);
	return STATUS_SUCCESS;
}
