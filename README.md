# README

`uspd` is a [TR-369/USP](https://usp.technology/) backend ubus daemon process to understand [USP syntax](https://usp.technology/specification/architecture/) defined in R-ARC.7 - R-ARC.12 and provide details.

## Project Components

Project consists of following components:

- Application itself written in C programming language
- Documentation in a Markdown format

## Build Instructions

`uspd` is written using C programming language and depends on a number of components found in OpenWrt for building and running.

## UCI Config

uspd requires a configuration file to provide more granular objects over ubus. Granularity is an optional feature of uspd, it can be skipped or set to level 0. The configuration file is an uci file `/etc/config/uspd`. Sample configuration file is provided below.

````bash
config uspd 'usp'
        option granularitylevel  '0'
````

For more info on the UCI uspd configuration see [link](./docs/api/uci.uspd.md)

## Ubus API

`uspd` needs to be started on startup after `ubusd`, as it exposes the USP functionality over ubus. By default(when granularity is not set in uci), uspd registers below two namespaces with ubus.

````bash
root@iopsys:~# ubus list |grep usp
usp
usp.raw
````

Each namespace provide the similar result but in different formats. `usp` namespace is to provide the output as required by `USP` or `End User` or in pretty format, whereas `usp.raw` namespace is to provide output in raw format, which can be used by other USP frontend applications(like: obuspa). Each namespace has below functionalities.

````bash
root@iopsys:~# ubus -v list usp
'usp' @ac452d85
	"get":{"path":"String","proto":"String"}
	"get_safe":{"paths":"Array","proto":"String"}
	"set":{"path":"String","value":"String","values":"Table","proto":"String"}
	"operate":{"path":"String","action":"String","input":"Table","proto":"String"}
	"add_object":{"path":"String","proto":"String","key":"String"}
	"del_object":{"path":"String","proto":"String","key":"String"}
	"object_names":{"path":"String","proto":"String"}
	"instances":{"path":"String","proto":"String"}

root@iopsys:~#
root@iopsys:~# ubus -v list usp.raw
'usp.raw' @20cb78d8
	"get":{"path":"String","proto":"String"}
	"get_safe":{"paths":"Array","proto":"String"}
	"set":{"path":"String","value":"String","values":"Table","proto":"String"}
	"operate":{"path":"String","action":"String","input":"Table","proto":"String"}
	"add_object":{"path":"String","proto":"String","key":"String"}
	"del_object":{"path":"String","proto":"String","key":"String"}
	"object_names":{"path":"String","proto":"String"}
	"instances":{"path":"String","proto":"String"}
root@iopsys:~#
````

> Note: `proto` in each method specify the datamodel prototype('cwmp', 'usp') to use, if not provided default datamodel will be used.

The objects registered with the above namespaces can be called with appropriate parameters to perform a USP `Get/Set/Operate/Add Object/Delete Object` operation as below.

## Overview

uspd provides below commands in pretty(usp) or raw(usp.raw) formats:

 1. Get
 2. Get safe
 3. Set
 4. Operate
 5. Add object
 6. Delete object
 7. Object names
 8. Instances

It also provide a granularity layer which can be configured using uci parameter and provide additional ubus objects.

### Granularity
Granularity feature is basically exposes the same USP functionality by registering additinal ubus namespaces to reduce the `path` length in ubus parameter. It is the number of levels upto which we want to shorten the length.

Ex:

 - When Granularity is set to 1, exposed ubus namespaces are

````bash
root@iopsys:~# ubus list|grep usp
usp
usp.Device
usp.raw
````

 - When Granularity is set to 2, exposed ubus namespaces are

````bash
root@iopsys:~# ubus list|grep usp
usp
usp.Device
usp.Device.ATM
usp.Device.Bridging
usp.Device.BulkData
usp.Device.DHCPv4
usp.Device.DHCPv6
usp.Device.DNS
usp.Device.DSL
usp.Device.DeviceInfo
usp.Device.Ethernet
usp.Device.Firewall
usp.Device.Hosts
usp.Device.IP
usp.Device.InterfaceStack
usp.Device.ManagementServer
usp.Device.NAT
usp.Device.PPP
usp.Device.PTM
usp.Device.Routing
usp.Device.Services
usp.Device.SoftwareModules
usp.Device.Time
usp.Device.UPnP
usp.Device.USB
usp.Device.UserInterface
usp.Device.Users
usp.Device.WiFi
usp.Device.XMPP
usp.Device.X_IOPSYS_EU_Buttons
usp.Device.X_IOPSYS_EU_Dropbear
usp.Device.X_IOPSYS_EU_ICE
usp.Device.X_IOPSYS_EU_IGMP
usp.Device.X_IOPSYS_EU_IpAccCfg
usp.Device.X_IOPSYS_EU_LoginCfg
usp.Device.X_IOPSYS_EU_Owsd
usp.Device.X_IOPSYS_EU_PowerManagement
usp.Device.X_IOPSYS_EU_SyslogCfg
usp.Device.X_IOPSYS_EU_WiFiLife
usp.raw
````

These granular ubus objects provides the same functionality as of `usp` ubus namespace

````bash
root@iopsys:~# ubus -v list usp.Device.WiFi
'usp.Device.WiFi' @69650977
	"get":{"path":"String","proto":"String"}
	"get_safe":{"paths":"Array","proto":"String"}
	"set":{"path":"String","value":"String","values":"Table","proto":"String"}
	"operate":{"path":"String","action":"String","input":"Table","proto":"String"}
	"add_object":{"path":"String","proto":"String","key":"String"}
	"del_object":{"path":"String","proto":"String","key":"String"}
	"object_names":{"path":"String","proto":"String"}
	"instances":{"path":"String","proto":"String"}
````

Registered method can be called with appropriate parameters, like:

````bash
root@iopsys:~# ubus call usp.Device get '{"path":"Users."}'
{
        "Users": {
                "User": [
                        {
                                "Alias": "cpe-1",
                                "Enable": true,
                                "Language": "",
                                "Password": "",
                                "RemoteAccessCapable": true,
                                "Username": "user"
                        },
                        {
                                "Alias": "cpe-2",
                                "Enable": true,
                                "Language": "",
                                "Password": "",
                                "RemoteAccessCapable": true,
                                "Username": "support"
                        },
                        {
                                "Alias": "cpe-3",
                                "Enable": true,
                                "Language": "",
                                "Password": "",
                                "RemoteAccessCapable": true,
                                "Username": "admin"
                        }
                ],
                "UserNumberOfEntries": 3
        }
}
````

### Get
API to query the value of a specific object.

#### To get the output in pretty format use `usp` ubus namespace:

````bash
root@iopsys:~# ubus call usp get '{"path":"Device.IP.Diagnostics.", "proto":"usp"}'
{
        "Diagnostics": {
                "IPv4DownloadDiagnosticsSupported": true,
                "IPv4PingSupported": true,
                "IPv4ServerSelectionDiagnosticsSupported": true,
                "IPv4TraceRouteSupported": true,
                "IPv4UDPEchoDiagnosticsSupported": true,
                "IPv4UploadDiagnosticsSupported": true,
                "IPv6DownloadDiagnosticsSupported": true,
                "IPv6PingSupported": true,
                "IPv6ServerSelectionDiagnosticsSupported": true,
                "IPv6TraceRouteSupported": true,
                "IPv6UDPEchoDiagnosticsSupported": true,
                "IPv6UploadDiagnosticsSupported": true,
                "UDPEchoConfig": {
                        "BytesReceived": 0,
                        "BytesResponded": 0,
                        "EchoPlusEnabled": false,
                        "EchoPlusSupported": false,
                        "Enable": false,
                        "Interface": "",
                        "PacketsReceived": 0,
                        "PacketsResponded": 0,
                        "SourceIPAddress": "",
                        "TimeFirstPacketReceived": "0",
                        "TimeLastPacketReceived": "0",
                        "UDPPort": 0
                }
        }
}
root@iopsys:~#
root@iopsys:~# ubus call usp get '{"path":"Device.IP.Diagnostics.", "proto":"cwmp"}'
{
        "Diagnostics": {
                "DownloadDiagnostics": {
                        "BOMTime": "0",
                        "DSCP": 0,
                        "DiagnosticsState": "None",
                        "DownloadDiagnosticMaxConnections": 1,
                        "DownloadTransports": "HTTP,FTP",
                        "DownloadURL": "",
                        "EOMTime": "0",
                        "EnablePerConnectionResults": false,
                        "EthernetPriority": 0,
                        "Interface": "",
                        "NumberOfConnections": 1,
                        "PerConnectionResultNumberOfEntries": 0,
                        "PeriodOfFullLoading": 0,
                        "ProtocolVersion": "Any",
                        "ROMTime": "0",
                        "TCPOpenRequestTime": "0",
                        "TCPOpenResponseTime": "0",
                        "TestBytesReceived": 0,
                        "TestBytesReceivedUnderFullLoading": 0,
                        "TotalBytesReceived": 0,
                        "TotalBytesReceivedUnderFullLoading": 0,
                        "TotalBytesSent": 0,
                        "TotalBytesSentUnderFullLoading": 0
                },
                "IPPing": {
                        "AverageResponseTime": 0,
                        "AverageResponseTimeDetailed": 0,
                        "DSCP": 0,
                        "DataBlockSize": 64,
                        "DiagnosticsState": "None",
                        "FailureCount": 0,
                        "Host": "",
                        "Interface": "",
                        "MaximumResponseTime": 0,
                        "MaximumResponseTimeDetailed": 0,
                        "MinimumResponseTime": 0,
                        "MinimumResponseTimeDetailed": 0,
                        "NumberOfRepetitions": 3,
                        "ProtocolVersion": "Any",
                        "SuccessCount": 0,
                        "Timeout": 1000
                },
                "IPv4DownloadDiagnosticsSupported": true,
                "IPv4PingSupported": true,
                "IPv4ServerSelectionDiagnosticsSupported": true,
                "IPv4TraceRouteSupported": true,
                "IPv4UDPEchoDiagnosticsSupported": true,
                "IPv4UploadDiagnosticsSupported": true,
                "IPv6DownloadDiagnosticsSupported": true,
                "IPv6PingSupported": true,
                "IPv6ServerSelectionDiagnosticsSupported": true,
                "IPv6TraceRouteSupported": true,
                "IPv6UDPEchoDiagnosticsSupported": true,
                "IPv6UploadDiagnosticsSupported": true,
                "ServerSelectionDiagnostics": {
                        "AverageResponseTime": 0,
                        "DiagnosticsState": "None",
                        "FastestHost": "",
                        "HostList": "",
                        "Interface": "",
                        "MaximumResponseTime": 0,
                        "MinimumResponseTime": 0,
                        "NumberOfRepetitions": 3,
                        "Port": 0,
                        "Protocol": "ICMP",
                        "ProtocolVersion": "Any",
                        "Timeout": 1000
                },
                "TraceRoute": {
                        "DSCP": 0,
                        "DataBlockSize": 38,
                        "DiagnosticsState": "None",
                        "Host": "",
                        "Interface": "",
                        "MaxHopCount": 30,
                        "NumberOfTries": 3,
                        "ProtocolVersion": "Any",
                        "ResponseTime": 0,
                        "RouteHopsNumberOfEntries": 0,
                        "Timeout": 5000
                },
                "UDPEchoConfig": {
                        "BytesReceived": 0,
                        "BytesResponded": 0,
                        "EchoPlusEnabled": false,
                        "EchoPlusSupported": false,
                        "Enable": false,
                        "Interface": "",
                        "PacketsReceived": 0,
                        "PacketsResponded": 0,
                        "SourceIPAddress": "",
                        "TimeFirstPacketReceived": "0",
                        "TimeLastPacketReceived": "0",
                        "UDPPort": 0
                },
                "UDPEchoDiagnostics": {
                        "AverageResponseTime": 0,
                        "DSCP": 0,
                        "DataBlockSize": 24,
                        "DiagnosticsState": "None",
                        "FailureCount": 0,
                        "Host": "",
                        "InterTransmissionTime": 1000,
                        "Interface": "",
                        "MaximumResponseTime": 0,
                        "MinimumResponseTime": 0,
                        "NumberOfRepetitions": 1,
                        "Port": 0,
                        "ProtocolVersion": "Any",
                        "SuccessCount": 0,
                        "Timeout": 5000
                },
                "UploadDiagnostics": {
                        "BOMTime": "0",
                        "DSCP": 0,
                        "DiagnosticsState": "None",
                        "EOMTime": "0",
                        "EnablePerConnectionResults": false,
                        "EthernetPriority": 0,
                        "Interface": "",
                        "NumberOfConnections": 1,
                        "PerConnectionResultNumberOfEntries": 0,
                        "PeriodOfFullLoading": 0,
                        "ProtocolVersion": "Any",
                        "ROMTime": "0",
                        "TCPOpenRequestTime": "0",
                        "TCPOpenResponseTime": "0",
                        "TestBytesSent": 0,
                        "TestBytesSentUnderFullLoading": 0,
                        "TestFileLength": 0,
                        "TotalBytesReceived": 0,
                        "TotalBytesReceivedUnderFullLoading": 0,
                        "TotalBytesSent": 0,
                        "TotalBytesSentUnderFullLoading": 0,
                        "UploadTransports": "HTTP,FTP",
                        "UploadURL": ""
                }
        }
}
root@iopsys:~#
root@iopsys:~# ubus call usp get '{"path":"Device.Users."}'
{
        "Users": {
                "User": [
                        {
                                "Alias": "cpe-1",
                                "Enable": true,
                                "Language": "",
                                "Password": "",
                                "RemoteAccessCapable": true,
                                "Username": "user"
                        },
                        {
                                "Alias": "cpe-2",
                                "Enable": true,
                                "Language": "",
                                "Password": "",
                                "RemoteAccessCapable": true,
                                "Username": "support"
                        },
                        {
                                "Alias": "cpe-3",
                                "Enable": true,
                                "Language": "",
                                "Password": "",
                                "RemoteAccessCapable": true,
                                "Username": "admin"
                        }
                ],
                "UserNumberOfEntries": 3
        }
}
````
#### To get the output in raw format use `usp.raw` ubus namespace:
````bash
root@iopsys:~# ubus call usp.raw get '{"path":"Device.Users."}'
{
        "parameters": [
                {
                        "parameter": "Device.Users.User.1.Alias",
                        "value": "cpe-1",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.1.Enable",
                        "value": "1",
                        "type": "xsd:boolean"
                },
                {
                        "parameter": "Device.Users.User.1.Language",
                        "value": "",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.1.Password",
                        "value": "",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.1.RemoteAccessCapable",
                        "value": "1",
                        "type": "xsd:boolean"
                },
                {
                        "parameter": "Device.Users.User.1.Username",
                        "value": "user",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.2.Alias",
                        "value": "cpe-2",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.2.Enable",
                        "value": "1",
                        "type": "xsd:boolean"
                },
                {
                        "parameter": "Device.Users.User.2.Language",
                        "value": "",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.2.Password",
                        "value": "",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.2.RemoteAccessCapable",
                        "value": "1",
                        "type": "xsd:boolean"
                },
                {
                        "parameter": "Device.Users.User.2.Username",
                        "value": "support",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.3.Alias",
                        "value": "cpe-3",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.3.Enable",
                        "value": "1",
                        "type": "xsd:boolean"
                },
                {
                        "parameter": "Device.Users.User.3.Language",
                        "value": "",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.3.Password",
                        "value": "",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.User.3.RemoteAccessCapable",
                        "value": "1",
                        "type": "xsd:boolean"
                },
                {
                        "parameter": "Device.Users.User.3.Username",
                        "value": "admin",
                        "type": "xsd:string"
                },
                {
                        "parameter": "Device.Users.UserNumberOfEntries",
                        "value": "3",
                        "type": "xsd:unsignedInt"
                }
        ]
}
````
- For more info on the `usp` ubus API see [link](./docs/api/usp.md#get)
- For more info on the `usp.raw` ubus API see [link](./docs/api/usp.raw.md#get)

#### Set
API to set value to specific object, object name must be provided in `path` parameter and vlaue in `value` paratermer.

````bash
root@iopsys:~# ubus call usp set '{"path":"Device.WiFi.SSID.[BSSID==\"34:E3:80:76:01:22\"].SSID", "value":"test-2g"}'
{
	"parameters": [
		{
			"status": true,
			"path": "Device.WiFi.SSID.1.SSID"
		}
	]
}

root@iopsys:~# ubus call usp get '{"path":"Device.WiFi.SSID.[BSSID==\"34:E3:80:76:01:22\"].SSID"}'?
{
        "SSID": [
                {
                        "SSID": "test-2g"
                }
        ]
}
````
- For more info on the `usp` ubus API see [link](./docs/api/usp.md#set)
- For more info on the `usp.raw` ubus API see [link](./docs/api/usp.raw.md#set)

#### Add object
API to add new objects in multi-instance object

````bash
root@iopsys:~# ubus call usp add_object '{"path":"Device.Users.User"}'
{
        "status": true,
        "instance": "4"
}
````
- For more info on the `usp` ubus API see [link](./docs/api/usp.md#add_object)
- For more info on the `usp.raw` ubus API see [link](./docs/api/usp.raw.md#add_object)

#### Delete object
API to delete an existing object from multi-instance object

````bash
root@iopsys:~# ubus call usp del_object '{"path":"Device.Users.User.[Username==\"user_2\"]."}'
{
	"parameters": [
		{
			"parameter": "Device.Users.User.2.",
			"status": true
		}
	]
}
````
- For more info on the `usp` ubus API see [link](./docs/api/usp.md#del_object)
- For more info on the `usp.raw` ubus API see [link](./docs/api/usp.raw.md#del_object)

#### Operate
API to run operate/diagnostics commands as defined in TR-369

````bash
root@iopsys:~# ubus call usp operate '{"path":"Device.IP.Diagnostics.", "action":"IPPing","input":{"Host":"iopsys.eu"}}'
{
        "AverageResponseTime": "0",
        "AverageResponseTimeDetailed": "0",
        "FailureCount": "3",
        "MaximumResponseTime": "0",
        "MaximumResponseTimeDetailed": "0",
        "MinimumResponseTime": "9999",
        "MinimumResponseTimeDetailed": "999999999",
        "SuccessCount": "0"
}


root@iopsys:~# ubus call usp operate '{"path":"Device.IP.Interface.[Name==\"wan\"]", "action":"Reset"}'
{
        "status": true
}
````
- For more info on the `usp` ubus API see [link](./docs/api/usp.md#operate)
- For more info on the `usp.raw` ubus API see [link](./docs/api/usp.raw.md#operate)

#### Instances
API to get the available instances of an multi-instance object. USP Instances method returns the registered instances.

````bash
root@iopsys:~# ubus call usp instances '{"path":"Device.IP.Interface.", "proto":"usp"}'
{
        "parameters": [
                {
                        "parameter": "Device.IP.Interface.1."
                },
                {
                        "parameter": "Device.IP.Interface.1.IPv4Address.1."
                },
                {
                        "parameter": "Device.IP.Interface.2."
                },
                {
                        "parameter": "Device.IP.Interface.3."
                },
                {
                        "parameter": "Device.IP.Interface.3.IPv4Address.1."
                },
                {
                        "parameter": "Device.IP.Interface.3.IPv6Address.1."
                },
                {
                        "parameter": "Device.IP.Interface.3.IPv6Prefix.1."
                }
        ]
}
````
- For more info on the `usp` ubus API see [link](./docs/api/usp.md#instances)
- For more info on the `usp.raw` ubus API see [link](./docs/api/usp.raw.md#instances)

### More example ###

````bash
root@iopsys:~# ubus call usp get '{"path":"Device.WiFi.SSID.*.SSID"}'
{
        "SSID": [
                {
                        "SSID": "NORRLAND-34E380760120"
                },
                {
                        "SSID": "NORRLAND-34E380760120"
                }
        ]
}

root@iopsys:~# ubus call usp get '{"path":"Device.WiFi.SSID.*.BSSID"}'
{
        "SSID": [
                {
                        "BSSID": "34:E3:80:76:01:22"
                },
                {
                        "BSSID": "34:E3:80:76:01:23"
                }
        ]
}

root@iopsys:~# ubus call usp get '{"path":"Device.WiFi.SSID.[BSSID==\"34:E3:80:76:01:22\"].SSID"}'
{
        "SSID": [
                {
                        "SSID": "NORRLAND-34E380760120"
                }
        ]
}

root@iopsys:~# ubus call usp get '{"path":"Device.IP.Interface.[Status==\"Up\"].IPv4Address.[AddressingType==\"DHCP\"].IPAddress"}'
{
        "Interface": [
                {
                        "IPv4Address": [
                                {
                                        "IPAddress": "192.168.0.96"
                                }
                        ]
                }
        ]
}

root@iopsys:~# ubus call usp get '{"path":"Device.IP.Interface.[Status==\"Up\"].IPv4Address.[AddressingType==\"DHCP\"&&Status==\"Up\"]."}'
{
        "Interface": [
                {
                        "IPv4Address": [
                                {
                                        "AddressingType": "DHCP",
                                        "Alias": "cpe-2",
                                        "Enable": true,
                                        "IPAddress": "192.168.0.96",
                                        "Status": "Up",
                                        "SubnetMask": "255.255.255.0",
                                        "X_IOPSYS_EU_FirewallEnabled": true
                                }
                        ]
                }
        ]
}

root@iopsys:~# ubus call usp get '{"path":"Device.IP.Interface.[Type==\"Normal\"&&Stats.PacketsSent<=500].IPv4Address.[AddressingType==\"Static\"].IPAddress"}'
{
        "Interface": [
                {
                        "IPv4Address": [
                                {
                                        "IPAddress": "192.168.1.1"
                                }
                        ]
                }
        ]
}

root@iopsys:~# ubus call usp get '{"path":"Device.WiFi.AccessPoint.[SSIDReference+.SSID==\"NORRLAND-34E380760120\"].AssociatedDevice.[Noise>15].SignalStrength"}
'
{
        "AccessPoint": [
                {
                        "AssociatedDevice": [
                                {
                                        "SignalStrength": -31
                                }
                        ]
                }
        ]
}


root@iopsys:~# ubus call usp get '{"path":"Device.WiFi.SSID.*.LowerLayers#1+.Name"}'
{
        {
                "Name": "wlan0",
                "Name": "wlan2"
        }
}



root@iopsys:~# ubus call usp get '{"path":"Device.Users.User.*.Username"}'
{
        "User": [
                {
                        "Username": "user"
                },
                {
                        "Username": "support"
                },
                {
                        "Username": "admin"
                }
        ]
}
````

- For more info on the usp ubus API see [link](./docs/api/usp.md)
- For more info on the usp.raw ubus API see [link](./docs/api/usp.raw.md)


## Concepts and Workflow

uspd internally uses libbbfdm to get the datamodel objects. On startup it parses the uci file to check if the granularity is set and then as per the granularity value it registers the required ubus namespaces.

When a ubus method is called it first checks the `path` parameter to identify if it has special USP syntax, if present it parses and determine the correct objects from libbbfdm, then proceeds with the `Get/Set/Operate/Add/Del` operation on the quilified objects.

So, `uspd` search for `[[+*]+` in path expression, if it matches then segments the path and get the schema from `libbbfdm` and store it in a link-list, then it proceeds with the next segment to filter out the unneeded schema paths. It keeps on doing so till all the expressions are solved and it finally left with qualified objects.
Once all the expressions are solved, it starts getting the values for qualified objects and store it in a `stack` to print the output in pretty format.

For operate command, it solve the path expression and then call `bbf_operate` from `libbbfdm` to execute the operate command.

`uspd` uses `dm_entry_param_method` API from `libbbfdm` to get the device tree schema and it's values.

In short, it covers/supports the new syntax introduced in `TR-369` by using the existing datamodel available with libbbfdm.

## Dependencies

### Build-Time Dependencies

To successfully build uspd, following libraries are needed:

| Dependency |                    Link                     |    License     |
| ---------- | ------------------------------------------- | -------------- |
| libuci     | https://git.openwrt.org/project/uci.git     | LGPL 2.1       |
| libubox    | https://git.openwrt.org/project/libubox.git | BSD            |
| libubus    | https://git.openwrt.org/project/ubus.git    | LGPL 2.1       |
| libjson-c  | https://s3.amazonaws.com/json-c_releases    | MIT            |
| libbbfdm   | https://dev.iopsys.eu/iopsys/bbf.git        | LGPL 2.1       |


### Run-Time Dependencies

In order to run the uspd, following dependencies are needed to be running/available before uspd.

| Dependency |                   Link                   | License  |
| ---------- | ---------------------------------------- | -------- |
| ubusd      | https://git.openwrt.org/project/ubus.git | LGPL 2.1 |
| libbbfdm   | https://dev.iopsys.eu/iopsys/bbf.git     | LGPL 2.1 |

System daemon ubusd is used to expose the USP functionality over ubus.
