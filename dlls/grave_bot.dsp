<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
# Microsoft Developer Studio Project File - Name="grave_bot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=grave_bot - Win32 Debug Metamod
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "grave_bot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "grave_bot.mak" CFG="grave_bot - Win32 Debug Metamod"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "grave_bot - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "grave_bot - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "grave_bot - Win32 Debug Metamod" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "grave_bot - Win32 Release Metamod" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SDKSrc/Public/dlls", NVGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "grave_bot - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /def:".\grave_bot.def"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\grave_bot.def" /out:"Release\grave_bot.dll"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Performing Custom Build Step on $(InputPath)
TargetDir=.\Release
TargetName=grave_bot
InputPath=.\Release\grave_bot.dll
InputName=grave_bot
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\grave_bot___Win"
# PROP BASE Intermediate_Dir ".\grave_bot___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\grave_bot.def"
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\grave_bot.def" /out:"Debug\grave_bot.dll"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Performing Custom Build Step on $(InputPath)
TargetDir=.\Debug
TargetName=grave_bot
InputPath=.\Debug\grave_bot.dll
InputName=grave_bot
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Metamod"
# PROP BASE Intermediate_Dir "Debug_Metamod"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Metamod"
# PROP Intermediate_Dir "Debug_Metamod"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "METAMOD_BUILD" /D "VALVE_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\engine" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\grave_bot.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\grave_bot.def" /out:"Debug_Metamod\grave_bot_mm.dll"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Performing Custom Build Step on $(InputPath)
TargetDir=.\Debug_Metamod
TargetName=grave_bot_mm
InputPath=.\Debug_Metamod\grave_bot_mm.dll
InputName=grave_bot_mm
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_Metamod"
# PROP BASE Intermediate_Dir "Release_Metamod"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Metamod"
# PROP Intermediate_Dir "Release_Metamod"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "METAMOD_BUILD" /D "VALVE_DLL" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\grave_bot.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\grave_bot.def" /out:"Release_Metamod\grave_bot_mm.dll"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Performing Custom Build Step on $(InputPath)
TargetDir=.\Release_Metamod
TargetName=grave_bot_mm
InputPath=.\Release_Metamod\grave_bot_mm.dll
InputName=grave_bot_mm
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "grave_bot - Win32 Release"
# Name "grave_bot - Win32 Debug"
# Name "grave_bot - Win32 Debug Metamod"
# Name "grave_bot - Win32 Release Metamod"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\bot.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_client.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_combat.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_navigate.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_start.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_weapons_hldm.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_weapons_si.cpp
# End Source File
# Begin Source File

SOURCE=.\dll.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp
# End Source File
# Begin Source File

SOURCE=.\linkfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\waypoint.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\bot.h
# End Source File
# Begin Source File

SOURCE=.\bot_client.h
# End Source File
# Begin Source File

SOURCE=.\bot_func.h
# End Source File
# Begin Source File

SOURCE=.\bot_weapons.h
# End Source File
# Begin Source File

SOURCE=.\enginecallback.h
# End Source File
# Begin Source File

SOURCE=.\extdll.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# Begin Source File

SOURCE=.\waypoint.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
<<<<<<< HEAD
=======
=======
# Microsoft Developer Studio Project File - Name="grave_bot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=grave_bot - Win32 Debug Metamod
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "grave_bot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "grave_bot.mak" CFG="grave_bot - Win32 Debug Metamod"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "grave_bot - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "grave_bot - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "grave_bot - Win32 Debug Metamod" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "grave_bot - Win32 Release Metamod" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SDKSrc/Public/dlls", NVGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "grave_bot - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /def:".\grave_bot.def"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\grave_bot.def" /out:"Release\grave_bot.dll"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Performing Custom Build Step on $(InputPath)
TargetDir=.\Release
TargetName=grave_bot
InputPath=.\Release\grave_bot.dll
InputName=grave_bot
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\grave_bot___Win"
# PROP BASE Intermediate_Dir ".\grave_bot___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\grave_bot.def"
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\grave_bot.def" /out:"Debug\grave_bot.dll"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Performing Custom Build Step on $(InputPath)
TargetDir=.\Debug
TargetName=grave_bot
InputPath=.\Debug\grave_bot.dll
InputName=grave_bot
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Metamod"
# PROP BASE Intermediate_Dir "Debug_Metamod"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Metamod"
# PROP Intermediate_Dir "Debug_Metamod"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "METAMOD_BUILD" /D "VALVE_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\engine" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\grave_bot.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\grave_bot.def" /out:"Debug_Metamod\grave_bot_mm.dll"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Performing Custom Build Step on $(InputPath)
TargetDir=.\Debug_Metamod
TargetName=grave_bot_mm
InputPath=.\Debug_Metamod\grave_bot_mm.dll
InputName=grave_bot_mm
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_Metamod"
# PROP BASE Intermediate_Dir "Release_Metamod"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Metamod"
# PROP Intermediate_Dir "Release_Metamod"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "METAMOD_BUILD" /D "VALVE_DLL" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\grave_bot.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\grave_bot.def" /out:"Release_Metamod\grave_bot_mm.dll"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Performing Custom Build Step on $(InputPath)
TargetDir=.\Release_Metamod
TargetName=grave_bot_mm
InputPath=.\Release_Metamod\grave_bot_mm.dll
InputName=grave_bot_mm
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "grave_bot - Win32 Release"
# Name "grave_bot - Win32 Debug"
# Name "grave_bot - Win32 Debug Metamod"
# Name "grave_bot - Win32 Release Metamod"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\bot.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_client.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_combat.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_navigate.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_start.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_weapons_hldm.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_weapons_si.cpp
# End Source File
# Begin Source File

SOURCE=.\dll.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp
# End Source File
# Begin Source File

SOURCE=.\linkfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\waypoint.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\bot.h
# End Source File
# Begin Source File

SOURCE=.\bot_client.h
# End Source File
# Begin Source File

SOURCE=.\bot_func.h
# End Source File
# Begin Source File

SOURCE=.\bot_weapons.h
# End Source File
# Begin Source File

SOURCE=.\enginecallback.h
# End Source File
# Begin Source File

SOURCE=.\extdll.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# Begin Source File

SOURCE=.\waypoint.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
>>>>>>> 3f195a42c7b84bf4071b789634ebbe14b6297ebc
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
