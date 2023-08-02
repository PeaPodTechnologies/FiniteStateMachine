## The Master Header: `<FSM.h>`

Prevents .tpp files from being auto-included at the end of their .h files. Includes them manually afterwards.

## Header Files (`.h`)

Contains: Declarations

No Implementations (avoid dupes)

Includes: All necessary headers (for `.tpp` too!), the associated `.tpp` (if applicable), NOT `<FSM.h>`

## Template Files (`.tpp`)

Contains: Template Implementations

Includes: N/A

Only include once

## Source Files (`.cc`)

Contains: Source Code, Non-Template Implementations

Includes: `<FSM.h>`