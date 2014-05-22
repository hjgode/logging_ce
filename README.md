logging_ce
==========

a collection of logging tools for windows ce or windows mobile

batterylog

*  log battery data periodically. Import to spreadsheet and make nice graphs from the log files

memuse

*  logs the memory use of running processes to a comma delimited text file
*  maybe you are able to find a memory leaking process
  
btmon

*  logs bluetooth events to file
*  useful if you have BT issues

connMgrLog
*  log connMgrMessages
*  currently the messages are shown on-screen only
*  TODO: log messages to file
  
showFullScreen
*  list all open windows on a running system
*  can change window size etc.
	Arguments:                default          meaning
	-class                    ""               class name of window to change, optional
	-title                    ""               title of window to change, optional
				at least -class or -title must be used to change a window

	-fullscreen               false            make window fullscreen
	-maximized                false            make window normal size

	-show                     no change        make window visible
	-hide                     no change        make window invisible

	-enable                   no change        enable window
	-disable                  no change        disable window

	-list                    -                 list windows into file

keyLoggerCE
*  logs hardware keyboard strokes (no software input panel inputs!) but with record of window and special keys.
  
