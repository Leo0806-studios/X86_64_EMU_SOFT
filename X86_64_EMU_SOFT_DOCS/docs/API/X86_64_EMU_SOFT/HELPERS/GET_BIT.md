# GET_BIT


## Content
* [Content](GET_BIT.md#content)
* [Types](GET_BIT.md#free-functions)
    * [GetBit](GET_BIT.md#getbit)
* [Implementation notes](GET_BIT.md#implementation-notes)



___________________________________________________
## Free Functions


### GetBit

extracts a single bit and returns it

###### Overloads

<dl class="GetBitOverloads">
    <dt>GetBit(uint64_t value, uint8_t bit)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] inline bool GetBit(uint64_t value, uint8_t bit) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>value</code> – where to extract the bit from</li>
                <li><code>bit</code> – the index of the bit to extract</li>
            </ul>
        </dd>
    </dt>
     <dt>GetBit(uint32_t value, uint8_t bit)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] inline bool GetBit(uint32_t value, uint8_t bit) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>value</code> – where to extract the bit from</li>
                <li><code>bit</code> – the index of the bit to extract</li>
            </ul>
        </dd>
    </dt>
     <dt>GetBit(uint16_t value, uint8_t bit)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] inline bool GetBit(uint16_t value, uint8_t bit) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>value</code> – where to extract the bit from</li>
                <li><code>bit</code> – the index of the bit to extract</li>
            </ul>
        </dd>
    </dt>
     <dt>GetBit(uint8_t value, uint8_t bit)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] inline bool GetBit(uint8_t value, uint8_t bit) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>value</code> – where to extract the bit from</li>
                <li><code>bit</code> – the index of the bit to extract</li>
            </ul>
        </dd>
    </dt>
</dl>


__________________________________________________

## Implementation Notes

it shifts <code>value</code> right by <code>bit</code>  


[Header File](https://github.com/Leo0806-studios/X86_64_EMU_SOFT/blob/master/X86_64_EMU_SOFT/HEADER/X86_64_EMU_SOFT/HELPERS/GET_BIT.h)  
[Source File]() \(none present\)

