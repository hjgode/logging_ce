========================================================================
    WIN32 APPLICATION : keyLoggerCE Project Overview
========================================================================

+ logs hardware and simulated hardware key input.
+ logs WM_KEYDOWN only
- unable to log software keyboard input!

Just copy to device and launch. Use a link inside \StartUp to start on every
boot.

You will see a notification icon on Home/Today screen. If icon is tapped, 
you have the option to exit the logger.

Creates a log file \keylog.txt. Every capture is logged immediately, no 
buffer is used.

If log file size exceeds 1MB, it is renamed to .bak and a new log file is
started. So you have a max of two, recent, log files.

Keys are logged with there char or functional name or as hex if undefined.

The shifted chars may not match your keyboard layout as the logger only
supports the US keyboard layout.

The last known foreground window will be logged too.

DEBUG version will create "\keyloggerce.log.txt" additionally

-----------------------------------------------------------------------------
Sample log:
-----------------------------------------------------------------------------

{24.10.2012 13:27:38}
###### KeyLoggerCE v0.0.1 started...

Active Window: 'Enter Passcode'
0x99 (orange plane)12340x99 (orange plane)
{24.10.2012 13:37:18}
###### KeyLoggerCE v0.0.1 started...

Active Window: 'Notes'
10110[RETURN]
[CONTROL]j\[SHIFT]C1[SHIFT]W06[SHIFT]A0150660538040[RETURN]
[CONTROL]j[SHIFT]CODE39[RETURN]
[CONTROL]j[SHIFT]INTERME[SHIFT]C [SHIFT]TECHNOL[SHIFT]OGIES [SHIFT]CORP.[RETURN]
[CONTROL]j24.7[RETURN]
[CONTROL]j[SHIFT]VISTA-[SHIFT]RES-7.4[SHIFT]MIL[RETURN]
[CONTROL]j[SHIFT]VISTA-[SHIFT]RES-7.4[SHIFT]MIL[RETURN]
[CONTROL]j19.7[SHIFT]MIL[RETURN]
[CONTROL]j24.7[RETURN]
[CONTROL]j[SHIFT]VISTA-[SHIFT]RES-7.4[SHIFT]MIL[RETURN]
[CONTROL]j19.7[SHIFT]MIL[RETURN]
[CONTROL]j[SHIFT]VISTA-[SHIFT]RES-7.4[SHIFT]MIL[RETURN]
[CONTROL]j19.7[SHIFT]MIL[RETURN]
[CONTROL]j24.7[RETURN]
[CONTROL]j[SHIFT]INTERME[SHIFT]C [SHIFT]TECHNOL[SHIFT]OGIES [SHIFT]CORP.[RETURN]
[CONTROL]j\[SHIFT]C1[SHIFT]W06[SHIFT]A0150660538040[RETURN]
[CONTROL]j43256815[RETURN]
[CONTROL]j1234567890128[RETURN]
[CONTROL]j0682345876511[RETURN]
[CONTROL]j\[SHIFT]C1[SHIFT]W06[SHIFT]A0150660538040[RETURN]
[CONTROL]jwww.intermec.com[RETURN]
[CONTROL]j0123456789abcdefghijklmnopqrstuvwxyz[SHIFT]ABCDEFG[SHIFT]HIJKLMN[SHIFT]OPQRSTU[SHIFT]WXYZ[RETURN]
[CONTROL]j[SHIFT]PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348...[RETURN]
[CONTROL]j[RETURN]
[CONTROL]jmailto[SHIFT];somebody[SHIFT][0x40]intermec.com[SHIFT]/Subject=[SHIFT]BARCODE[SHIFT]-RECOGN[SHIFT]ITION[RETURN]
[CONTROL]j[RETURN]
[CONTROL]j[SHIFT]We now come to the decisive step ofmathematical abstraction[SHIFT]; [SHIFT]We forget about what the symbols stand for ...[RETURN]
[CONTROL]j[RETURN]
[CONTROL]j[TAB][TAB][TAB][SHIFT]Hermann [SHIFT]Weyl, [SHIFT]The [SHIFT]Mathematical [SHIFT]Way of [SHIFT]Thinking[SHIFT]...[RETURN]
[CONTROL]j#### keylogger ended. {24.10.2012 13:39:21}

another example log:

{04.02.2014 16:43:57}
###### KeyLoggerCE v0.0.1 started...

Active Window: 'Notes'
hello world[RETURN]
123[RETURN]
[F3]
Active Window: 'Phone '
[F4]
/////////////////////////////////////////////////////////////////////////////