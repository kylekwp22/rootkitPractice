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