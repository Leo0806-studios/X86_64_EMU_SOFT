# SET_BIT


## Content
* [Content](SET_BIT.md#content)
* [Types](SET_BIT.md#free-functions)
    * [SetBit](SET_BIT.md#setbit)
* [Implementation notes](SET_BIT.md#implementation-notes)



___________________________________________________
## Free Functions


### SetBit

Sets a single bit in the provided value and returns it

###### Overloads

<dl class="GetBitOverloads">
    <dt>SetBit(uint64_t value, uint8_t bit, bool set)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]]inline uint64_t  SetBit(uint64_t value, uint8_t bit, bool set) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>value</code> – where to update the bit</li>
                <li><code>bit</code> – the index of the bit to update</li>
                <li><code>set</code> - the new state of the bit</li>
            </ul>
        </dd>
    </dt>
     <dt>SetBit(uint32_t value, uint8_t bit, bool set)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] inline uint32_t SetBit(uint32_t value, uint8_t bit, bool set) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>value</code> – where to update the bit</li>
                <li><code>bit</code> – the index of the bit to update</li>
                <li><code>set</code> - the new state of the bit</li>
            </ul>
        </dd>
    </dt>
     <dt>SetBit(uint16_t value, uint8_t bit, bool set)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] inline uint16_t SetBit(uint16_t value, uint8_t bit, bool set) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>value</code> – where to update the bit</li>
                <li><code>bit</code> – the index of the bit to update</li>
                <li><code>set</code> - the new state of the bit</li>
            </ul>
        </dd>
    </dt>
     <dt>SetBit(uint8_t value, uint8_t bit, bool set)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] inline uint8_t SetBit(uint8_t value, uint8_t bit, bool set) noexcept;</code></p>
            <strong>Parameters</strong>
            <ul >
                <li><code>value</code> – where to update the bit</li>
                <li><code>bit</code> – the index of the bit to update</li>
                <li><code>set</code> - the new state of the bit</li>
            </ul>
        </dd>
    </dt>
</dl>


__________________________________________________

## Implementation Notes

it shifts <code>value</code> right by <code>bit</code>  


[Header File](https://github.com/Leo0806-studios/X86_64_EMU_SOFT/blob/master/X86_64_EMU_SOFT/HEADER/X86_64_EMU_SOFT/HELPERS/SET_BIT.h)  
[Source File]() \(none present\)

