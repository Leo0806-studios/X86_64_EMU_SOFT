# MEMORY


## Content
* [Content](MEMORY.md#content)
* [Types](MEMORY.md#types)
    * [MemoryBus](MEMORY.md#memorybus)
    * [MemoryBus::PageEntry](MEMORY.md#memorybuspageentry)
    * [MemoryBus::PageEntry::PageSection](MEMORY.md#memorybuspageentrypagesection)
* [Implementation notes](MEMORY.md#implementation-notes)



___________________________________________________
## Types


### MemoryBus

represents the Memory bus of the emulator
all MMIO devices have to be registered here and all memory access goes trough it

###### Members

<dl class="MemoryBusAPI">
    <dt>RegisterIODevice(std::shared_ptr<IO_DEVICEs::DeviceBase> device, uint64_t sizeBytes, uint64_t preferedBaseAdress)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] bool RegisterIODevice(std::shared_ptr<IO_DEVICEs::DeviceBase> device, uint64_t sizeBytes, uint64_t preferedBaseAdress);</code></p>
            <p>Registers a device to the Memory Bus and stores the provided information<br>
            if it can be mapped at the preferred base adress it will shift up as much as neccesary to fit.
            returns false on failure
            </p>
            <strong>Parameters</strong>
            <ul >
                <li><code>device</code> – Shared pointer to the device to be registered </li>
                <li><code>sizeBytes</code> – the size in bytes that the device requests to be mapped </li>
                <li><code>preferedBaseAdress</code> – the prefered base adress of the device.</li>
            </ul>
            <strong>Return</strong>
            <ul>
                <li>true on success</li>
            </ul>
        </dd>
    <dt>BuildPageTable()</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] bool BuildPageTable() noexcept;</code></p>
            <p>
                Builds the PageTable from all registered devices.<br>
            </p>
            <strong>Return</strong> 
            <ul>
                <li>true on success</li>
            </ul>
        </dd>
    <dt>UnregisterIODevice(std::shared_ptr<IO_DEVICEs::DeviceBase> device)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] bool UnregisterIODevice(std::shared_ptr<IO_DEVICEs::DeviceBase> device) noexcept</code></p>
            <p> unregisters a device from the memory bus. un-maps it from all pages and leaves open bus in its place.<br> warning! potentialy very slow</p>
            <strong>Parameters</strong>
            <ul>
                <li><code>device</code> - The device to un-map</li>
            </ul>
            <strong>Return</strong>
            <ul>
                <li>true on success</li>
            </ul>
        </dd>
</dl>

###### Private Members
<dl class ="MemoryBusAPIPrivate">
    <dt> MemoryPages</dt>
        <dd>
            <p>Signature: <code>std::vector<PageEntry> MemoryPages</code></p>
            <p>internal storage of the Memory Pages</p>
        </dd>
    <dt>RegisteredDevices</dt>
        <dd>
            <p>Signature: <code>std::vector<DeviceInfos> RegisteredDevices</code></p>
            <p>internal storage of all registered devices</p>
        </dd>
</dl>

### MemoryBus::PageEntry

###### Members
<dl class = "MemoryBusPageEntryAPI">
</dl>

### MemoryBus::PageEntry::PageSection
__________________________________________________

## Implementation Notes




[Header File]()  
[Source File]()

