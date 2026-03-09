# DIFF


## Content
* [Content](DIFF.md#content)
* [Types](DIFF.md#free-functions)
    * [Diff](DIFF.md#Diff)
* [Implementation notes](DIFF.md#implementation-notes)



___________________________________________________
## Free Functions


### Diff

Computes the absolute difference between a and b and returns it

###### Overloads

<dl class="DiffOverloads">
    <dt>Diff(uint64_t a, uint64_t b)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] uint64_t Diff(uint64_t a, uint64_t b) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>a</code> – param1</li>
                <li><code>b</code> – param2</li>
            </ul>
        </dd>
    </dt>
     <dt>Diff(uint32_t a, uint32_t b)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] uint32_t Diff(uint32_t a, uint32_t b) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>a</code> – param1</li>
                <li><code>b</code> – param2</li>
            </ul>
        </dd>
    </dt>
     <dt>Diff(uint16_t a, uint16_t b)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] uint16_t Diff(uint16_t a, uint16_t b) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>a</code> – param1</li>
                <li><code>b</code> – param2</li>
            </ul>
        </dd>
    </dt>
     <dt>Diff(uint8_t a, uint8_t b)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] uint8_t Diff(uint8_t a, uint8_t b) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>a</code> – param1</li>
                <li><code>b</code> – param2</li>
            </ul>
        </dd>
    </dt>
    <dt>Diff(const std::bitset<64>& a, const std::bitset<64>& b)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] std::bitset<64> Diff(const std::bitset<64>& a, const std::bitset<64>& b) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>a</code> – param1</li>
                <li><code>b</code> – param2</li>
            </ul>
        </dd>
    </dt>
    <dt>Diff(const std::bitset<32>& a, const std::bitset<32>& b)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] std::bitset<32> Diff(const std::bitset<32>& a, const std::bitset<32>& b) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>a</code> – param1</li>
                <li><code>b</code> – param2</li>
            </ul>
        </dd>
    </dt>
    <dt>Diff(const std::bitset<16>& a, const std::bitset<16>& b)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] std::bitset<16> Diff(const std::bitset<16>& a, const std::bitset<16>& b) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>a</code> – param1</li>
                <li><code>b</code> – param2</li>
            </ul>
        </dd>
    </dt>
    <dt>Diff(const std::bitset<8>& a, const std::bitset<8>& b)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] std::bitset<8> Diff(const std::bitset<8>& a, const std::bitset<8>& b) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>a</code> – param1</li>
                <li><code>b</code> – param2</li>
            </ul>
        </dd>
    </dt>
</dl>


__________________________________________________

## Implementation Notes

The overloads that work on uintX_t should be preferred since they run faster than the ones that operate on bitsets



[Header File](https://github.com/Leo0806-studios/X86_64_EMU_SOFT/blob/master/X86_64_EMU_SOFT/HEADER/X86_64_EMU_SOFT/HELPERS/DIFF.h)  
[Source File]() \(none present\)

