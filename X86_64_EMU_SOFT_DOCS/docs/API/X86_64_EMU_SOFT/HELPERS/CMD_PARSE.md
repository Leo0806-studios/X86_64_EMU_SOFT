# CMD_PARSE


## Content
* [Content](CMD_PARSE.md#content)
* [Types](CMD_PARSE.md#types)
    * [CmdArgs](CMD_PARSE.md#cmdargs)
* [Implementation notes](CMD_PARSE.md#implementation-notes)



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
            <p>Signature: <code>const boost::program_options::variables_map& GetArgMap() const noexcept;</code></p>
            <p>gets a const reference to the internal storage</p>
        </dd>
</dl>

###### Private Members
<dl class ="CmdArgsAPIPrivate">
    <dt>ArgMap</dt>
        <dd>
            <p>Signature: <code>boost::program_options::variables_map ArgMap;</code></p>
            <p>Internal storage of the parsed Arguments</p>
        </dd>
</dl>
__________________________________________________

## Implementation Notes

uses boost::program_options internally to handle parsing


[Header File](https://github.com/Leo0806-studios/X86_64_EMU_SOFT/blob/master/X86_64_EMU_SOFT/HEADER/X86_64_EMU_SOFT/HELPERS/CMD_PARSE.h)  
[Source File](https://github.com/Leo0806-studios/X86_64_EMU_SOFT/blob/master/X86_64_EMU_SOFT/SOURCE/HELPERS/CMD_PARSE.cpp)

