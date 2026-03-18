# 1.2.2 Devices

Devices are passed to the Emulator using Comand line Strings  
the strings have two allowed formats

## 1.2.2.1 Format 1  
"path/to/a/file.deviceInfo"  
* the path seperators have to match the ones used on the host OS.  
the syntax inside the .deviceInfo file is identical to Format 2

## 1.2.2.2 Format 2  


    DeviceName,DeviceType,HandlerBinary,DeviceArg1,DeviceArg2,DeviceArg3,DeviceArg4,DeviceArg5,DeviceArg6  


### 1.2.2.2.1 DeviceName
the name of the Device. Must be unique and ASCII, is not allowed to contain commas

### 1.2.2.2.2 DeviceType

the type of device described.
the type of device does not enforce the capabilities. its just there to guide internal optimisations and provide organisation for the user

all supported types are listed in the following table

| Value | Description|
|-------|------------|
|MEMORY | Main memory. only one of that type is allowed to exist|
|USB    | a generic usb device|
|STORAGE| a generic Block Device|
|GPU    | a graphics processing unit|
|AUDIO  | a generic audio output or rendering device|
|CRYPT  | a generic device that provides cryptographic operations. can include TPM or external Hardware entrophy|
|NETWORK| a generic Network adapter. can be a ethernet or wifi|
|RESET_BYTES| represents a small reset vector rom that gets mapped to the reset vector|
|FIRMWARE| represents a medium sized rom that contains the firmware|
|CUSTOM| a compleatly custom device|

multiple of each type are allowed except for the MEMORY Device  
FIRMWARE and RESET_BYTES are special
they are guaranteed to be mapped to their prefered base adress as long as they dont overlapp (Wich causes a abort)
### 1.2.2.2.3 HandlerBinary

a absolute path to the binary that contains the implementation of the passed device or "buildin" if the inbuild version should be used  
example:

    GPU0,GPU,buildin,pci,NULL,NULL,NULL,NULL,NULL
    GPU0,GPU,C:\User\user\NvidiaPastrough.dll,pci,NULL,NULL,NULL,NULL,NULL 

### 1.2.2.2.4 DeviceArg1 - DeviceArg6

the Device arguments contain information about the device being passed  
the following table contains all possible meanings  
empty cells are not yet determined

|Argument  |GPU   |USB   |MEMORY    |STORAGE              |AUDIO|CRYPT|NETWORK|CUSTOM|RESET_BYTES|FIRMWARE  |
|----------|------|------|----------|---------------------|-----|-----|-------|------|-----------|----------|
|DeviceArg1|BusID |BusID|BusID      |BusID                |BusID|BusID|BusID  |BusID |BusID      |BusID     |
|DeviceArg2|      |      |MaxRam(KB)|Boot Priority (uint8)|     |     |       |      |File       |File      |
|DeviceArg3|      |      |          |StorageFile          |     |     |       |      |           |BaseAdress|
|DeviceArg4|
|DeviceArg5|
|DeviceArg6|

for unused arguments "NULL" has to be passed  
example:

    GPU0,GPU,buildin,pci,NULL,NULL,NULL,NULL,NULL




