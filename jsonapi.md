# Contents
- [JSON API](#json-api)
  * [Example API Call](#example-api-call)
    + [Row HTTP conversation](#row-http-conversation)
    + [Calling using the Fetch API](#calling-using-the-fetch-api)
- [Commands](#commands)
  * [Current Data](#current-data)
    + [GETLATEST](#getlatest)
      - [Parameters](#parameters)
      - [Response](#response)
      - [Example](#example)
    + [SYSTEMINFO](#systeminfo)
      - [Parameters](#parameters-1)
      - [Response](#response-1)
      - [Example](#example-1)
  * [Settings Management](#settings-management)
    + [LOADSETTINGS](#loadsettings)
      - [Parameters](#parameters-2)
      - [Response](#response-2)
      - [Example](#example-2)
    + [SAVESETTINGS](#savesettings)
      - [Parameters](#parameters-3)
      - [Response](#response-3)
      - [Example](#example-3)
  * [Network Management](#network-management)
    + [GETNETWORKINFO](#getnetworkinfo)
      - [Parameters](#parameters-4)
      - [Response](#response-4)
      - [Example](#example-4)
    + [GETNETWORKS](#getnetworks)
      - [Parameters](#parameters-5)
      - [Response](#response-5)
      - [Example](#example-5)
    + [SELECTNETWORK](#selectnetwork)
      - [Parameters](#parameters-6)
      - [Response](#response-6)
      - [Example - Test Mode](#example---test-mode)
      - [Example - Apply Mode](#example---apply-mode)
      - [Example - Status Mode](#example---status-mode)
  * [CO2 Calibration](#co2-calibration)
    + [GETLATESTCALIB](#getlatestcalib)
      - [Parameters](#parameters-7)
      - [Response](#response-7)
      - [Example](#example-6)
    + [MANUALCALIB](#manualcalib)
      - [Parameters](#parameters-8)
      - [Response](#response-8)
      - [Example](#example-7)
    + [ENABLEABC](#enableabc)
      - [Parameters](#parameters-9)
      - [Response](#response-9)
      - [Example](#example-8)
    + [DISABLEABC](#disableabc)
      - [Parameters](#parameters-10)
      - [Response](#response-10)
      - [Example](#example-9)


# JSON API
To implement a modern Web UI experience the C++ core of the firmware serves a HTML5/JavaScript application using it's inbuilt HTTP server that talks to the device using a JSON API, which is also served from the inbuilt HTTP server on the device. 

The HTML5/JavaScript application is currently written utilizing the ReactJS framework this any comparable web framework can be utilized, The Web UI can be replaced by simply replacing the contents of the "web" partition with another set of HTML5/JavaScript files which call the JSON API described in this document.

This approach has two advantages. First that it allows customization and "re-skinning" of the user interface without disrupting the core "business logic" of the firmware. Secondly it significantly simplifies the C++ core as that deals only with commands issued through the JSON API.






## Example API Call
Each command is issued as a post request to "/command" on the devices HTTP endpoint.  The body of the post request is a JSON object with a property called "COMMAND" which has the name of the command to execute. If the command requires additional parameters they must be included as properties of this JSON object.

### Row HTTP conversation
At a raw HTTP level the command to retrieve the latest readings is as follows.

    POST /command
    HTTP/1.1 
    Host: HOSTADDR
    Connection: keep-alive 
    Content-Length: 23 
    User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.164 Safari/537.36 Edg/91.0.864.71 
    Content-Type: application/json;charset=UTF-8 
    Accept: */* 
    Origin: http://HOSTADDR
    Referer: http://HOSTADDR/index.html 
    Accept-Encoding: gzip, deflate 
    Accept-Language: en-GB,en;q=0.9,en-US;q=0.8
	
	{"COMMAND":"GETLATEST"}

Which receives this as a response:-

	HTTP/1.1 200 OK 
	Content-Type: application/json;charset=UTF-8 
	Content-Length: 1053
	
	{
		"Status":	  		  "true",
		"HasCo2":			  "true",
		"Co2":				  "577",
		"Co2Max":			  "5000",
		"Co2Heating":		  "false",
		"Co2Error":			  "false",
		"Co2Name":	 		  "Senseair I2C",
		"Co2SwVersion":		  "3.0",
		"Co2SerialNo":	      "0329AD96",
		"Co2AdditionalValues":	[
			{
				"Name":	"Measurement Info",
				"Value":"2s since reading #151"
			}, {
				"Name":	"Firmware Type",
				"Value":"0"
			}, {
				"Name":	"Temperature",
				"Value":"24.24"
			}, {
				"Name":	"Fatal Error",
				"Value":"false"
			}, {
				"Name":	"I2C Error",
				"Value":"false"
			}, {
				"Name":	"Algorithm Error",
				"Value":"false"
			}, {
				"Name":	"Calibration Error",
				"Value":"false"
			}, {
				"Name":	"Self Diagnostics Error",
				"Value":"false"
			}, {
				"Name":	"Out of range Error",
				"Value":"false"
			}, {
				"Name":	"Memory Error",
				"Value":"false"
			}, {
				"Name":	"Low Voltage Error",
				"Value":"false"
			}, {
				"Name":	"Measurement Timeout Error",
				"Value":"false"
			}],
		"Temp":		"24.41",
		"Pressure":	"1007.1",
		"Humidity":	"58.38",
		"Time":		"2021-07-24T01:04:12Z"
    }


### Calling using the Fetch API

Normally one will execute commands using the FetchAPI if issuing commands within a HTML/JavaScript application. Here is an example calling the same command as above using the FetchAPI.

    var  cmd = { COMMAND:  "GETLATEST};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("CO2="+data.Co2);
		).catch(function() {
		    console.log("Error Executing Command");
	    });;

# Commands

## Current Data

### GETLATEST
This command gets the most recent values from the CO2,  temperature, pressure and humidity sensors. It also returns various other pieces of information that may be of use when displaying the current values to the user.
#### Parameters
None
#### Response
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded  |
|Error |String  | If Status = false then this property **may** be provided to described the error|
|HasCo2 |Boolean | A CO2 Sensor has been configured and is functioning|
|Co2 |Integer | The current measured CO2 reading in parts per million (PPM)|
|Co2Max |Integer | The maximum CO2 reading the sensor can measure|
|Co2Heating|Boolean | True if the CO2 sensor is heating up|
|Co2Error|Boolean | True if an error has been detected in the CO2 sensor|
|Co2Name|String | A string describing the type of CO2 sensor.|
|Co2SwVersion|String | The firmware version as read from the CO2 sensor|
|Co2SerialNo|String| The serial number read from the CO2 sensor|
|Co2AdditionalValues| Object Array| An array of objects each having Name & Value properties. Each type of sensor exposes values unique to each via this property. |
|Temp|Decimal | Current temperature in C.
|Pressure| Decimal| Current atmospheric pressure in hPa. |
|Humidity| Decimal| Current humidity as a percentage.
|Time|String| Current time in ISO format.

#### Example
    var  cmd = { COMMAND:  "GETLATEST};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("CO2="+data.Co2);
		).catch(function() {
		    console.log("Error Executing Command");
	    });;

### SYSTEMINFO
This command gets general information about the device micro-controller.

#### Parameters
None

#### Response
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|
|MCUName |String | Name of the Micro Controller |
|CoreCore |Integer | The number of processor cores in the Micro Controller|
|FreeHeap |Integer | The amount of heap memory in bytes currently available |
|LeastFreeHeap | Integer | The minimum amount of heap memory in bytes available since boot (e.g peak usage)|


#### Example
    var  cmd = { COMMAND:  "SYSTEMINFO"};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("FreeHeap="+data.FreeHeap);
		).catch(function() {
		    console.log("Error Executing Command");
	    });;
## Settings Management

### LOADSETTINGS
This command gets the current settings as stored within the configuration partition on the device's flash storage. It is use in conjunction with **SAVESETTINGS**  to implement the settings screen within the Web UI.

#### Parameters
None
#### Response
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|
|DeviceName|String | The network name for this device.|
|EnableMqtt|Boolean | Enable MQTT Telemetry.|
|EnableDhcpNtp|Boolean | If true NTP up to two NTP servers will be configured from DHCP.|
|MqttServerAddress|String | The address of the MQTT server in the form mqtt://..|
|MqttTopic|String | The name of the MQTT topic to which values will be published.|
|MqttPublishDelay|Int | How often to publish to the configured MQTT server in seconds.|
|SensorUpdateInterval|Int| How often to read values from the sensors in seconds|


#### Example
    var  cmd = { COMMAND:  "LOADSETTINGS"};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("DeviceName="+data.DeviceName);
		).catch(function() {
		    console.log("Error Executing Command");
	    });;

### SAVESETTINGS
This command sets some or all of the device settings. It is use in conjunction with **SAVESETTINGS**  to implement the settings screen within the Web UI.

#### Parameters
The command object sent to this command can have any of the following properties provided. If a property is present then it's value is updated.

Since the configuration is stored on the flash storage of the device it is preferable to set as many properties at once as is possible to minimize flash writes.

|Property|Data Type|Description|
|--|--|--|
|DeviceName|String | The network name for this device.|
|EnableMqtt|Boolean | Enable MQTT Telemetry.|
|EnableDhcpNtp|Boolean | If true NTP up to two NTP servers will be configured from DHCP.|
|MqttServerAddress|String | The address of the MQTT server in the form mqtt://..|
|MqttTopic|String | The name of the MQTT topic to which values will be published.|
|MqttPublishDelay|Int | How often to publish to the configured MQTT server in seconds.|
|SensorUpdateInterval|Int| How often to read values from the sensors in seconds|
None
#### Response
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|


#### Example
    var  cmd = { 
	    COMMAND:  "SAVESETTINGS",
	    DeviceName: "MyDevice1",
		SensorUpdateInterval: 16,
		MqttPublishDelay: 300
	};
	 

 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("Status="+data.Status);
		).catch(function() {
		    console.log("Error Executing Command");
	    });;


## Network Management

### GETNETWORKINFO
This command gets information about the current network configuration and status of the device. 

#### Parameters
None

#### Response
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|
|WifiSSID|String |The SSID of the wireless network to which this device is connected as a station. |
|WifiChannel|Integer | The 2.4ghz channel on which this is communicating. |
|WifiAuthMode|Integer | The wireless authentication mode in use. |
|WifiIP4Address| Integer | The IPv4 address of this device's wifi station interface.|
|WifiIP4Netmask| Integer | The IPv4 netmask of this device's wifi station interface.|
|WifiIP4Gateway| Integer | The IPv4 gateway of this device's wifi station interface.|



#### Example
    var  cmd = { COMMAND:  "GETNETWORKINFO"};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("SSID="+data.WifiSSID);
		).catch(function() {
		    console.log("Error Executing Command");
	    });;


### GETNETWORKS
This command initiates a wireless network scan and returns the list of networks that were found. It is used to support the "Change Network" & provisioning features in the WebUI

#### Parameters
None

#### Response
The response object holds a list of network objects for the networks found during the scan.
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|
|Networks|Object Array | Each network found is represented by an item in this array property. |


Each network object has these properties:-

|Property|Data Type|Description|
|--|--|--|
|WifiSSID|String |The SSID of the wireless network. |
|WifiChannel|Integer | The 2.4ghz channel on which this is communicating. |
|WifiAuthMode|Integer | The wireless authentication mode this network uses. |
|WifiApMacAddr| String | The MAC address of the access point.|
|WifiSignalStrength| Decimal | The signal strength of the network in dBm.|




#### Example
    var  cmd = { COMMAND:  "GETNETWORKS"};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
		    for(var network in data.Networks) 
				console.log("SSID="+data.Networks[network].WifiSsid);
		).catch(function() {
		    console.log("Error Executing Command");
	    });;

### SELECTNETWORK
This command is used to configure to which wireless network this device connects. There are three different modes:-

 1. **Test** - This mode temporarily attempts to connect to the network using the provided credentials. If successful or after it times out it reconnects to the network it was connected to. This allows the users credentials to be tested before application. *Note: Since this causes the network to disconnect if issued over HTTP it will not return a response.*
 2. **Apply** - This mode connects to the network using the provided credentials and saves the configuration. In correct parameters will cause the device to be unreachable. *Note: Since this causes the network to disconnect if issued over HTTP it will not return a response.*
 3. **Status** - This mode is used to check the status of the device after issuing either of the previous two commands. 

#### Parameters
|Property|Data Type|Description|
|--|--|--|
|Mode|String | The mode valid values are Status, Test, Apply |
|Ssid|String| In Test & Apply mode the SSID of the network to which to connect. |
|Password|String | In Test & Apply mode the password to use when connecting to the network. |
|Id|Integer | In Test & Apply mode a random number to represent the request.|

#### Response
The response object holds a list of network objects for the networks found during the scan.
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|
|Testing|Boolean | In Status mode only - A Wifi network connection test is in progress. This is obviously "difficult" to actually catch over HTTP! |
|TestSuccess|Boolean|In Status mode only - The network connection test was successful if true. |
|Applying|Boolean|In Status mode only - A Wifi network connection change is in progress. This is obviously "difficult" to actually catch over HTTP!|

Each network object has these properties:-

|Property|Data Type|Description|
|--|--|--|
|WifiSSID|String |The SSID of the wireless network. |
|WifiChannel|Integer | The 2.4ghz channel on which this is communicating. |
|WifiAuthMode|Integer | The wireless authentication mode this network uses. |
|WifiApMacAddr| String | The MAC address of the access point.|
|WifiSignalStrength| Decimal | The signal strength of the network in dBm.|




#### Example - Test Mode
    var  cmd = { 
	    COMMAND:  "SELECTNETWORK",
		Mode: "Test",
		Ssid: "MyNetworkSSID",
		Password: "My WPA/WPA2 password".
		Id = Math.floor(Math.random() * 1000000)
	};
	
    
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
		   
		).catch(function() {
		    console.log("Error Executing Command");
	    });;

#### Example - Apply Mode
    var  cmd = { 
	    COMMAND:  "SELECTNETWORK",
		Mode: "Apply",
		Ssid: "MyNetworkSSID",
		Password: "My WPA/WPA2 password".
		Id = Math.floor(Math.random() * 1000000)
	};
    
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
		   
		).catch(function() {
		    console.log("Error Executing Command");
	    });;

#### Example - Status Mode
    var  cmd = { 
	    COMMAND:  "SELECTNETWORK",
		Mode: "Status",
	};
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
		   console.log("Testing "+data.Testing+", Success "+data.TestSuccess+" ", Applying "+data.Applying);
		).catch(function() {
		    console.log("Error Executing Command");
	    });

## CO2 Calibration

### GETLATESTCALIB
This command gets the latest information regarding the calibration status of the CO2 sensor within this device.

#### Parameters
None

#### Response
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|
|ABCEnabled|Boolean |True if the sensors ABC feature is enabled. |
|MinBaseline|Integer | The minimum baseline CO2 value supported by the sensor in PPM (e.g. background/outside level). |
|MaxBaseline|Integer | The maximum baseline CO2 value supported by the sensor in PPM (e.g. background/outside level). |
|Baseline| Integer | The current baseline CO2 value used by the sensor in PPM. |
|CalibWaitTime| Integer | The amount time to wait in seconds in ideal background CO2 levels before invoking the manual background calibration function. |
|MinABCDaysPerCycle| Integer | The minimum ABC cycle length in days. |
|MaxABCDaysPerCycle| Integer | The maximuim ABC cycle length in days.  |
|DaysPerABCCycle| Integer | The current number of days per ABC cycle. |



#### Example
    var  cmd = { COMMAND:  "GETLATESTCALIB"};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("IsAbcEnable="+data.ABCEnabled);
		).catch(function() {
		    console.log("Error Executing Command");
	    });

### MANUALCALIB
This command forces the CO2 sensor to initiate it's background calibration feature. Before issuing this command the sensor must have been placed outside for the amount of seconds dictated by the **CalibWaitTime** property of the **GETLATESTCALIB** command response whilst being fully powered. 


#### Parameters
None

#### Response
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|




#### Example
    var  cmd = { COMMAND:  "MANUALCALIB"};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("Status="+data.Status);
		).catch(function() {
		    console.log("Error Executing Command");
	    });

### ENABLEABC
This command enables the ABC feature within the CO2 sensor if it was previously disabled otherwise it also allows the baseline value to be  updated.

It is generally recommended to keep the ABC function in most CO2 sensors enabled.

#### Parameters
|Property|Data Type|Description|
|--|--|--|
|Baseline|Integer | The baseline (background) CO2 level in PPM for the sensor to use. This value must be in the range dictated by the **MinBaseline** and **MaxBaseline** properties of the **GETLATESTCALIB** command response  |
|ABCDaysPerCycle|Integer| The baseline (background) CO2 level in PPM for the sensor to use. This value must be in the range dictated by the **MinABCDaysPerCycle** and **MaxABCDaysPerCycle** properties of the **GETLATESTCALIB** command response  |

#### Response
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|




#### Example
    var  cmd = { 
	    COMMAND:  "ENABLEABC",
	    Baseline: 400,
	    ABCDaysPerCycle: 7
	};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("Status="+data.Status);
		).catch(function() {
		    console.log("Error Executing Command");
	    });

### DISABLEABC
This command disables the ABC feature within the CO2 sensor.

It is generally recommended to keep the ABC function in most CO2 sensors enabled.

#### Parameters
None

#### Response
|Property|Data Type|Description|
|--|--|--|
|Status |Boolean  |True if command succeeded.  |
|Error |String  | If Status = false then this property **may** be provided to described the error.|




#### Example
    var  cmd = { 
	    COMMAND:  "DISABLEABC",	   
	};
 
    const  requestOptions = {
	    method:  'POST',
	    headers: { 'Content-Type':  'application/json;charset=UTF-8' },
	    body:  JSON.stringify(cmd)
    };
    
    fetch('command', requestOptions)
	    .then(response  => {return  response.json();})
	    .then(data  =>  {
			console.log("Status="+data.Status);
		).catch(function() {
		    console.log("Error Executing Command");
	    });