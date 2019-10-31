# README #

uspd is a [TR-369/USP](https://usp.technology/) backend ubus daemon process to understand [USP syntax](https://usp.technology/specification/architecture/) defined in R-ARC.7 - R-ARC.12 and provide details.

## Project Components ##

Project consists of following components:

- Application itself written in C programming language
- Documentation in a Markdown format

## Build Instructions ##

uspd is written using C programming language and depends on a number of components found in OpenWrt for building and running.

## Usage ##

uspd needs to be started on startup after ubusd, as it exposes the USP functionality over ubus. By default(when granularity is not set in uci), uspd registers below two namespaces with ubus.

```
root@iopsys:~# ubus list |grep usp
usp
usp.raw
```

Each namespace provide the similar result but in different formats. `usp` namespace is to provide the output as required by `USP` or `End User` or in pretty format, whereas `usp.raw` namespace is to provide output in raw format, which can be used by other USP frontend applications(like: obuspa). Each namespace has below functionalities.

```
root@iopsys:~# ubus -v list usp
'usp' @5914738c
        "get":{"path":"String"}
        "set":{"path":"String","value":"String","values":"Table"}
        "operate":{"path":"String","action":"String","input":"Table"}
        "add_object":{"path":"String"}
        "del_object":{"path":"String"}
root@iopsys:~#
root@iopsys:~# ubus -v list usp.raw
'usp.raw' @c2aec62b
        "get":{"path":"String"}
        "set":{"path":"String","value":"String","values":"Table"}
        "operate":{"path":"String","action":"String","input":"Table"}
        "add_object":{"path":"String"}
        "del_object":{"path":"String"}
root@iopsys:~#
```

The objects registered with the above namespaces can be called with apppriate parameters to perform a USP `Get/Set/Operate/Add Object/Delete Object` operation as below.
To get the output in pretty format use `usp` ubus namespace:

```
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
```
To get the output in raw format use `usp.raw` ubus namespace:
```
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
```

Granularity feature is basically exposes the same USP functionality by registering additinal ubus namespaces to reduce the `path` length in ubus parameter. It is the number of levels upto which we want to shorten the length.

Ex:

 - When Granularity is set to 1, exposed ubus namespaces are

```
root@iopsys:~# ubus list|grep usp
usp
usp.Device
usp.raw
```

 - When Granularity is set to 2, exposed ubus namespaces are

```
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
```

These granular ubus objects provides the same functionality as of `usp` ubus namespace

```
root@iopsys:~# ubus -v list usp.Device.WiFi
'usp.Device.WiFi' @69650977
        "get":{"path":"String"}
        "set":{"path":"String","value":"String","values":"Table"}
        "operate":{"path":"String","action":"String","input":"Table"}
        "add_object":{"path":"String"}
        "del_object":{"path":"String"}
```

Registered method can be called with appropriate parameters, like:
```
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
```

## Configuration File ##

uspd requires a configuration file to provide more granular objects over ubus. Granularity is an optional feature of uspd, it can be skipped or set to level 0. The configuration file is an uci file `/etc/config/uspd`. Sample configuration file is provided below.

```
config uspd 'usp'
        option granularitylevel  '0'
```

## Concepts and Workflow ##

uspd internally uses libbbfdm to get the datamodel objects. On startup it parses the uci file to check if the granularity is set and then as per the granularity value it registers the required ubus namespaces.

When a ubus method is called it first checks the `path` parameter to identify if it has special USP syntax, if present it parses and determine the correct objects from libbbfdm, then proceeds with the `Get/Set/Operate/Add/Del` operation on the quilified objects.

In short, it covers/supports the new syntax introduced in `TR-369` by using the existing datamodel available with libbbfdm.

## Dependencies ##

### Build-Time Dependencies ###

To successfully build uspd, following libraries are needed:

| Dependency |                    Link                     |    License     |
| ---------- | ------------------------------------------- | -------------- |
| libuci     | https://git.openwrt.org/project/uci.git     | GPLv2          |
| libubox    | https://git.openwrt.org/project/libubox.git | BSD            |
| libubus    | https://git.openwrt.org/project/ubus.git    | LGPL 2.1       |
| libjson-c  | https://s3.amazonaws.com/json-c_releases    | MIT            |
| libbbfdm   | https://dev.iopsys.eu/iopsys/bbf.git        | GPLv2          |


### Run-Time Dependencies ###

In order to run the uspd, following dependencies are needed to be running before uspd.

| Dependency |                   Link                   | License  |
| ---------- | ---------------------------------------- | -------- |
| ubusd      | https://git.openwrt.org/project/ubus.git | LGPL 2.1 |

System daemon ubusd is used to expose the USP functionality over ubus.
