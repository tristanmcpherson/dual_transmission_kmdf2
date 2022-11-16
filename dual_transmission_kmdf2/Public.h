/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_dualtransmissionkmdf2,
    0x8d3e89d1,0x533f,0x45d3,0x85,0x37,0x50,0xdc,0x88,0x70,0x2f,0xe1);
// {8d3e89d1-533f-45d3-8537-50dc88702fe1}
