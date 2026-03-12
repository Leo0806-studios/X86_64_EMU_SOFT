# GENERAL_PROTECTION_FAULT


## Content
* [Content](GENERAL_PROTECTION_FAULT.md#content)
* [Types](GENERAL_PROTECTION_FAULT.md#types)
    * [GENERAL_PROTECTION_FAULT](GENERAL_PROTECTION_FAULT.md#general_protection_fault-1)
* [Implementation notes](GENERAL_PROTECTION_FAULT.md#implementation-notes)



___________________________________________________
## Types


### GENERAL_PROTECTION_FAULT

a type that represents the General Protection fault cpu exception

###### Members

<dl class="GENERAL_PROTECTION_FAULTAPI">
    <dt>GENERAL_PROTECTION_FAULT(const std::string& msg)</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]]explicit GENERAL_PROTECTION_FAULT(const std::string& msg);</code></p>
            <p>Constructs a General protection fault and stores a message </p>
            <strong>Parameters</strong>
            <ul >
                <li><code>msg</code> – stores information about where and how the fault occured</li>
            </ul>
        </dd>
    <dt>GetMessage()</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] const std::string& GetMessage() const noexcept;</code></p>
            <p>
                getter for the stored exception message  .<br>
            </p>
                <strong>Return</strong> 
                <ul>
                    <li>const & to the stored message</li>
                </ul>
        </dd>
    <dt>GetArgMap</dt>
        <dd>
            <p>gets a const reference to the internal storage</p>
        </dd>
</dl>

###### Private Members
<dl class ="CmdArgsAPIPrivate">
    dt
</dl>
__________________________________________________

## Implementation Notes




[Header File]()  
[Source File]()

