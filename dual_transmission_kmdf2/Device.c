/*++

Module Name:

    device.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.
    
Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "device.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, dualtransmissionkmdf2CreateDevice)
#endif

//VOID
//Read(
//  PDEVICE_CONTEXT Context
//);

UCHAR ReportDescriptor[26] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x20,                    //     USAGE_MAXIMUM (Button 32)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x20,                    //     REPORT_COUNT (32)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION
    0xc0                           // END_COLLECTION
};

NTSTATUS
dualtransmissionkmdf2CreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
                    structure will be freed by the framework when the WdfDeviceCreate
                    succeeds. So don't access the structure after that point.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;
    NTSTATUS status;
    VHF_CONFIG vhfConfig;

    PAGED_CODE();

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    if (NT_SUCCESS(status)) {
        //
        // Get a pointer to the device context structure that we just associated
        // with the device object. We define this structure in the device.h
        // header file. DeviceGetContext is an inline function generated by
        // using the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro in device.h.
        // This function will do the type checking and return the device context.
        // If you pass a wrong object handle it will return NULL and assert if
        // run under framework verifier mode.
        //
        deviceContext = DeviceGetContext(device);


        VHF_CONFIG_INIT(&vhfConfig,
          WdfDeviceWdmGetDeviceObject(device),
          sizeof(ReportDescriptor),
          ReportDescriptor);

        //vhfConfig.EvtVhfReadyForNextReadReport = Read;

        status = VhfCreate(&vhfConfig, &(deviceContext->VhfHandle));

        if (NT_SUCCESS(status))
          status = VhfStart(deviceContext->VhfHandle);

        ////
        //// Initialize the context.
        ////
        //deviceContext->PrivateDeviceData = 0;

        ////
        //// Create a device interface so that applications can find and talk
        //// to us.
        ////
        //status = WdfDeviceCreateDeviceInterface(
        //    device,
        //    &GUID_DEVINTERFACE_dualtransmissionkmdf2,
        //    NULL // ReferenceString
        //    );

        if (NT_SUCCESS(status)) {
            //
            // Initialize the I/O Package and any Queues
            //
            status = dualtransmissionkmdf2QueueInitialize(device);
        }
    }

    return status;
}

//VOID 
//Read(
//  PDEVICE_CONTEXT Context
//  ) 
//{
//  TraceEvents(TRACE_LEVEL_WARNING, TRACE_DEVICE, std::to_string());
//  // read input from other devices;
////    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "VhfReadReportSubmit failed %!STATUS!", status);
//  //MY_SubmitReadReport(Context, buttonType, readButtonState)
//}

//VOID
//MY_SubmitReadReport(
//  PMY_CONTEXT  Context,
//  BUTTON_TYPE  ButtonType,
//  BUTTON_STATE ButtonState
//)
//{
//  NTSTATUS status;
//  PDEVICE_CONTEXT deviceContext = (PDEVICE_CONTEXT)(Context);
//
//  if (ButtonState == ButtonStateUp) {
//    deviceContext->VhfHidReport.reportBuffer[0] &= ~(0x01 << ButtonType);
//  }
//  else {
//    deviceContext->VhfHidReport.reportBuffer[0] |= (0x01 << ButtonType);
//  }
//
//  &deviceContext->VhfHidReport.reportBuffer
//  status = VhfReadReportSubmit(deviceContext->VhfHandle, &deviceContext->VhfHidReport);
//
//  if (!NT_SUCCESS(status)) {
//    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "VhfReadReportSubmit failed %!STATUS!", status);
//  }
//}