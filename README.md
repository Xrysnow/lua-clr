# lua-clr

Use C# library in Lua.

The `clr_helper` encapsulates unmanaged APIs to hosting CLR. It can get information from C# library and run codes in it. Lua scripts wrap `clr_helper` into easy-to-use classes and functions.

## Requirements

* LuaJIT 2.0+ (for FFI)
* Windows 7+
* .NET Framework 4+

## Example

```lua
local scope = require('helper').scope
scope('mscorlib.dll')(function()
    using(System)

    Console.WriteLine(Math.PI)
    Console.WriteLine(Math.Sqrt(2))
end)
```

Use thirdparty library:

```lua
scope('mscorlib.dll', 'Interop.MLApp.dll')(function()
    using(System)

    local app = MLApp.MLAppClass()
    Console.WriteLine(app.Execute([[dsolve('Dy=y^2-y')]]))
end)
```
