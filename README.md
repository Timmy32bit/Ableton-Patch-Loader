Ableton Live Patch loader v1.0

By UwUCaptain aka Timmy32bit aka Timmy
================================

By default this patch loader comes bundled with a simple patch called "SimpleStartup"
This plugin does the following...

* Disables Ableton usage telemtry
* Disables plugin scanning (so no more annoying scanning on startup)
* Disables writing the usage log (temp file)

This plugin was only tested on Ableton Live 11, so while I doubt there'd be issues for Live 12, I haven't confirmed

To manage your patches, they are located in the folder "patches"
Every module located in this folder will get loaded at startup

To use this patch loader both "x.dll and "REX Shared Library.dll" must be installed in the program directory.
the "REX Shared Library.dll" module in the directory must be replaced with mine
Absolutely **ZERO** of the functionality of the original is lost
so there is no need to remove it to disable the patch loader, deleting or even renaming "x.dll" will suffice.
to load patches, make a directory called "patches" and insert any DLL's you'd like loaded at startup.

**...Or extract the contents from the latest git release into the Ableton installation directory**

**For example, "C:\ProgramData\Ableton\Live 11 Suite\Program"**

FAQ:
---
- **Q: How does this work?**
- *A: The tldr is that I patched "REX Shared Library.dll" so that the (literally useless) entry loads "x.dll", which loads the patches*

- **Q: Is this safe?**
- *A: Of course. Virustotal might flag some things.*
	*But the short version is that virustotal flags literally ANYTHING that could even be used in a malicious way.*
	*Poll the keyboard? Well virustotal says "keylogger".*
	*Anyway both of these binaries are not obfuscated in anyway, if someone wants to reverse them be my guest.*

- **Q: What can this be used for?**
- *A: You can use this so that you or someone else could write patches for ableton (or plugins "wink wink") and load them easily.*

- **Q: Will all patches be compatible with eachother?**
- *A: Fuck no. If 2 things attempt to patch or hook the same thing then that is a recipe for some fucky wucky-ness. Do not make it my problem*

---

Thank you for reading this!
