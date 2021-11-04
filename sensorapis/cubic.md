# Cubic Sensors
This document outlines the knowledge gained during implementing the integration for Cubic sensors using five second hand sensors got cheap on AliExpress & various third party sources. Be warned that many commands can seriously affect the accuracy of your sensor. 

The assist in preventing damage some commands are labelled with 💥 should be used with extra caution.  But please note there are almost certainly errors in this document and there are likely many different versions of firmware, therefore by applying anything contained within this document you must use your own best judgement and accept the full risk of any damage/consequences that may occur.

The purpose of this document it to allow integrators to better  understand Cubic NDIR CO2 sensors.



# Commands

| Risk |Code| Name  | CM1106 | CM1106SL-NS | CM1107 |
|--|--|--|--|--|--|
| | 0x01 | [Read Values](#read-values-0x01)          | ✔ |  ✔ |  ✔ |
|   | 0x02 | [Read Internal Values](#read-internal-values-0x02) | ✔ |    |  ✔ |
| 💥| 0x03 | [Start Calibration](#start-calibration-0x03)    | ✔ | ✔  |  ✔ |
|   | 0x04 | [Read Real Data](#read-real-data-0x04) | ✔ | ✔  |  ✔ |
|   | 0x05 | [Read Real Data 2](#read-real-data-2-0x05)      | ✔ |    |  ✔ |
| 💥| 0x06 | [Set Unknown Mode](#get-set-unknown-mode-0x06)     | ✔ | ✔  |  ✔ |
|   | 0x0D | [Get Sensor Info](#get-sensor-info-0x0D)     | ✔ | ✔  |  ✔ |
| 💥| 0x0E | [Set Sensor Info](#set-sensor-info-0x0E-💥)     | ✔ | ✔  |  ✔ |
|   | 0x0F | [Get ABC](#get-abc-0x0F)     | ✔ | ✔  |  ✔ |
|   | 0x10 | [Set ABC](#set-abc-0x10)     | ✔ | ✔  |  ✔ |
|   | 0x11 | [Save ABC](#save-abc-0x11)     |   |  ✔  |   |
|   | 0x1E | [Get Software Version](#get-software-version-0x1E)     | ✔ | ✔  |  ✔ |
|   | 0x1F | [Get Serial Number](#get-serial-0x1F)     | ✔ | ✔  |  ✔ |
| 💥 | 0x23 | [Set Serial Number](#set-serial-0x23-💥)     | ✔ | ✔  |  ✔ |
|   | 0x50 |  [Get/Set Sensor Measurement Period and Smoothing Data](#getset-sensor-measurement-period-and-smoothing-data-0x50)     |  | ✔  |   |
|   | 0x51 | [Get/Set Sensor Working Status](#getset-sensor-working-status-0x51)     |  | ✔  |  
| 💥| 0x41 | Unknown     | ✔ |   |  ✔ |
| 💥| 0x42 | Unknown     | ✔ |   |  ✔ |
| 💥| 0x43 | Unknown     | ✔ |   |  ✔ |
| 💥| 0x44 | Unknown     | ✔ |   |  ✔ |
| 💥| 0x45 | Unknown    | ✔ |   |  ✔ |
| 💥| 0x46 | Unknown     | ✔ |   |  ✔ |
| 💥| 0x47 | Unknown     | ✔ |   |  ✔ |
| 💥| 0x49 | Unknown     | ✔ |   |  ✔ |
| 💥| 0x49 | Unknown     | ✔ |   |  ✔ |
| 💥| 0x4A | Unknown     | ✔ |   |  ✔ |
| 💥| 0x4B | Possibly Zero Point for User Calibration | ✔ |   |  ✔ |
| 💥| 0x4C | Possibly Full Scale Point for User Calibration | ✔ |   |  ✔ |
| 💥| 0x8C | Unknown |  | ✔ |   |
| 💥| 0xA2 | Unknown |  | ✔ |   |
|   | 0xFA | [Read Sensor Text Status](#read-sensor-text-status-0xfa) | ✔ | ✔  |  ✔ |
|   | 0xFD | [Start/Stop Stream Mode](#startstop-stream-mode-0xfd)  | ✔ | ✔  |  ✔ |
| 💥| 0xFE | [Get/Set Configuration Word](#getset-configuration-word-0xfe)     | ✔ |    |  ✔ |

## Protocol Structure

### Checksum Calculation

The checksum calculation is as follows, it is always performed on all bytes in a message except for obvious reasons the final checksum byte that will be added after calculation:

     256 - (messageBytes.Sum() % 256)


### Commands
All commands issued to the sensor follow the format described bellow.
|Field | Length in Bytes  | Description |
|--|--|--|
| Start Code  | 1 | Always 0x11 |
| Length      | 1 | The number of data bytes plus 1 to include the command byte	
| Command     | 1 | The command to issue |
| Data        | Length - 1 | Parameter data 
| Checksum    | 1  | Checksum calculated as per described algorithm | 



### Responses

All responses to commands issued to the sensor follow either the success or error formats described bellow except in the case of [Read Sensor Text Status](#read-sensor-text-status-0xfa) & [Start/Stop Stream Mode](#start-stop-stream-mode-0xfd).

#### Successful Responses
The structure of successful response packets is as follows:-
|Field | Length in Bytes  | Description |
|--|--|--|
| Start Code  | 1 | Always 0x16 |
| Length      | 1 | The number of data bytes plus 1 to include the command byte	|
| Command     | 1 | The command to which the response is responding |
| Data        | Length - 1 | Response Data 
| Checksum    | 1  | Checksum calculated as per described algorithm | 

#### Error Responses
The structure of error response packets is as follows:-
|Field | Length in Bytes  | Description |
|--|--|--|
| Start Code  | 1 | Always 0x06 |
| Length      | 1 | Always 2 |
| Command     | 1 | The command to which the response is responding |
| Error Code  | 1 | The error code |
| Checksum    | 1  | Checksum calculated as per described algorithm | 

The known error codes are the following:-
| Code| Meaning |
|--|--|
| 0x01 | Checksum Invalid  |
| 0x02 | Unrecognised Command |
| 0x03 | Data Length Incorrect |
| 0x04 | Out Of Range |



## Read Values (0x01)

### Officially Documented 
This command is the documented way to read the current CO2 reading from the sensor.  The documented form of this command is as follows:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x01 |
| Checksum    | 1  | 0xED | 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x05	
| Command     | 1 | 0x01 |
| CO2    | 2  | Little endian ushort | 
| Status    | 1  | A series of status bits |
| Counter    | 1  | A byte which counts up roughly once per second until it rolls over |
| Checksum    | 1  | Calculated as per the checksum algorithm |



### Undocumented Variant
There is an undocumented form of this command which takes an extra a parameter byte as follows:-
 |Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x02 |	
| Command     | 1 | 0x01 |
| Unknown     | 1 | 0x00 is known to work so maybe formally the sensor index |
| Checksum    | 1  | 0xED (Calculated as per the checksum algorithm) |

The response is like the documented version with an additional short value provided.
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x07	
| Command     | 1 | 0x01 |
| CO2    | 2  | Little endian ushort | 
| Unknown    | 2  | Little endian ushort | 
| Status    | 1  | A series of status bits |
| Counter    | 1  | A byte which counts up roughly once per second until it rolls over |
| Checksum    | 1  | Calculated as per the checksum algorithm |

### Status Bits

|Bit | Meaning | Observed In Wild |
|--|--|--|
| 0 | Set once the sensor has heated up. | Yes |
| 1 | Set once the sensor has heated up. | No |
| 2 | Reading is greater than sensor range (Fault?). | No |
| 3 | Reading is less than sensor range (Fault?). | No |
| 4 | Sensor is calibrated. | No |
| 5 | Light Aging | No |
| 6 | Drift | No |
| 7 | Unknown | No |



## Read Internal Values (0x02)
This command is an undocumented method to get raw readings from the sensor.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x02	
| Command     | 1 | 0x02 |
| Unknown     | 1 | 0x00 |
| Checksum    | 1  | 0xEC (Calculated as per the checksum algorithm)| 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x0A	
| Command     | 1 | 0x02 |
| Unknown    | 1   | 0x00 has been observed. | 
| Unknown    | 2   | 0x44, 0x94 has been observed. | 
| Unknown    | 2   | 0x3C, 0x94 has been observed. | 
| tAD (Best Guess)   | 2  | Little endian ushort  | 
| rAD (Best Guess)   | 2  | Little endian ushort | 
| Checksum    | 1  | Calculated as per the checksum algorithm |
## Start Calibration (0x03)

### Officially Documented 
This command is the documented way to read the current CO2 reading from the sensor.  The documented form of this command is as follows:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x03 |
| Ambient CO2 Level |2 | Little endian ushort usually 400 |
| Checksum    | 1  | Calculated as per the checksum algorith |

The response is:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x03 |
| Checksum    | 1  | Calculated as per the checksum algorithm |


### Undocumented Variant 💥
There is an undocumented form of this command which takes an extra a short parameter. This can significantly change your readings I have found values such as 2000 give more reasonable results. But this command variant is **very** risky:-
 |Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x02 |	
| Command     | 1 | 0x03 |
| Ambient CO2 Level |2 | Little endian ushort usually 400 |
| Unknown | 2 | Little endian ushort 2000 gives much better result than 0 😁 |
| Checksum    | 1  | Calculated as per the checksum algorithm |

The response is:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x03 |
| Checksum    | 1  | Calculated as per the checksum algorithm |

## Read Real Data (0x04)
This command is an undocumented method to get CO2, temperature and raw readings.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x04 |
| Checksum    | 1  | 0xEA (Calculated as per the checksum algorithm)| 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x0B	
| Command     | 1 | 0x04 |
| CO2    | 2  | Little endian ushort | 
| Temperature (Best Guess)   | 2  | Little endian ushort in C multiplied by 100 | 
| tAD (Best Guess)   | 2  | Little endian ushort  | 
| rAD (Best Guess)   | 2  | Little endian ushort | 
| TAD (Best Guess)   | 2  | Little endian ushort | 
| Checksum    | 1  | Calculated as per the checksum algorithm |


## Read Real Data 2 (0x05)
This command is an undocumented method to get CO2, temperature and raw readings.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x05 |
| Checksum    | 1  | 0xE9 (Calculated as per the checksum algorithm)| 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x0D	
| Command     | 1 | 0x05 |
| CO2    | 2  | Little endian ushort | 
| Temperature (Best Guess)   | 2  | Little endian ushort in C multiplied by 100 | 
| tAD (Best Guess)   | 2  | Little endian ushort  | 
| rAD (Best Guess)   | 2  | Little endian ushort | 
| TAD (Best Guess)   | 2  | Little endian ushort | 
| Unknown   | 2  | Appears to always be 0x00, 0x00 | 
| Checksum    | 1  | Calculated as per the checksum algorithm |

## Get/Set Unknown Mode (0x06)
This command manages an undocumented numeric mode. It is known to be a mode as [Read Sensor Text Status](#read-sensor-text-status-0xfa) includes text which says as much. It is currently known what this mode changes.
### Get
This command is an undocumented method to get an unknown mode value.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x06 |
| Checksum    | 1  | 0xE8  (Calculated as per the checksum algorithm)| 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x02	
| Command     | 1 | 0x06 |
| Mode    | 1  | 0-255 | 
| Checksum    | 1  | Calculated as per the checksum algorithm |

### Set 💥
This command is an undocumented method to set an unknown mode value.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x02	
| Command     | 1 | 0x06 |
| Mode    | 1  | 0-255 | 
| Checksum    | 1  | Calculated as per the checksum algorithm |

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x02	
| Command     | 1 | 0x06 |
| Mode    | 1  | 0-255 | 
| Checksum    | 1  | Calculated as per the checksum algorithm |

## Get Sensor Info (0x0D)
This command is an undocumented way to get the sensor info such as maximum PPM.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x0D |
| Checksum    | 1  | 0xE1  (Calculated as per the checksum algorithm)| 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x06	
| Command     | 1 | 0x0D |
| Max PPM    | 2  | Little endian ushort. Known values are 2000 and 5000 | 
| Unknown    | 1  | Always 0  |
| Unknown    | 1  | Always 1  |
| Unknown    | 1  | Always 0  |
| Checksum    | 1  | Calculated as per the checksum algorithm |

## Set Sensor Info (0x0E) 💥
This command is an undocumented way to set the sensor info such as maximum PPM.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x06	
| Command     | 1 | 0x0D |
| Max PPM    | 2  | Little endian ushort. Known values are 2000 and 5000 | 
| Unknown    | 1  | Always 0  |
| Unknown    | 1  | Always 1  |
| Unknown    | 1  | Always 0  |
| Checksum    | 1  | Calculated as per the checksum algorithm |

## Get ABC (0x0F)

## Set ABC (0x10)

## Save ABC (0x11)

## Get Software Version (0x1E)
This command is the documented way to read the firmware version number of the sensor.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x1E |
| Checksum    | 1  | 0xD0  (Calculated as per the checksum algorithm)| 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x0C	
| Command     | 1 | 0x1E |
| Version    | 11  | ASCII Version String | 
| Checksum    | 1  | Calculated as per the checksum algorithm |

## Get Serial (0x1F)
This command is the documented way to read the serial number of the sensor.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0x1F |
| Checksum    | 1  | 0xCF  (Calculated as per the checksum algorithm)| 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x0B	
| Command     | 1 | 0x1E |
| Part1    | 2  | Little endian ushort in range 0-9999 | 
| Part2    | 2  | Little endian ushort in range 0-9999 | 
| Part3    | 2  | Little endian ushort in range 0-9999 | 
| Part4    | 2  | Little endian ushort in range 0-9999 | 
| Part5    | 2  | Little endian ushort in range 0-9999 | 
| Checksum    | 1  | Calculated as per the checksum algorithm |


## Set Serial (0x23) 💥
This command is undocumented but has been found to work on all devices examined so far. It allows setting of the same serial number as received from , it has the following syntax:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x0C	
| Command     | 1 | 0x1E |
| Unknown | 1 | 0x00 |
| Part1    | 2  | Little endian ushort in range 0-9999 | 
| Part2    | 2  | Little endian ushort in range 0-9999 | 
| Part3    | 2  | Little endian ushort in range 0-9999 | 
| Part4    | 2  | Little endian ushort in range 0-9999 | 
| Part5    | 2  | Little endian ushort in range 0-9999 | 
| Checksum    | 1  | Calculated as per the checksum algorithm |

## Get/Set Sensor Measurement Period and Smoothing Data (0x50)

## Get/Set Sensor Working Status (0x51)



## Read Sensor Text Status (0xFA)
This command is an undocumented way to get approximately 900 bytes of status text generated by the sensor. Since at 9600 baud this takes nearly 1 second to retrieve this data as such it is not generally speaking practical use frequently. It was most likely included for debug/support purposes.

It is issued in the following form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0xFA |
| Checksum    | 1  | 0xF4  (Calculated as per the checksum algorithm)| 

The command does not respond in the same format as most commands. It solely returns text separated specific character sequences.  As there is no length byte one way to ensure the whole packet has been received is to wait 1 second for data before processing.

The result string is made of three parts which are separated by distinct two byte sequences. The structure is as allows:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Sensor Information  | variable | This text includes read outs of various internal sensor values the exact contents appears to change between sensors. It can sometimes be clear that is a sensor is broken by virtue of multiple NaN/0 entries in this text. |
| Separator #1    | 2 | 0xA1, 0xE6	
| Unknown Text    | variable | The purpose of this status text is unknown |
| Separator #2   | 2  | 0xA1, 0xEB | 
| Calibration/Mode Status Text    | variable  | This text includes information on whether ABC is enabled, how often the sensor has auto zeroed itself. The value set in [Set Unknown Mode](#get-set-unknown-mode-0x06) is also included in this text as the second value labelled mode (The first being whether ABC is enabled or not).  |
| Checksum    | 1  | Calculated as per the checksum algorithm |

### Sensor Information
Here is a sample of sensor information component of returned string:-

    Num 1227-6171-50 CM1107 VER CM V2.0.0 Range 2000ppm  
    TEMP:  
    t_[0]=0,x_[0]=1196,TAD_[0]=9976,tAD_[0]=27611,rAD_[0]=23075  
    t_[1]=53,x_[1]=1238,TAD_[1]=18355,tAD_[1]=28637,rAD_[1]=23128  
    t_[2]=24,x_[2]=1195,TAD_[2]=16675,tAD_[2]=27595,rAD_[2]=23073  
    t_[3]=44,x_[3]=1223,TAD_[3]=8240,tAD_[3]=27820,rAD_[3]=22732  
    t_[4]=23,x_[4]=1196,TAD_[4]=16606,tAD_[4]=27611,rAD_[4]=23076  
    t_[5]=41,x_[5]=1144,TAD_[5]=8347,tAD_[5]=26001,rAD_[5]=22718  
    1 1197  
    1 1159  
    1 1161  
    0 , -1  
    0 , -0  
    GAS  
    x_[0]=1195,y_[0]=550,rAD_[0]=23073,tAD_[0]=27595,TAD_[0]=16675  
    x_[1]=1211,y_[1]=800,rAD_[1]=23028,tAD_[1]=27908,TAD_[1]=14534  
    x_[2]=1195,y_[2]=550,rAD_[2]=23074,tAD_[2]=27594,TAD_[2]=6668  
    x_[3]=1173,y_[3]=1600,rAD_[3]=23016,tAD_[3]=27003,TAD_[3]=14671  
    x_[4]=1157,y_[4]=2000,rAD_[4]=23015,tAD_[4]=26637,TAD_[4]=14662  
    1195 550 550  
    1157 2000 2155  
    0  
    -13  
    13964  
    -5101564  
      
    T 24

### Unknown Text
Here is a sample of unknown text component of returned string:-

    D 32

### Calibration/Mode Status Text
Here is a sample of Calibration/Mode Status component of returned string:-

    AZero  
    MODE 0 (2-OFF) ZeroIncV 0  
    AutoZeroCnt 29635 TCnt 60MODE 1

## Start/Stop Stream Mode (0xFD)
This command is undocumented command that places the sensor into a mode that streams fixed length packets until the command is issued again which stops this process.

It is issued in the following form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x01	
| Command     | 1 | 0xFD |
| Checksum    | 1  | 0xF1  (Calculated as per the checksum algorithm)| 

The response has not yet been analysed in great detail. What is know is that it is fixed length and is continuous until this command is issued a second time.


## Get/Set Configuration Word (0xFE)
This undocumented command that allows direct reading & writing of configuration data within the sensor.  Clearly this is potentially very dangerous but it should also should be noted there has been some level of success in using these commands to "restore" sensors after some tests went badly wrong.

The indexes appear to be direct address references e.g. as two bytes are written or read per command issued the next two bytes can be retrieved by adding 2 to the index. *There is some uncertainty regarding the index*

### Get
This command is an undocumented method to get an a configuration value by index.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x02 |	
| Command     | 1 | 0xFE |
| Index     | 1 | Index to read |
| Checksum    | 1  | Calculated as per the checksum algorithm | 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x06	
| Command     | 1 | 0xFE |
| Index     | 1 | Index to read |
| Unknown     | 1 | 0x00 |
| Unknown     | 1 | 0x00 |
| Byte #1 | 1 | First byte read  |
| Byte #1 | 1 | Second byte read  |
| Checksum    | 1  | Calculated as per the checksum algorithm | 

### Set 💥
This command is an undocumented method to set an a configuration value by index.  It is issued in this form:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x11 |
| Length      | 1 | 0x06	
| Command     | 1 | 0xFE |
| Index     | 1 | Index to read |
| Unknown     | 1 | 0x00 |
| Unknown     | 1 | 0x00 |
| Byte to Write #1 | 1 | First byte to write  |
| Byte to Write #1 | 1 | Second byte to write  |
| Checksum    | 1  | Calculated as per the checksum algorithm | 

It responds with the following:-
|Field | Length in Bytes  | Value |
|--|--|--|
| Start Code  | 1 | 0x16 |
| Length      | 1 | 0x06	
| Command     | 1 | 0xFE |
| Index     | 1 | Index to read |
| Unknown     | 1 | 0x00 |
| Unknown     | 1 | 0x00 |
| Byte #1 | 1 | First byte read  |
| Byte #1 | 1 | Second byte read  |
| Checksum    | 1  | Calculated as per the checksum algorithm | 


# Credits & Sources

