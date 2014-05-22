========================================================================
    CONSOLE APPLICATION : showFullScreen Project Overview
========================================================================

Tool to alter foreign windows

/////////////////////////////////////////////////////////////////////////////
Arguments:				default			meaning
-class					""				class name of window to change, optional
-title					""				title of window to change, optional
			at least -class or -title must be used to change a window
										
-fullscreen				false			make window fullscreen
-maximized				false			make window normal size

-show					no change		make window visible
-hide					no change		make window invisible

-enable					no change		enable window
-disable				no change		disable window

-list					-				list windows into file

/////////////////////////////////////////////////////////////////////////////
examples:

Enable IE6 soft menu button:	-class "IE6on6SoftKeyBar" -enable
Disable IE6 soft menu button:	-class "IE6on6SoftKeyBar" -disable

Enable taskbar:		-class "HHTASKBAR" -enable
Disable taskbar:	-class "HHTASKBAR" -disable

make win fullscreen: -class "WFIcaClient" -fullscreen



/////////////////////////////////////////////////////////////////////////////