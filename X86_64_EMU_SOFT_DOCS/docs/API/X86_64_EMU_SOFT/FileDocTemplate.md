# CMD_PARSE


## Content
* [Content]()
* [Types]()
    * [CmdArgs]()
* [Implementation notes]()



___________________________________________________
## Types


### CmdArgs

A uncopyable, unmovable type that represents Parsed Command line arguments  
performs optional verification on parsed arguments to ensure no invalid combination is passed

###### Members

<dl class="CmdArgsAPI">
    <dt>Constructor</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] explicit CmdArgs(int argc, const char** argv);</code></p>
            <p>Parses the passed arguments and stores them internally</p>
            <strong>Parameters</strong>
            <ul >
                <li><code>argc</code> – Argument count</li>
                <li><code>argv</code> – Array of pointers to arguments</li>
            </ul>
        </dd>
    <dt>Validate</dt>
        <dd>
            <p>Signature: <code>[[nodiscard]] bool Validate() const noexcept;</code></p>
            <p>
                Validates the Parsed arguments.<br>
            </p>
                <strong>Return</strong> 
                <ul>
                    <li>true on success</li>
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

