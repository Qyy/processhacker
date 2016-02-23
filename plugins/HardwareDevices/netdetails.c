/*
 * Process Hacker Plugins -
 *   Hardware Devices Plugin
 *
 * Copyright (C) 2015-2016 dmex
 * Copyright (C) 2016 wj32
 *
 * This file is part of Process Hacker.
 *
 * Process Hacker is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Process Hacker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Process Hacker.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "devices.h"

static VOID NTAPI ProcessesUpdatedHandler(
    _In_opt_ PVOID Parameter,
    _In_opt_ PVOID Context
    )
{
    PDV_NETADAPTER_DETAILS_CONTEXT context = Context;

    if (context->WindowHandle)
    {
        PostMessage(context->WindowHandle, UPDATE_MSG, 0, 0);
    }
}

static VOID AddListViewItemGroupId(
    _In_ HWND ListViewHandle,
    _In_ INT GroupId,
    _In_ INT Index,
    _In_ PWSTR Text
    )
{
    LVITEM item;
    item.mask = LVIF_TEXT | LVIF_GROUPID;
    item.iGroupId = GroupId;
    item.iItem = Index;
    item.pszText = Text;
    item.iSubItem = 0;

    ListView_InsertItem(ListViewHandle, &item);
}

static VOID AddListViewGroup(
    _In_ HWND ListViewHandle,
    _In_ INT Index,
    _In_ PWSTR Text
    )
{
    LVGROUP group = { LVGROUP_V5_SIZE };
    group.mask = LVGF_HEADER | LVGF_GROUPID;

    if (WindowsVersion >= WINDOWS_VISTA)
    {
        group.cbSize = sizeof(LVGROUP);
        group.mask = group.mask | LVGF_ALIGN | LVGF_STATE;
        group.uAlign = LVGA_HEADER_LEFT;
        group.state = LVGS_COLLAPSIBLE;
    }

    group.iGroupId = Index;
    group.pszHeader = Text;

    ListView_InsertGroup(ListViewHandle, INT_MAX, &group);
}

static VOID AddListViewItemGroups(
    _In_ HWND ListViewHandle
    )
{
    ListView_EnableGroupView(ListViewHandle, TRUE);
    AddListViewGroup(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, L"Adapter");
    AddListViewGroup(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, L"Unicast");
    AddListViewGroup(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_BROADCAST, L"Broadcast");
    AddListViewGroup(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_MULTICAST, L"Multicast");
    AddListViewGroup(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ERRORS, L"Errors");

    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_STATE, L"State");
    //AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_CONNECTIVITY, L"Connectivity");  
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_IPADDRESS, L"IP address");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_SUBNET, L"Subnet mask");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_GATEWAY, L"Default gateway");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_DOMAIN, L"Domain");

    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_LINKSPEED, L"Link speed");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_SENT, L"Sent");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_RECEIVED, L"Received");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_TOTAL, L"Total");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_SENDING, L"Sending");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_RECEIVING, L"Receiving");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ADAPTER, NETADAPTER_DETAILS_INDEX_UTILIZATION, L"Utilization");

    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, NETADAPTER_DETAILS_INDEX_UNICAST_SENTPKTS, L"Sent packets");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, NETADAPTER_DETAILS_INDEX_UNICAST_RECVPKTS, L"Received packets");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, NETADAPTER_DETAILS_INDEX_UNICAST_TOTALPKTS, L"Total packets");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, NETADAPTER_DETAILS_INDEX_UNICAST_SENT, L"Sent");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, NETADAPTER_DETAILS_INDEX_UNICAST_RECEIVED, L"Received");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, NETADAPTER_DETAILS_INDEX_UNICAST_TOTAL, L"Total");
    //AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, NETADAPTER_DETAILS_INDEX_UNICAST_SENDING, L"Sending");
    //AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, NETADAPTER_DETAILS_INDEX_UNICAST_RECEIVING, L"Receiving");
    //AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_UNICAST, NETADAPTER_DETAILS_INDEX_UNICAST_UTILIZATION, L"Utilization");

    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_BROADCAST, NETADAPTER_DETAILS_INDEX_BROADCAST_SENTPKTS, L"Sent packets");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_BROADCAST, NETADAPTER_DETAILS_INDEX_BROADCAST_RECVPKTS, L"Received packets");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_BROADCAST, NETADAPTER_DETAILS_INDEX_BROADCAST_TOTALPKTS, L"Total packets");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_BROADCAST, NETADAPTER_DETAILS_INDEX_BROADCAST_SENT, L"Sent");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_BROADCAST, NETADAPTER_DETAILS_INDEX_BROADCAST_RECEIVED, L"Received");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_BROADCAST, NETADAPTER_DETAILS_INDEX_BROADCAST_TOTAL, L"Total");

    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_MULTICAST, NETADAPTER_DETAILS_INDEX_MULTICAST_SENTPKTS, L"Sent packets");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_MULTICAST, NETADAPTER_DETAILS_INDEX_MULTICAST_RECVPKTS, L"Received packets");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_MULTICAST, NETADAPTER_DETAILS_INDEX_MULTICAST_TOTALPKTS, L"Total packets");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_MULTICAST, NETADAPTER_DETAILS_INDEX_MULTICAST_SENT, L"Sent");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_MULTICAST, NETADAPTER_DETAILS_INDEX_MULTICAST_RECEIVED, L"Received");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_MULTICAST, NETADAPTER_DETAILS_INDEX_MULTICAST_TOTAL, L"Total");

    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ERRORS, NETADAPTER_DETAILS_INDEX_ERRORS_SENTPKTS, L"Send errors");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ERRORS, NETADAPTER_DETAILS_INDEX_ERRORS_RECVPKTS, L"Receive errors");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ERRORS, NETADAPTER_DETAILS_INDEX_ERRORS_TOTALPKTS, L"Total errors");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ERRORS, NETADAPTER_DETAILS_INDEX_ERRORS_SENT, L"Send discards");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ERRORS, NETADAPTER_DETAILS_INDEX_ERRORS_RECEIVED, L"Receive discards");
    AddListViewItemGroupId(ListViewHandle, NETADAPTER_DETAILS_CATEGORY_ERRORS, NETADAPTER_DETAILS_INDEX_ERRORS_TOTAL, L"Total discards");
}


static VOID NetAdapterLookupConfig(
    _Inout_ PDV_NETADAPTER_DETAILS_CONTEXT Context
    )
{
    if (WindowsVersion >= WINDOWS_VISTA)
    {
        ULONG flags = GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_SKIP_FRIENDLY_NAME;
        ULONG bufferLength = 0;
        PIP_ADAPTER_ADDRESSES buffer = NULL;
        PPH_STRING domainString = NULL;
        PPH_STRING ipAddressString = NULL;
        PPH_STRING subnetAddressString = NULL;
        PPH_STRING gatewayAddressString = NULL;
        PH_STRING_BUILDER ipAddressBuffer;
        PH_STRING_BUILDER subnetAddressBuffer;
        PH_STRING_BUILDER gatewayAddressBuffer;

        PhInitializeStringBuilder(&ipAddressBuffer, 64);
        PhInitializeStringBuilder(&subnetAddressBuffer, 64);
        PhInitializeStringBuilder(&gatewayAddressBuffer, 64);

        if (PhGetIntegerSetting(SETTING_NAME_ENABLE_HIDDEN_ADAPTERS) && WindowsVersion >= WINDOWS_VISTA)
        {
            flags |= GAA_FLAG_INCLUDE_ALL_INTERFACES;
        }

        __try
        {
            if (GetAdaptersAddresses(AF_UNSPEC, flags, NULL, NULL, &bufferLength) != ERROR_BUFFER_OVERFLOW)
                __leave;

            buffer = PhAllocate(bufferLength);
            memset(buffer, 0, bufferLength);

            if (GetAdaptersAddresses(AF_UNSPEC, flags, NULL, buffer, &bufferLength) != ERROR_SUCCESS)
                __leave;

            for (PIP_ADAPTER_ADDRESSES addressesBuffer = buffer; addressesBuffer; addressesBuffer = addressesBuffer->Next)
            {
                if (addressesBuffer->Luid.Value != Context->AdapterId.InterfaceLuid.Value)
                    continue;

                for (PIP_ADAPTER_UNICAST_ADDRESS unicastAddress = addressesBuffer->FirstUnicastAddress; unicastAddress; unicastAddress = unicastAddress->Next)
                {
                    if (unicastAddress->Address.lpSockaddr->sa_family == AF_INET)
                    {
                        PSOCKADDR_IN sockAddrIn = { 0 };
                        IN_ADDR subnetMask = { 0 };
                        WCHAR ipv4AddressString[INET_ADDRSTRLEN] = L"";
                        WCHAR subnetAddressString[INET_ADDRSTRLEN] = L"";

                        sockAddrIn = (PSOCKADDR_IN)unicastAddress->Address.lpSockaddr;

                        if (WindowsVersion >= WINDOWS_VISTA && ConvertLengthToIpv4Mask)
                        {
                            ConvertLengthToIpv4Mask(unicastAddress->OnLinkPrefixLength, &subnetMask.s_addr);
                        }

                        if (RtlIpv4AddressToString(&sockAddrIn->sin_addr, ipv4AddressString))
                        {
                            PhAppendFormatStringBuilder(&ipAddressBuffer, L"%s, ", ipv4AddressString);
                        }

                        if (RtlIpv4AddressToString(&subnetMask, subnetAddressString))
                        {
                            PhAppendFormatStringBuilder(&subnetAddressBuffer, L"%s, ", subnetAddressString);
                        }
                    }
                    //else if (unicastAddress->Address.lpSockaddr->sa_family == AF_INET6)
                    //{
                    //    PSOCKADDR_IN6 sockAddrIn6 = { 0 };
                    //    WCHAR ipv6AddressString[INET6_ADDRSTRLEN] = L"";
                    //
                    //    sockAddrIn6 = (PSOCKADDR_IN6)unicastAddress->Address.lpSockaddr;
                    //
                    //    if (RtlIpv6AddressToString(&sockAddrIn6->sin6_addr, ipv6AddressString))
                    //    {
                    //        PhAppendFormatStringBuilder(&ipAddressBuffer, L"%s, ", ipv6AddressString);
                    //    }
                    //}
                }

                for (PIP_ADAPTER_GATEWAY_ADDRESS gatewayAddress = addressesBuffer->FirstGatewayAddress; gatewayAddress; gatewayAddress = gatewayAddress->Next)
                {
                    if (gatewayAddress->Address.lpSockaddr->sa_family == AF_INET)
                    {
                        PSOCKADDR_IN sockAddrIn = { 0 };
                        WCHAR ipv4AddressString[INET_ADDRSTRLEN] = L"";

                        sockAddrIn = (PSOCKADDR_IN)gatewayAddress->Address.lpSockaddr;

                        if (RtlIpv4AddressToString(&sockAddrIn->sin_addr, ipv4AddressString))
                        {
                            PhAppendFormatStringBuilder(&gatewayAddressBuffer, L"%s, ", ipv4AddressString);
                        }
                    }
                    //else if (gatewayAddress->Address.lpSockaddr->sa_family == AF_INET6)
                    //{
                    //    PSOCKADDR_IN6 sockAddrIn6 = (PSOCKADDR_IN6)gatewayAddress->Address.lpSockaddr;
                    //    WCHAR ipv6AddressString[INET6_ADDRSTRLEN] = L"";
                    //
                    //    if (RtlIpv6AddressToString(&sockAddrIn6->sin6_addr, ipv6AddressString))
                    //    {
                    //        PhAppendFormatStringBuilder(&gatewayAddressBuffer, L"%s, ", ipv6AddressString);
                    //    }
                    //}
                }

                //for (PIP_ADAPTER_DNS_SERVER_ADDRESS dnsAddress = addressesBuffer->FirstDnsServerAddress; dnsAddress; dnsAddress = dnsAddress->Next)
                //{
                //    if (dnsAddress->Address.lpSockaddr->sa_family == AF_INET)
                //    {
                //        PSOCKADDR_IN sockAddrIn = (PSOCKADDR_IN)dnsAddress->Address.lpSockaddr;
                //        WCHAR ipv4AddressString[INET_ADDRSTRLEN] = L"";
                //
                //        if (RtlIpv4AddressToString(&sockAddrIn->sin_addr, ipv4AddressString))
                //        {
                //            PhAppendFormatStringBuilder(&dnsAddressBuffer, L"%s, ", ipv4AddressString);
                //        }
                //    }
                //    else if (dnsAddress->Address.lpSockaddr->sa_family == AF_INET6)
                //    {
                //        PSOCKADDR_IN6 sockAddrIn6 = (PSOCKADDR_IN6)dnsAddress->Address.lpSockaddr;
                //        WCHAR ipv6AddressString[INET6_ADDRSTRLEN] = L"";
                //    
                //        if (RtlIpv6AddressToString(&sockAddrIn6->sin6_addr, ipv6AddressString))
                //        {
                //            PhAppendFormatStringBuilder(&dnsAddressBuffer, L"%s, ", ipv6AddressString);
                //        }
                //    }
                //}

                if (ipAddressBuffer.String->Length > 2)
                    PhRemoveEndStringBuilder(&ipAddressBuffer, 2);
                if (subnetAddressBuffer.String->Length > 2)
                    PhRemoveEndStringBuilder(&subnetAddressBuffer, 2);
                if (gatewayAddressBuffer.String->Length > 2)
                    PhRemoveEndStringBuilder(&gatewayAddressBuffer, 2);

                domainString = PhCreateString(addressesBuffer->DnsSuffix);
                ipAddressString = PhFinalStringBuilderString(&ipAddressBuffer);
                subnetAddressString = PhFinalStringBuilderString(&subnetAddressBuffer);
                gatewayAddressString = PhFinalStringBuilderString(&gatewayAddressBuffer);

                //PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_CONNECTIVITY, 1, internet ? L"Internet" : L"Local");      
                PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_IPADDRESS, 1, ipAddressString->Buffer);
                PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_SUBNET, 1, subnetAddressString->Buffer);
                PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_GATEWAY, 1, gatewayAddressString->Buffer);
                PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_DOMAIN, 1, domainString->Buffer);
            }
        }
        __finally
        {
            if (buffer)
                PhFree(buffer);

            PhClearReference(&domainString);
            PhDeleteStringBuilder(&ipAddressBuffer);
            PhDeleteStringBuilder(&subnetAddressBuffer);
            PhDeleteStringBuilder(&gatewayAddressBuffer);
        }
    }
    else
    {
        static PH_STRINGREF tcpIpv4KeyName = PH_STRINGREF_INIT(L"System\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\");
        HANDLE keyHandle;
        PPH_STRING keyNameIpV4;

        keyNameIpV4 = PhConcatStringRef2(&tcpIpv4KeyName, &Context->AdapterId.InterfaceGuid->sr);

        if (NT_SUCCESS(PhOpenKey(
            &keyHandle,
            KEY_READ,
            PH_KEY_LOCAL_MACHINE,
            &keyNameIpV4->sr,
            0
            )))
        {
            PPH_STRING domainString = NULL;
            PPH_STRING ipAddressString = NULL;
            PPH_STRING subnetAddressString = NULL;
            PPH_STRING gatewayAddressString = NULL;

            domainString = PhQueryRegistryString(keyHandle, L"DhcpDomain");
            ipAddressString = PhQueryRegistryString(keyHandle, L"DhcpIPAddress");
            subnetAddressString = PhQueryRegistryString(keyHandle, L"DhcpSubnetMask");
            gatewayAddressString = PhQueryRegistryString(keyHandle, L"DhcpDefaultGateway");

            if (PhIsNullOrEmptyString(domainString))
                PhMoveReference(&domainString, PhQueryRegistryString(keyHandle, L"Domain"));

            if (PhIsNullOrEmptyString(ipAddressString))
                PhMoveReference(&ipAddressString, PhQueryRegistryString(keyHandle, L"IPAddress"));

            if (PhIsNullOrEmptyString(subnetAddressString))
                PhMoveReference(&subnetAddressString, PhQueryRegistryString(keyHandle, L"SubnetMask"));

            if (PhIsNullOrEmptyString(gatewayAddressString))
                PhMoveReference(&gatewayAddressString, PhQueryRegistryString(keyHandle, L"DefaultGateway"));

            //PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_CONNECTIVITY, 1, internet ? L"Internet" : L"Local");      
            PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_IPADDRESS, 1, PhIsNullOrEmptyString(ipAddressString) ? L"" : ipAddressString->Buffer);
            PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_SUBNET, 1, PhIsNullOrEmptyString(subnetAddressString) ? L"" : subnetAddressString->Buffer);
            PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_GATEWAY, 1, PhIsNullOrEmptyString(gatewayAddressString) ? L"" : gatewayAddressString->Buffer);
            PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_DOMAIN, 1, PhIsNullOrEmptyString(domainString) ? L"" : domainString->Buffer);

            PhClearReference(&domainString);
            PhClearReference(&ipAddressString);
            PhClearReference(&subnetAddressString);
            PhClearReference(&gatewayAddressString);

            NtClose(keyHandle);
        }

        PhDereferenceObject(keyNameIpV4);
    }
}

static VOID NETIOAPI_API_ NetAdapterChangeCallback(
    _In_ PVOID CallerContext,
    _In_opt_ PMIB_IPINTERFACE_ROW Row,
    _In_ MIB_NOTIFICATION_TYPE NotificationType
    )
{
    PDV_NETADAPTER_DETAILS_CONTEXT context = CallerContext;

    if (NotificationType == MibInitialNotification)
    {
        NetAdapterLookupConfig(context);
    }
    else if (NotificationType == MibParameterNotification)
    {
        if (Row->InterfaceLuid.Value = context->AdapterId.InterfaceLuid.Value)
        {
            NetAdapterLookupConfig(context);
        }
    }
}

static VOID NetAdapterUpdateDetails(
    _Inout_ PDV_NETADAPTER_DETAILS_CONTEXT Context
    )
{
    ULONG64 interfaceLinkSpeed = 0;
    ULONG64 interfaceRcvSpeed = 0;
    ULONG64 interfaceXmitSpeed = 0;
    NDIS_STATISTICS_INFO interfaceStats = { 0 };
    NDIS_MEDIA_CONNECT_STATE mediaState = MediaConnectStateUnknown;
    HANDLE deviceHandle = NULL;

    if (PhGetIntegerSetting(SETTING_NAME_ENABLE_NDIS))
    {
        // Create the handle to the network device
        NetworkAdapterCreateHandle(
            &deviceHandle,
            Context->AdapterId.InterfaceGuid
            );

        if (deviceHandle)
        {
            if (!Context->CheckedDeviceSupport)
            {
                // Check the network adapter supports the OIDs we're going to be using.
                if (NetworkAdapterQuerySupported(deviceHandle))
                {
                    Context->DeviceSupported = TRUE;
                }

                Context->CheckedDeviceSupport = TRUE;
            }

            if (!Context->DeviceSupported)
            {
                // Device is faulty. Close the handle so we can fallback to GetIfEntry.
                NtClose(deviceHandle);
                deviceHandle = NULL;
            }
        }
    }

    if (deviceHandle)
    {
        NDIS_LINK_STATE interfaceState;

        NetworkAdapterQueryStatistics(deviceHandle, &interfaceStats);

        if (NT_SUCCESS(NetworkAdapterQueryLinkState(deviceHandle, &interfaceState)))
        {
            mediaState = interfaceState.MediaConnectState;
            interfaceLinkSpeed = interfaceState.XmitLinkSpeed;
        }
        else
        {
            NetworkAdapterQueryLinkSpeed(deviceHandle, &interfaceLinkSpeed);
        }

        NtClose(deviceHandle);
    }
    else
    {
        if (WindowsVersion >= WINDOWS_VISTA && GetIfEntry2)
        {
            MIB_IF_ROW2 interfaceRow;

            interfaceRow = QueryInterfaceRowVista(&Context->AdapterId);

            interfaceStats.ifInDiscards = interfaceRow.InDiscards;
            interfaceStats.ifInErrors = interfaceRow.InErrors;
            interfaceStats.ifHCInOctets = interfaceRow.InOctets;
            interfaceStats.ifHCInUcastPkts = interfaceRow.InUcastPkts;
            //interfaceStats.ifHCInMulticastPkts;
            //interfaceStats.ifHCInBroadcastPkts;
            interfaceStats.ifHCOutOctets = interfaceRow.OutOctets;
            interfaceStats.ifHCOutUcastPkts = interfaceRow.OutUcastPkts;
            //interfaceStats.ifHCOutMulticastPkts;
            //interfaceStats.ifHCOutBroadcastPkts;
            interfaceStats.ifOutErrors = interfaceRow.OutErrors;
            interfaceStats.ifOutDiscards = interfaceRow.OutDiscards;
            interfaceStats.ifHCInUcastOctets = interfaceRow.InUcastOctets;
            interfaceStats.ifHCInMulticastOctets = interfaceRow.InMulticastOctets;
            interfaceStats.ifHCInBroadcastOctets = interfaceRow.InBroadcastOctets;
            interfaceStats.ifHCOutUcastOctets = interfaceRow.OutUcastOctets;
            interfaceStats.ifHCOutMulticastOctets = interfaceRow.OutMulticastOctets;
            interfaceStats.ifHCOutBroadcastOctets = interfaceRow.OutBroadcastOctets;
            //interfaceRow.InNUcastPkts;
            //interfaceRow.InUnknownProtos;
            //interfaceRow.OutNUcastPkts;
            //interfaceRow.OutQLen;

            mediaState = interfaceRow.MediaConnectState;
            interfaceLinkSpeed = interfaceRow.TransmitLinkSpeed;
        }
        else
        {
            MIB_IFROW interfaceRow;

            interfaceRow = QueryInterfaceRowXP(&Context->AdapterId);

            interfaceStats.ifInDiscards = interfaceRow.dwInDiscards;
            interfaceStats.ifInErrors = interfaceRow.dwInErrors;
            interfaceStats.ifHCInOctets = interfaceRow.dwInOctets;
            interfaceStats.ifHCInUcastPkts = interfaceRow.dwInUcastPkts;
            //interfaceStats.ifHCInMulticastPkts;
            //interfaceStats.ifHCInBroadcastPkts;
            interfaceStats.ifHCOutOctets = interfaceRow.dwOutOctets;
            interfaceStats.ifHCOutUcastPkts = interfaceRow.dwOutUcastPkts;
            //interfaceStats.ifHCOutMulticastPkts;
            //interfaceStats.ifHCOutBroadcastPkts;
            interfaceStats.ifOutErrors = interfaceRow.dwOutErrors;
            interfaceStats.ifOutDiscards = interfaceRow.dwOutDiscards;
            //interfaceStats.ifHCInUcastOctets;
            //interfaceStats.ifHCInMulticastOctets;
            //interfaceStats.ifHCInBroadcastOctets;
            //interfaceStats.ifHCOutUcastOctets;
            //interfaceStats.ifHCOutMulticastOctets;
            //interfaceStats.ifHCOutBroadcastOctets;
            //interfaceRow.InNUcastPkts;
            //interfaceRow.InUnknownProtos;
            //interfaceRow.OutNUcastPkts;
            //interfaceRow.OutQLen;

            mediaState = interfaceRow.dwOperStatus == IF_OPER_STATUS_OPERATIONAL ? MediaConnectStateConnected : MediaConnectStateDisconnected;
            interfaceLinkSpeed = interfaceRow.dwSpeed;
        }
    }

    interfaceRcvSpeed = interfaceStats.ifHCInOctets - Context->LastDetailsInboundValue;
    interfaceXmitSpeed = interfaceStats.ifHCOutOctets - Context->LastDetailsIOutboundValue;
    //interfaceRcvUnicastSpeed = interfaceStats.ifHCInUcastOctets - Context->LastDetailsInboundUnicastValue;
    //interfaceXmitUnicastSpeed = interfaceStats.ifHCOutUcastOctets - Context->LastDetailsIOutboundUnicastValue;

    if (!Context->HaveFirstSample)
    {
        interfaceRcvSpeed = 0;
        interfaceXmitSpeed = 0;
        Context->HaveFirstSample = TRUE;
    }

    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_STATE, 1, mediaState == MediaConnectStateConnected ? L"Connected" : L"Disconnected");

    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_LINKSPEED, 1, PhaFormatString(L"%s/s", PhaFormatSize(interfaceLinkSpeed / BITS_IN_ONE_BYTE, -1)->Buffer)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_SENT, 1, PhaFormatSize(interfaceStats.ifHCOutOctets, -1)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_RECEIVED, 1, PhaFormatSize(interfaceStats.ifHCInOctets, -1)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_TOTAL, 1, PhaFormatSize(interfaceStats.ifHCInOctets + interfaceStats.ifHCOutOctets, -1)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_SENDING, 1, interfaceXmitSpeed != 0 ? PhaFormatString(L"%s/s", PhaFormatSize(interfaceXmitSpeed, -1)->Buffer)->Buffer : L"");
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_RECEIVING, 1, interfaceRcvSpeed != 0 ? PhaFormatString(L"%s/s", PhaFormatSize(interfaceRcvSpeed, -1)->Buffer)->Buffer : L"");
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UTILIZATION, 1, PhaFormatString(L"%.2f%%", (FLOAT)(interfaceRcvSpeed + interfaceXmitSpeed) / (interfaceLinkSpeed / BITS_IN_ONE_BYTE) * 100)->Buffer);

    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UNICAST_SENTPKTS, 1, PhaFormatUInt64(interfaceStats.ifHCOutUcastPkts, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UNICAST_RECVPKTS, 1, PhaFormatUInt64(interfaceStats.ifHCInUcastPkts, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UNICAST_TOTALPKTS, 1, PhaFormatUInt64(interfaceStats.ifHCInUcastPkts + interfaceStats.ifHCOutUcastPkts, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UNICAST_SENT, 1, PhaFormatSize(interfaceStats.ifHCOutUcastOctets, -1)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UNICAST_RECEIVED, 1, PhaFormatSize(interfaceStats.ifHCInUcastOctets, -1)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UNICAST_TOTAL, 1, PhaFormatSize(interfaceStats.ifHCInUcastOctets + interfaceStats.ifHCOutUcastOctets, -1)->Buffer);
    //PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UNICAST_SENDING, 1, interfaceXmitUnicastSpeed != 0 ? PhaFormatString(L"%s/s", PhaFormatSize(interfaceXmitUnicastSpeed, -1)->Buffer)->Buffer : L"");
    //PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UNICAST_RECEIVING, 1, interfaceRcvUnicastSpeed != 0 ? PhaFormatString(L"%s/s", PhaFormatSize(interfaceRcvUnicastSpeed, -1)->Buffer)->Buffer : L"");
    //PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_UNICAST_UTILIZATION, 1, PhaFormatString(L"%.2f%%", utilization2)->Buffer);

    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_BROADCAST_SENTPKTS, 1, PhaFormatUInt64(interfaceStats.ifHCOutBroadcastPkts, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_BROADCAST_RECVPKTS, 1, PhaFormatUInt64(interfaceStats.ifHCInBroadcastPkts, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_BROADCAST_TOTALPKTS, 1, PhaFormatUInt64(interfaceStats.ifHCInBroadcastPkts + interfaceStats.ifHCOutBroadcastPkts, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_BROADCAST_SENT, 1, PhaFormatSize(interfaceStats.ifHCOutBroadcastOctets, -1)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_BROADCAST_RECEIVED, 1, PhaFormatSize(interfaceStats.ifHCInBroadcastOctets, -1)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_BROADCAST_TOTAL, 1, PhaFormatSize(interfaceStats.ifHCInBroadcastOctets + interfaceStats.ifHCOutBroadcastOctets, -1)->Buffer);

    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_MULTICAST_SENTPKTS, 1, PhaFormatUInt64(interfaceStats.ifHCOutMulticastPkts, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_MULTICAST_RECVPKTS, 1, PhaFormatUInt64(interfaceStats.ifHCInMulticastPkts, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_MULTICAST_TOTALPKTS, 1, PhaFormatUInt64(interfaceStats.ifHCInMulticastPkts + interfaceStats.ifHCOutMulticastPkts, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_MULTICAST_SENT, 1, PhaFormatSize(interfaceStats.ifHCOutMulticastOctets, -1)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_MULTICAST_RECEIVED, 1, PhaFormatSize(interfaceStats.ifHCInMulticastOctets, -1)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_MULTICAST_TOTAL, 1, PhaFormatSize(interfaceStats.ifHCInMulticastOctets + interfaceStats.ifHCOutMulticastOctets, -1)->Buffer);

    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_ERRORS_SENTPKTS, 1, PhaFormatUInt64(interfaceStats.ifOutErrors, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_ERRORS_RECVPKTS, 1, PhaFormatUInt64(interfaceStats.ifInErrors, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_ERRORS_TOTALPKTS, 1, PhaFormatUInt64(interfaceStats.ifInErrors + interfaceStats.ifOutErrors, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_ERRORS_SENT, 1, PhaFormatUInt64(interfaceStats.ifOutDiscards, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_ERRORS_RECEIVED, 1, PhaFormatUInt64(interfaceStats.ifInDiscards, TRUE)->Buffer);
    PhSetListViewSubItem(Context->ListViewHandle, NETADAPTER_DETAILS_INDEX_ERRORS_TOTAL, 1, PhaFormatUInt64(interfaceStats.ifInDiscards + interfaceStats.ifOutDiscards, TRUE)->Buffer);

    Context->LastDetailsInboundValue = interfaceStats.ifHCInOctets;
    Context->LastDetailsIOutboundValue = interfaceStats.ifHCOutOctets;
    //Context->LastDetailsInboundUnicastValue = interfaceStats.ifHCInUcastOctets;
    //Context->LastDetailsIOutboundUnicastValue = interfaceStats.ifHCOutUcastOctets;
}


static INT_PTR CALLBACK AdapterDetailsDlgProc(
    _In_ HWND hwndDlg,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
    )
{
    PDV_NETADAPTER_DETAILS_CONTEXT context;

    if (uMsg == WM_INITDIALOG)
    {
        context = (PDV_NETADAPTER_DETAILS_CONTEXT)lParam;
        SetProp(hwndDlg, L"Context", (HANDLE)context);
    }
    else
    {
        context = (PDV_NETADAPTER_DETAILS_CONTEXT)GetProp(hwndDlg, L"Context");

        if (uMsg == WM_DESTROY)
            RemoveProp(hwndDlg, L"Context");
    }

    if (!context)
        return FALSE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            context->WindowHandle = hwndDlg;
            context->ListViewHandle = GetDlgItem(hwndDlg, IDC_DETAILS_LIST);

            if (context->AdapterName)
                SetWindowText(hwndDlg, context->AdapterName->Buffer);

            PhCenterWindow(hwndDlg, context->ParentHandle);

            PhSetListViewStyle(context->ListViewHandle, FALSE, TRUE);
            PhSetControlTheme(context->ListViewHandle, L"explorer");
            PhAddListViewColumn(context->ListViewHandle, 0, 0, 0, LVCFMT_LEFT, 325, L"Property");
            PhAddListViewColumn(context->ListViewHandle, 1, 1, 1, LVCFMT_LEFT, 90, L"Value");

            PhInitializeLayoutManager(&context->LayoutManager, hwndDlg);
            PhAddLayoutItem(&context->LayoutManager, context->ListViewHandle, NULL, PH_ANCHOR_ALL);
            PhAddLayoutItem(&context->LayoutManager, GetDlgItem(hwndDlg, IDOK), NULL, PH_ANCHOR_BOTTOM | PH_ANCHOR_RIGHT);

            AddListViewItemGroups(context->ListViewHandle);

            PhRegisterCallback(
                &PhProcessesUpdatedEvent,
                ProcessesUpdatedHandler,
                context,
                &context->ProcessesUpdatedRegistration
                );

            NetAdapterLookupConfig(context);
            NetAdapterUpdateDetails(context);

            if (WindowsVersion >= WINDOWS_VISTA && NotifyIpInterfaceChange)
            {
                NotifyIpInterfaceChange(
                    AF_UNSPEC,
                    NetAdapterChangeCallback,
                    context,
                    FALSE,
                    &context->NotifyHandle
                    );
            }
        }
        break;
    case WM_DESTROY:
        {
            PhUnregisterCallback(
                &PhProcessesUpdatedEvent,
                &context->ProcessesUpdatedRegistration
                );

            if (WindowsVersion >= WINDOWS_VISTA && CancelMibChangeNotify2 && context->NotifyHandle)
            {
                CancelMibChangeNotify2(context->NotifyHandle);
            }

            PhDeleteLayoutManager(&context->LayoutManager);
            PostQuitMessage(0);
        }
        break;
    case WM_COMMAND:
        {
            switch (GET_WM_COMMAND_ID(wParam, lParam))
            {
            case IDCANCEL:
            case IDOK:
                DestroyWindow(hwndDlg);
                break;
            }
        }
        break;
    case WM_SIZE:
        {
            PhLayoutManagerLayout(&context->LayoutManager);
        }
        break;
    case WM_SHOWDIALOG:
        {
            if (IsMinimized(hwndDlg))
                ShowWindow(hwndDlg, SW_RESTORE);
            else
                ShowWindow(hwndDlg, SW_SHOW);

            SetForegroundWindow(hwndDlg);
        }
        break;
    case UPDATE_MSG:
        {
            NetAdapterUpdateDetails(context);
        }
        break;
    }

    return FALSE;
}

static VOID FreeDetailsContext(
    _In_ PDV_NETADAPTER_DETAILS_CONTEXT Context
    )
{
    DeleteNetAdapterId(&Context->AdapterId);
    PhClearReference(&Context->AdapterName);
    PhFree(Context);
}

static NTSTATUS ShowDetailsDialogThread(
    _In_ PVOID Parameter
    )
{
    PDV_NETADAPTER_DETAILS_CONTEXT context = (PDV_NETADAPTER_DETAILS_CONTEXT)Parameter;
    BOOL result;
    MSG message;
    HWND dialogHandle;
    PH_AUTO_POOL autoPool;

    PhInitializeAutoPool(&autoPool);

    dialogHandle = CreateDialogParam(
        PluginInstance->DllBase,
        MAKEINTRESOURCE(IDD_NETADAPTER_DETAILS),
        NULL,
        AdapterDetailsDlgProc,
        (LPARAM)context
        );

    PostMessage(dialogHandle, WM_SHOWDIALOG, 0, 0);

    while (result = GetMessage(&message, NULL, 0, 0))
    {
        if (result == -1)
            break;

        if (!IsDialogMessage(dialogHandle, &message))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        PhDrainAutoPool(&autoPool);
    }

    PhDeleteAutoPool(&autoPool);

    FreeDetailsContext(context);

    return STATUS_SUCCESS;
}

VOID ShowDetailsDialog(
    _In_ PDV_NETADAPTER_SYSINFO_CONTEXT Context
    )
{
    HANDLE dialogThread = NULL;
    PDV_NETADAPTER_DETAILS_CONTEXT context;

    context = PhAllocate(sizeof(DV_NETADAPTER_DETAILS_CONTEXT));
    memset(context, 0, sizeof(DV_NETADAPTER_DETAILS_CONTEXT));

    context->ParentHandle = Context->WindowHandle;
    PhSetReference(&context->AdapterName, Context->AdapterEntry->AdapterName);
    CopyNetAdapterId(&context->AdapterId, &Context->AdapterEntry->Id);

    if (dialogThread = PhCreateThread(0, ShowDetailsDialogThread, context))
        NtClose(dialogThread);
    else
        FreeDetailsContext(context);
}