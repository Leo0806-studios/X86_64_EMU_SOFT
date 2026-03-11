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
    <dt> MapMainMemory(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t Base)</dt>
        <dd>
        <p>Signature: <code>[[nodiscard]] bool MapMainMemory(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t Base);</code></p>
        <p>Registers the Main Memory device and maps all pages to it.</p>
        <strong>Parameters</strong>
            <ul>
            <li></li>
            </ul>
        </dd>
    <dt>Read8(uint64_t address)</dt>
        <dd>
        <p>Signature: <code> [[nodiscard]] uint8_t Read8(uint64_t address) const noexcept;</code></p>
        <p>Reads a byte from the specified adress</p>
        <strong>Parameters</strong>
            <ul>
            <li><code> adress </code> - the adress to be read from</li>
            </ul>
        </dd>
    <dt>Read16(uint64_t address)</dt>
        <dd>
        <p>Signature: <code> [[nodiscard]] uint16_t Read8(uint64_t address) const noexcept;</code></p>
        <p>Reads a word from the specified adress</p>
        <p>bytes crossing into different mmio reagions will be read from the coresponding devices</p>
        <strong>Parameters</strong>
            <ul>
            <li><code> adress </code> - the adress to be read from</li>
            </ul>
        </dd>
    <dt>Read32(uint64_t address)</dt>
        <dd>
        <p>Signature: <code> [[nodiscard]] uint32_t Read8(uint64_t address) const noexcept;</code></p>
        <p>Reads a double word from the specified adress</p>
        <p>bytes crossing into different mmio reagions will be read from the coresponding devices</p>
        <strong>Parameters</strong>
            <ul>
            <li><code> adress </code> - the adress to be read from</li>
            </ul>
        </dd>
    <dt>Read64(uint64_t address)</dt>
        <dd>
        <p>Signature: <code> [[nodiscard]] uint64_t Read8(uint64_t address) const noexcept;</code></p>
        <p>Reads a quad word from the specified adress</p>
        <p>bytes crossing into different mmio reagions will be read from the coresponding devices</p>
        <strong>Parameters</strong>
            <ul>
            <li><code> adress </code> - the adress to be read from</li>
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

