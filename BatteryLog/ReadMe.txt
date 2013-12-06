BatteryLog.exe

Version 
	2.10
		changed DumpPowerFlags
Version:
	0.91

Purpose:
	Periodically log battery status. 
	Logging is done into file batterylog.txt in the executable directory and via a boradcast into the network.
	One can use netcat to get the broadcasts. The broadcast udp port used is 9998.

Usage:

	batterylog.exe 1
		starts logging for every second, battery life will be estimated with the default battery capacity of 4200mAh
	batterylog.exe 1 2200
		starts logging for every second, battery life will be estimated with a battery capacity of 2200mAh
	batterylog.exe 1 2200 NOBCAST
		starts logging for every second, battery life will be estimated with a battery capacity of 2200mAh; 
		no broadcast is send over the network
	batterylog.exe kill
		stops logging and application quits
		quitting the application can also be forced with a file called _stop_battery_log.flag in windows directory
		the application will quit after a maximum of the logging interval

	agenda
		ACLineStatus;
			AC_LINE_OFFLINE                 0x00
			AC_LINE_ONLINE                  0x01
			AC_LINE_BACKUP_POWER            0x02
			AC_LINE_UNKNOWN                 0xFF

		BatteryFlag;
			BATTERY_FLAG_HIGH               0x01
			BATTERY_FLAG_LOW                0x02
			BATTERY_FLAG_CRITICAL           0x04
			BATTERY_FLAG_CHARGING           0x08
			BATTERY_FLAG_NO_BATTERY         0x80
			BATTERY_FLAG_UNKNOWN            0xFF
		BatteryLifePercent;
			Percentage of full battery charge remaining.  Must be in the range 0 to 100, or BATTERY_PERCENTAGE_UNKNOWN
		BackupBatteryFlag;
			BATTERY_FLAG_HIGH               0x01
			BATTERY_FLAG_LOW                0x02
			BATTERY_FLAG_CRITICAL           0x04
			BATTERY_FLAG_CHARGING           0x08
			BATTERY_FLAG_NO_BATTERY         0x80
			BATTERY_FLAG_UNKNOWN            0xFF
		BackupBatteryLifePercent;
			Percentage of full battery charge remaining.  Must be in the range 0 to 100, or BATTERY_PERCENTAGE_UNKNOWN

		BATTERY_PERCENTAGE_UNKNOWN      0xFF
		BATTERY_LIFE_UNKNOWN        0xFFFFFFFF

		BatteryVoltage; 				// Reports Reading of battery voltage in millivolts (0..65535 mV)
		BatteryCurrent;					// Reports Instantaneous current drain (mA). 0..32767 for charge, 0 to -32768 for discharge
		BatteryAverageCurrent; 			// Reports short term average of device current drain (mA). 0..32767 for charge, 0 to -32768 for discharge
		BatteryAverageInterval;			// Reports time constant (mS) of integration used in reporting BatteryAverageCurrent	
		BatterymAHourConsumed; 			// Reports long-term cumulative average DISCHARGE (mAH). Reset by charging or changing the batteries. 0 to 32767 mAH  
		BatteryTemperature;				// Reports Battery temp in 0.1 degree C (-3276.8 to 3276.7 degrees C)
		BackupBatteryVoltage;			// Reports Reading of backup battery voltage

	wifi
		current wifi status: 0=off, 1=on, other unknown or unsupported
	phone 	
		current phone status: 0=off, 1=on, other unknown or unsupported
	BT
		current BT status: 0=off, 1=on, other unknown or unsupported
	bkl
		current backlight level, = step level, ie 5 on CN3 means maximum
	BKL stat
		current backlight status, 0=off, 1=on, others=unknown
