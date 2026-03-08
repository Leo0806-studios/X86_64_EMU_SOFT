# Device Handlers

## Definition:

 A device handler is a binary that implements a custom Device inhereting from X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::DeviceBase.  
 Furthermore it has to export a function with C Linkage that creates the device and returns a std::shared_ptr<X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::DeviceBase\> 
    