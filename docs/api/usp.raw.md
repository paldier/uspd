# usp.raw Schema

```
https://www.iopsys.eu/usp.raw.json
```

| Custom Properties | Additional Properties |
| ----------------- | --------------------- |
| Forbidden         | Forbidden             |

# usp.raw

| List of Methods               |
| ----------------------------- |
| [add_object](#add_object)     | Method | usp.raw (this schema) |
| [del_object](#del_object)     | Method | usp.raw (this schema) |
| [get](#get)                   | Method | usp.raw (this schema) |
| [instances](#instances)       | Method | usp.raw (this schema) |
| [object_names](#object_names) | Method | usp.raw (this schema) |
| [operate](#operate)           | Method | usp.raw (this schema) |
| [set](#set)                   | Method | usp.raw (this schema) |

## add_object

### Add a new object instance

Add a new object in multi instance object

`add_object`

- type: `Method`

### add_object Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `input`  | object | **Required** |
| `output` | object | Optional     |

#### input

`input`

- is **required**
- type: `object`

##### input Type

`object` with following properties:

| Property | Type   | Required     | Default  |
| -------- | ------ | ------------ | -------- |
| `path`   | string | **Required** |          |
| `proto`  | string | Optional     | `"both"` |

#### path

Complete object element path as per TR181-2.13

`path`

- is **required**
- type: reference

##### path Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\d\.)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.1.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.1.>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### path Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.1.
```

```json
Device.WiFi.
```

#### proto

`proto`

- is optional
- type: reference
- default: `"both"`

##### proto Type

`string`

All instances must conform to this regular expression (test examples
[here](<https://regexr.com/?expression=(usp%7Ccwmp%7Cboth)>)):

```regex
(usp|cwmp|both)
```

### Ubus CLI Example

```
ubus call usp.raw add_object {"path":"Device.vTv.VQyn.fKoV.MsEURRa.V.2.KculFptLFI.Z.ofRtoScbNqy.5.SyYN","proto":"usp"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "usp.raw",
    "add_object",
    { "path": "Device.vTv.VQyn.fKoV.MsEURRa.V.2.KculFptLFI.Z.ofRtoScbNqy.5.SyYN", "proto": "usp" }
  ]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property   | Type    | Required |
| ---------- | ------- | -------- |
| `fault`    | integer | Optional |
| `instance` | integer | Optional |
| `status`   | boolean | Optional |

#### fault

`fault`

- is optional
- type: reference

##### fault Type

`integer`

- minimum value: `9000`
- maximum value: `9032`

#### instance

`instance`

- is optional
- type: `integer`

##### instance Type

`integer`

- minimum value: `1`

#### status

`status`

- is optional
- type: `boolean`

##### status Type

`boolean`

### Output Example

```json
{ "status": true, "instance": 10254321, "fault": 9000 }
```

## del_object

### Delete object instance

Delete a object instance from multi instance object

`del_object`

- type: `Method`

### del_object Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `input`  | object | **Required** |
| `output` | object | Optional     |

#### input

`input`

- is **required**
- type: `object`

##### input Type

`object` with following properties:

| Property | Type   | Required     | Default  |
| -------- | ------ | ------------ | -------- |
| `path`   | string | **Required** |          |
| `proto`  | string | Optional     | `"both"` |

#### path

Complete object element path as per TR181-2.13 and TR369

`path`

- is **required**
- type: reference

##### path Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\[[a-zA-Z0-9-_+=!><&]*\]|(\d\.)|\*)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.[SSID&amp;#x3D;&amp;#x3D;&amp;quot;test_ssid&amp;quot;].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID%3D%3D%22test_ssid%22%5D.BSSID>)
- test example:
  [Device.WiFi.SSID.\*.BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.*.BSSID>)
- test example:
  [Device.WiFi.SSID.[SSID!&amp;#x3D;&amp;quot;test_ssid&amp;quot;&amp;amp;&amp;amp;Enable&amp;#x3D;&amp;#x3D;1].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID!%3D%22test_ssid%22%26%26Enable%3D%3D1%5D.BSSID>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### path Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.[SSID=="test_ssid"].BSSID
```

```json
Device.WiFi.SSID.*.BSSID
```

```json
Device.WiFi.SSID.[SSID!="test_ssid"&&Enable==1].BSSID
```

```json
Device.WiFi.
```

#### proto

`proto`

- is optional
- type: reference
- default: `"both"`

##### proto Type

`string`

All instances must conform to this regular expression (test examples
[here](<https://regexr.com/?expression=(usp%7Ccwmp%7Cboth)>)):

```regex
(usp|cwmp|both)
```

### Ubus CLI Example

```
ubus call usp.raw del_object {"path":"Device.wLP.sDOUwdGRNLk.dR.XIKUrnd.w.XyxGJ.MjTDPSrna.MvlfofkvDvs.","proto":"usp"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "usp.raw",
    "del_object",
    { "path": "Device.wLP.sDOUwdGRNLk.dR.XIKUrnd.w.XyxGJ.MjTDPSrna.MvlfofkvDvs.", "proto": "usp" }
  ]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type    | Required |
| -------- | ------- | -------- |
| `fault`  | integer | Optional |
| `status` | boolean | Optional |

#### fault

`fault`

- is optional
- type: reference

##### fault Type

`integer`

- minimum value: `9000`
- maximum value: `9032`

#### status

`status`

- is optional
- type: `boolean`

##### status Type

`boolean`

### Output Example

```json
{ "status": false, "fault": 9010 }
```

## get

### Get handler

Query the datamodel object

`get`

- type: `Method`

### get Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `input`  | object | **Required** |
| `output` | object | Optional     |

#### input

`input`

- is **required**
- type: `object`

##### input Type

`object` with following properties:

| Property | Type   | Required     | Default  |
| -------- | ------ | ------------ | -------- |
| `path`   | string | **Required** |          |
| `proto`  | string | Optional     | `"both"` |

#### path

Complete object element path as per TR181-2.13 and TR369

`path`

- is **required**
- type: reference

##### path Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\[[a-zA-Z0-9-_+=!><&]*\]|(\d\.)|\*)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.[SSID&amp;#x3D;&amp;#x3D;&amp;quot;test_ssid&amp;quot;].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID%3D%3D%22test_ssid%22%5D.BSSID>)
- test example:
  [Device.WiFi.SSID.\*.BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.*.BSSID>)
- test example:
  [Device.WiFi.SSID.[SSID!&amp;#x3D;&amp;quot;test_ssid&amp;quot;&amp;amp;&amp;amp;Enable&amp;#x3D;&amp;#x3D;1].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID!%3D%22test_ssid%22%26%26Enable%3D%3D1%5D.BSSID>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### path Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.[SSID=="test_ssid"].BSSID
```

```json
Device.WiFi.SSID.*.BSSID
```

```json
Device.WiFi.SSID.[SSID!="test_ssid"&&Enable==1].BSSID
```

```json
Device.WiFi.
```

#### proto

`proto`

- is optional
- type: reference
- default: `"both"`

##### proto Type

`string`

All instances must conform to this regular expression (test examples
[here](<https://regexr.com/?expression=(usp%7Ccwmp%7Cboth)>)):

```regex
(usp|cwmp|both)
```

### Ubus CLI Example

```
ubus call usp.raw get {"path":"Device.qo.rgTV.X.qWnvwFMKomP.AczcHZev.g.guEQYm.sJSNv.3.RVpSA.IHS","proto":"usp"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "usp.raw",
    "get",
    { "path": "Device.qo.rgTV.X.qWnvwFMKomP.AczcHZev.g.guEQYm.sJSNv.3.RVpSA.IHS", "proto": "usp" }
  ]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property     | Type  | Required     |
| ------------ | ----- | ------------ |
| `parameters` | array | **Required** |

#### parameters

`parameters`

- is **required**
- type: `object[]`

##### parameters Type

Array type: `object[]`

All items must be of the type: `object` with following properties:

| Property    | Type   | Required |
| ----------- | ------ | -------- |
| `parameter` | string | Optional |
| `type`      | string | Optional |
| `value`     | string | Optional |

#### parameter

Complete object element path as per TR181-2.13

`parameter`

- is optional
- type: reference

##### parameter Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\d\.)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.1.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.1.>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### parameter Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.1.
```

```json
Device.WiFi.
```

#### type

`type`

- is optional
- type: reference

##### type Type

`string`

All instances must conform to this regular expression (test examples
[here](<https://regexr.com/?expression=xsd%3A(string%7CunsignedInt%7Cint%7CunsignedLong%7Clong%7Cboolean%7CdateTime%7ChexBinary)>)):

```regex
xsd:(string|unsignedInt|int|unsignedLong|long|boolean|dateTime|hexBinary)
```

#### value

`value`

- is optional
- type: `string`

##### value Type

`string`

### Output Example

```json
{
  "parameters": [
    { "parameter": "Device.xHx.uu.8.", "value": "sint ", "type": "xsd:long" },
    { "parameter": "Device.ZveFXSUyJ.7.", "value": "do officia reprehenderit est", "type": "xsd:unsignedLong" },
    { "parameter": "Device.1.JfDqkttL", "value": "enim aliquip dolore", "type": "xsd:string" },
    {
      "parameter": "Device.eRTQCXQeJr.gQqoxx.uNcS.KIGLTq.eepKX.FJS.OMDtHaQtV.5.WrXSp",
      "value": "in",
      "type": "xsd:string"
    },
    { "parameter": "Device.lH", "value": "cillum laboris quis adipisicing dolor", "type": "xsd:int" }
  ]
}
```

## instances

### Instance query handler

Get the instances of multi object

`instances`

- type: `Method`

### instances Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `input`  | object | **Required** |
| `output` | object | Optional     |

#### input

`input`

- is **required**
- type: `object`

##### input Type

`object` with following properties:

| Property | Type   | Required     | Default  |
| -------- | ------ | ------------ | -------- |
| `path`   | string | **Required** |          |
| `proto`  | string | Optional     | `"both"` |

#### path

Complete object element path as per TR181-2.13 and TR369

`path`

- is **required**
- type: reference

##### path Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\[[a-zA-Z0-9-_+=!><&]*\]|(\d\.)|\*)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.[SSID&amp;#x3D;&amp;#x3D;&amp;quot;test_ssid&amp;quot;].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID%3D%3D%22test_ssid%22%5D.BSSID>)
- test example:
  [Device.WiFi.SSID.\*.BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.*.BSSID>)
- test example:
  [Device.WiFi.SSID.[SSID!&amp;#x3D;&amp;quot;test_ssid&amp;quot;&amp;amp;&amp;amp;Enable&amp;#x3D;&amp;#x3D;1].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID!%3D%22test_ssid%22%26%26Enable%3D%3D1%5D.BSSID>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### path Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.[SSID=="test_ssid"].BSSID
```

```json
Device.WiFi.SSID.*.BSSID
```

```json
Device.WiFi.SSID.[SSID!="test_ssid"&&Enable==1].BSSID
```

```json
Device.WiFi.
```

#### proto

`proto`

- is optional
- type: reference
- default: `"both"`

##### proto Type

`string`

All instances must conform to this regular expression (test examples
[here](<https://regexr.com/?expression=(usp%7Ccwmp%7Cboth)>)):

```regex
(usp|cwmp|both)
```

### Ubus CLI Example

```
ubus call usp.raw instances {"path":"Device.JlroxVewXIu.GHxjlSmJp.uUUbGUFazjz.RZRUW.jqGPjra.sd.Qnmo.L","proto":"cwmp"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "usp.raw",
    "instances",
    { "path": "Device.JlroxVewXIu.GHxjlSmJp.uUUbGUFazjz.RZRUW.jqGPjra.sd.Qnmo.L", "proto": "cwmp" }
  ]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property     | Type  | Required     |
| ------------ | ----- | ------------ |
| `parameters` | array | **Required** |

#### parameters

`parameters`

- is **required**
- type: `object[]`

##### parameters Type

Array type: `object[]`

All items must be of the type: `object` with following properties:

| Property    | Type   | Required |
| ----------- | ------ | -------- |
| `parameter` | string | Optional |

#### parameter

Multi object instances

`parameter`

- is optional
- type: reference

##### parameter Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*.%5Cd>)):

```regex
Device\.(([a-zA-Z]+\.)*(\d\.)?)*[a-zA-Z]*.\d
```

### Output Example

```json
{
  "parameters": [
    { "parameter": "Device.XV.SCQEPKrLFsi.hEXGDbdSEwO.P.qYmjJx.wcrI.3.GnSjZEz.elBPzN" },
    { "parameter": "Device.WeCVrpWJwY(2" },
    { "parameter": "Device.XRYRCNFrDzM.Ze.VUhLrmLTT.onRX.JXP.e.nNROY.HmOxmPbdZF.hmCd" },
    { "parameter": "Device.tloQsSPT.cPOjUwXgTmI.BWGGxG.Pdhyif.qscWyr.hr.KPAe.iPVHQUm" },
    { "parameter": "Device.xAbUVXo6" }
  ]
}
```

## object_names

### Get objects names

Get names of all the objects below input object path

`object_names`

- type: `Method`

### object_names Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `input`  | object | **Required** |
| `output` | object | Optional     |

#### input

`input`

- is **required**
- type: `object`

##### input Type

`object` with following properties:

| Property | Type   | Required     | Default  |
| -------- | ------ | ------------ | -------- |
| `path`   | string | **Required** |          |
| `proto`  | string | Optional     | `"both"` |

#### path

Complete object element path as per TR181-2.13 and TR369

`path`

- is **required**
- type: reference

##### path Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\[[a-zA-Z0-9-_+=!><&]*\]|(\d\.)|\*)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.[SSID&amp;#x3D;&amp;#x3D;&amp;quot;test_ssid&amp;quot;].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID%3D%3D%22test_ssid%22%5D.BSSID>)
- test example:
  [Device.WiFi.SSID.\*.BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.*.BSSID>)
- test example:
  [Device.WiFi.SSID.[SSID!&amp;#x3D;&amp;quot;test_ssid&amp;quot;&amp;amp;&amp;amp;Enable&amp;#x3D;&amp;#x3D;1].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID!%3D%22test_ssid%22%26%26Enable%3D%3D1%5D.BSSID>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### path Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.[SSID=="test_ssid"].BSSID
```

```json
Device.WiFi.SSID.*.BSSID
```

```json
Device.WiFi.SSID.[SSID!="test_ssid"&&Enable==1].BSSID
```

```json
Device.WiFi.
```

#### proto

`proto`

- is optional
- type: reference
- default: `"both"`

##### proto Type

`string`

All instances must conform to this regular expression (test examples
[here](<https://regexr.com/?expression=(usp%7Ccwmp%7Cboth)>)):

```regex
(usp|cwmp|both)
```

### Ubus CLI Example

```
ubus call usp.raw object_names {"path":"Device.zRhCMzna","proto":"usp"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "usp.raw", "object_names", { "path": "Device.zRhCMzna", "proto": "usp" }]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property     | Type  | Required     |
| ------------ | ----- | ------------ |
| `parameters` | array | **Required** |

#### parameters

`parameters`

- is **required**
- type: `object[]`

##### parameters Type

Array type: `object[]`

All items must be of the type: `object` with following properties:

| Property    | Type   | Required |
| ----------- | ------ | -------- |
| `parameter` | string | Optional |

#### parameter

Complete object element path as per TR181-2.13

`parameter`

- is optional
- type: reference

##### parameter Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\d\.)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.1.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.1.>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### parameter Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.1.
```

```json
Device.WiFi.
```

### Output Example

```json
{
  "parameters": [
    { "parameter": "Device.wWgGJYAmhSj.LaJfneU.SHGvqOA.EjCbSpea.YtNFq.AXJXtLkt.pBDVd" },
    { "parameter": "Device.Xnr.xFmigNaxS.NpFWqXwcJzu.hBaBz.zRYjWpxu.WtTmFZFS.KhVjsLT" },
    { "parameter": "Device.cqFmG" },
    { "parameter": "Device.DoTW.OsjtuHib.Mzs.tamrdwsj.CUOeXoNt.AglMNeF.HDnLQfmkuCw.u" },
    { "parameter": "Device.W.xcGtDYbjU.8.NzxNZzSVog.hWXxYblPUd.M.uBihci.FqdUmB.ZPZYj" }
  ]
}
```

## operate

### Operate handler

Operate on object element provided in path

`operate`

- type: `Method`

### operate Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `input`  | object | **Required** |
| `output` | object | Optional     |

#### input

`input`

- is **required**
- type: `object`

##### input Type

`object` with following properties:

| Property | Type   | Required     | Default  |
| -------- | ------ | ------------ | -------- |
| `action` | string | **Required** |          |
| `input`  | object | Optional     |          |
| `path`   | string | **Required** |          |
| `proto`  | string | Optional     | `"both"` |

#### action

Opreate command as defined in TR-369, TR-181-2.13

`action`

- is **required**
- type: `string`

##### action Type

`string`

All instances must conform to this regular expression

```regex
[a-zA-Z]+\(\)
```

- test example:
  [{&amp;quot;path&amp;quot;:&amp;quot;Device.WiFi.&amp;quot;, &amp;quot;action&amp;quot;:&amp;quot;Reset\(\)&amp;quot;}](<https://regexr.com/?expression=%5Ba-zA-Z%5D%2B%5C(%5C)&text=%7B%22path%22%3A%22Device.WiFi.%22%2C%20%22action%22%3A%22Reset%5C(%5C)%22%7D>)

##### action Example

```json
{ "path": "Device.WiFi.", "action": "Reset()" }
```

#### input

Input arguments for the operate command as defined in TR-181-2.13

`input`

- is optional
- type: `object`

##### input Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

##### input Example

```json
{ "path": "Device.IP.Diagnostics", "action": "IPPing()", "input": { "Host": "iopsys.eu" } }
```

#### path

Complete object element path as per TR181-2.13 and TR369

`path`

- is **required**
- type: reference

##### path Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\[[a-zA-Z0-9-_+=!><&]*\]|(\d\.)|\*)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.[SSID&amp;#x3D;&amp;#x3D;&amp;quot;test_ssid&amp;quot;].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID%3D%3D%22test_ssid%22%5D.BSSID>)
- test example:
  [Device.WiFi.SSID.\*.BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.*.BSSID>)
- test example:
  [Device.WiFi.SSID.[SSID!&amp;#x3D;&amp;quot;test_ssid&amp;quot;&amp;amp;&amp;amp;Enable&amp;#x3D;&amp;#x3D;1].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID!%3D%22test_ssid%22%26%26Enable%3D%3D1%5D.BSSID>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### path Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.[SSID=="test_ssid"].BSSID
```

```json
Device.WiFi.SSID.*.BSSID
```

```json
Device.WiFi.SSID.[SSID!="test_ssid"&&Enable==1].BSSID
```

```json
Device.WiFi.
```

#### proto

`proto`

- is optional
- type: reference
- default: `"both"`

##### proto Type

`string`

All instances must conform to this regular expression (test examples
[here](<https://regexr.com/?expression=(usp%7Ccwmp%7Cboth)>)):

```regex
(usp|cwmp|both)
```

### Ubus CLI Example

```
ubus call usp.raw operate {"path":"Device.SoZcSFnX.OPTVFzwYk.2.YBUrkH","action":"DUIlucIUlp()","proto":"usp","input":{}}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "usp.raw",
    "operate",
    { "path": "Device.SoZcSFnX.OPTVFzwYk.2.YBUrkH", "action": "DUIlucIUlp()", "proto": "usp", "input": {} }
  ]
}
```

#### output

Output will have status for sync commands and for async commands parameters as defined in TR-181-2.13

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

##### output Examples

```json
{
  "status": true
}
```

```json
{
  "AverageResponseTime": "0",
  "AverageResponseTimeDetailed": "130",
  "FailureCount": "0",
  "MaximumResponseTime": "0",
  "MaximumResponseTimeDetailed": "140",
  "MinimumResponseTime": "0",
  "MinimumResponseTimeDetailed": "120",
  "SuccessCount": "3"
}
```

### Output Example

```json
{}
```

## set

### Set handler

Set values of datamodel object element

`set`

- type: `Method`

### set Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `input`  | object | **Required** |
| `output` | object | Optional     |

#### input

`input`

- is **required**
- type: `object`

##### input Type

`object` with following properties:

| Property | Type   | Required     | Default  |
| -------- | ------ | ------------ | -------- |
| `path`   | string | **Required** |          |
| `proto`  | string | Optional     | `"both"` |
| `value`  | string | **Required** |          |
| `values` | object | Optional     |          |

#### path

Complete object element path as per TR181-2.13 and TR369

`path`

- is **required**
- type: reference

##### path Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\[[a-zA-Z0-9-_+=!><&]*\]|(\d\.)|\*)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.[SSID&amp;#x3D;&amp;#x3D;&amp;quot;test_ssid&amp;quot;].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID%3D%3D%22test_ssid%22%5D.BSSID>)
- test example:
  [Device.WiFi.SSID.\*.BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.*.BSSID>)
- test example:
  [Device.WiFi.SSID.[SSID!&amp;#x3D;&amp;quot;test_ssid&amp;quot;&amp;amp;&amp;amp;Enable&amp;#x3D;&amp;#x3D;1].BSSID](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.%5BSSID!%3D%22test_ssid%22%26%26Enable%3D%3D1%5D.BSSID>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5C%5B%5Ba-zA-Z0-9-_%2B%3D!%3E%3C%26%5D*%5C%5D%7C(%5Cd%5C.)%7C%5C*)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### path Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.[SSID=="test_ssid"].BSSID
```

```json
Device.WiFi.SSID.*.BSSID
```

```json
Device.WiFi.SSID.[SSID!="test_ssid"&&Enable==1].BSSID
```

```json
Device.WiFi.
```

#### proto

`proto`

- is optional
- type: reference
- default: `"both"`

##### proto Type

`string`

All instances must conform to this regular expression (test examples
[here](<https://regexr.com/?expression=(usp%7Ccwmp%7Cboth)>)):

```regex
(usp|cwmp|both)
```

#### value

value of the object element provided in path, path should contains valid writable object element

`value`

- is **required**
- type: `string`

##### value Type

`string`

All instances must conform to this regular expression

```regex
[a-zA-Z]+
```

- test example:
  [{&amp;quot;path&amp;quot;:&amp;quot;Device.WiFi.SSID.1.SSID&amp;quot;, &amp;quot;value&amp;quot;:&amp;quot;test_ssid&amp;quot;}](https://regexr.com/?expression=%5Ba-zA-Z%5D%2B&text=%7B%22path%22%3A%22Device.WiFi.SSID.1.SSID%22%2C%20%22value%22%3A%22test_ssid%22%7D)
- test example:
  [{&amp;quot;path&amp;quot;:&amp;quot;Device.WiFi.SSID.2.Enable&amp;quot;, &amp;quot;value&amp;quot;:&amp;quot;true&amp;quot;}](https://regexr.com/?expression=%5Ba-zA-Z%5D%2B&text=%7B%22path%22%3A%22Device.WiFi.SSID.2.Enable%22%2C%20%22value%22%3A%22true%22%7D)
- test example:
  [{&amp;quot;path&amp;quot;:&amp;quot;Device.WiFi.SSID.1.Enable&amp;quot;, &amp;quot;value&amp;quot;:&amp;quot;0&amp;quot;}](https://regexr.com/?expression=%5Ba-zA-Z%5D%2B&text=%7B%22path%22%3A%22Device.WiFi.SSID.1.Enable%22%2C%20%22value%22%3A%220%22%7D)

##### value Examples

```json
{ "path": "Device.WiFi.SSID.1.SSID", "value": "test_ssid" }
```

```json
{ "path": "Device.WiFi.SSID.2.Enable", "value": "true" }
```

```json
{ "path": "Device.WiFi.SSID.1.Enable", "value": "0" }
```

#### values

To set multiple values at once, path should be relative to object elements

`values`

- is optional
- type: `object`

##### values Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

##### values Examples

```json
{ "path": "Device.WiFi.SSID.1", "values": { ".SSID": "test_ssid", ".Name": "test_name" } }
```

```json
{ "path": "Device.WiFi.SSID.2", "values": { ".SSID": "test_ssid" } }
```

### Ubus CLI Example

```
ubus call usp.raw set {"path":"Device.UQyKps.FeByxMa.to.tyq.nwXm.KjVpuffc.v.Rtoh.wDNaFMCt.mmj.e","value":"qNysRHGx","proto":"usp","values":{}}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "usp.raw",
    "set",
    {
      "path": "Device.UQyKps.FeByxMa.to.tyq.nwXm.KjVpuffc.v.Rtoh.wDNaFMCt.mmj.e",
      "value": "qNysRHGx",
      "proto": "usp",
      "values": {}
    }
  ]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property     | Type  | Required     |
| ------------ | ----- | ------------ |
| `parameters` | array | **Required** |

#### parameters

`parameters`

- is **required**
- type: `object[]`

##### parameters Type

Array type: `object[]`

All items must be of the type: `object` with following properties:

| Property | Type    | Required |
| -------- | ------- | -------- |
| `fault`  | integer | Optional |
| `path`   | string  | Optional |
| `status` | boolean | Optional |

#### fault

`fault`

- is optional
- type: reference

##### fault Type

`integer`

- minimum value: `9000`
- maximum value: `9032`

#### path

Complete object element path as per TR181-2.13

`path`

- is optional
- type: reference

##### path Type

`string`

- minimum length: 6 characters
- maximum length: 256 characters All instances must conform to this regular expression

```regex
Device\.(([a-zA-Z]+\.)*(\d\.)?)*[a-zA-Z]*
```

- test example:
  [Device.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.>)
- test example:
  [Device.DeviceInfo.Manufacturer](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.DeviceInfo.Manufacturer>)
- test example:
  [Device.WiFi.SSID.1.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.SSID.1.>)
- test example:
  [Device.WiFi.](<https://regexr.com/?expression=Device%5C.((%5Ba-zA-Z%5D%2B%5C.)*(%5Cd%5C.)%3F)*%5Ba-zA-Z%5D*&text=Device.WiFi.>)

##### path Examples

```json
Device.
```

```json
Device.DeviceInfo.Manufacturer
```

```json
Device.WiFi.SSID.1.
```

```json
Device.WiFi.
```

#### status

`status`

- is optional
- type: `boolean`

##### status Type

`boolean`

### Output Example

```json
{
  "parameters": [
    { "path": "Device.PZZPAsTS.yjWaclHSOi.Yy.ybbgp.3.kXmNo.szatbC.TlkHHZAdkyU.l", "status": true, "fault": 9022 }
  ]
}
```
