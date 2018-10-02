# IDBG
Allows you to add breakpoints from IDA (from the graph/text view) to WinDbg easily.<br />

Since I found the debugger inside IDA confusing(probably because I couldn't use it properly) I decided to write a plugin that allowed me to add plugins from the IDA graph view to WinDbg.

How to load:<br />
On IDA: File, Script File (or just ALT F7) then browse to idbg.py <br />
  On WinDbg: !load path  <br />
             !sync_with module (module without the .dll at the end) <br />
How to unload: <br />
  On WinDbg: !unload_idbg <br />
             !unload path <br />
  On IDA: <br />
             just click somewhere on the graph so OnViewCurpos gets called <br />
(path is the path of the WinDbg DLL extension)  <br />


How to use: load idbg on IDA and windbg-extension.dll on WinDbg then you simply have to press the key j + left click wherever you want to add a breakpoint to add a breakpoint and doing the same will also remove it.<br />
To change the key j to whatever key you want, you have to modify the parameter of the is_key_down function with the vkey code of the key you desire to use instead. <br />

Credits: <br />
    https://github.com/geohot/qira/blob/master/ida/python/qira.py (took the hooks from there) <br />
    dbgsdk samples from Microsoft and wdbgark 
