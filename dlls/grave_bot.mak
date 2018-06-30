# Microsoft Developer Studio Generated NMAKE File, Based on grave_bot.dsp
!IF "$(CFG)" == ""
CFG=grave_bot - Win32 Debug Metamod
!MESSAGE No configuration specified. Defaulting to grave_bot - Win32 Debug Metamod.
!ENDIF 

!IF "$(CFG)" != "grave_bot - Win32 Release" && "$(CFG)" != "grave_bot - Win32 Debug" && "$(CFG)" != "grave_bot - Win32 Debug Metamod" && "$(CFG)" != "grave_bot - Win32 Release Metamod"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "grave_bot - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\grave_bot.dll" ".\grave_bot"


CLEAN :
	-@erase "$(INTDIR)\bot.obj"
	-@erase "$(INTDIR)\bot_client.obj"
	-@erase "$(INTDIR)\bot_combat.obj"
	-@erase "$(INTDIR)\bot_navigate.obj"
	-@erase "$(INTDIR)\bot_start.obj"
	-@erase "$(INTDIR)\bot_weapons.obj"
	-@erase "$(INTDIR)\bot_weapons_hldm.obj"
	-@erase "$(INTDIR)\bot_weapons_si.obj"
	-@erase "$(INTDIR)\dll.obj"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\linkfunc.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\waypoint.obj"
	-@erase "$(OUTDIR)\grave_bot.dll"
	-@erase "$(OUTDIR)\grave_bot.exp"
	-@erase "$(OUTDIR)\grave_bot.lib"
	-@erase "$(OUTDIR)\grave_bot.map"
	-@erase "$(OUTDIR)\grave_bot.pdb"
	-@erase "grave_bot"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /Fp"$(INTDIR)\grave_bot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\grave_bot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\grave_bot.pdb" /map:"$(INTDIR)\grave_bot.map" /debug /machine:I386 /def:".\grave_bot.def" /out:"$(OUTDIR)\grave_bot.dll" /implib:"$(OUTDIR)\grave_bot.lib" 
LINK32_OBJS= \
	"$(INTDIR)\bot.obj" \
	"$(INTDIR)\bot_client.obj" \
	"$(INTDIR)\bot_combat.obj" \
	"$(INTDIR)\bot_navigate.obj" \
	"$(INTDIR)\bot_start.obj" \
	"$(INTDIR)\bot_weapons.obj" \
	"$(INTDIR)\bot_weapons_hldm.obj" \
	"$(INTDIR)\bot_weapons_si.obj" \
	"$(INTDIR)\dll.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\linkfunc.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\waypoint.obj"

"$(OUTDIR)\grave_bot.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetDir=.\Release
TargetName=grave_bot
InputPath=.\Release\grave_bot.dll
InputName=grave_bot
SOURCE="$(InputPath)"

".\grave_bot" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
<< 
	

!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\grave_bot.dll" "$(OUTDIR)\grave_bot.bsc" ".\grave_bot"


CLEAN :
	-@erase "$(INTDIR)\bot.obj"
	-@erase "$(INTDIR)\bot.sbr"
	-@erase "$(INTDIR)\bot_client.obj"
	-@erase "$(INTDIR)\bot_client.sbr"
	-@erase "$(INTDIR)\bot_combat.obj"
	-@erase "$(INTDIR)\bot_combat.sbr"
	-@erase "$(INTDIR)\bot_navigate.obj"
	-@erase "$(INTDIR)\bot_navigate.sbr"
	-@erase "$(INTDIR)\bot_start.obj"
	-@erase "$(INTDIR)\bot_start.sbr"
	-@erase "$(INTDIR)\bot_weapons.obj"
	-@erase "$(INTDIR)\bot_weapons.sbr"
	-@erase "$(INTDIR)\bot_weapons_hldm.obj"
	-@erase "$(INTDIR)\bot_weapons_hldm.sbr"
	-@erase "$(INTDIR)\bot_weapons_si.obj"
	-@erase "$(INTDIR)\bot_weapons_si.sbr"
	-@erase "$(INTDIR)\dll.obj"
	-@erase "$(INTDIR)\dll.sbr"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\engine.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\linkfunc.obj"
	-@erase "$(INTDIR)\linkfunc.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\waypoint.obj"
	-@erase "$(INTDIR)\waypoint.sbr"
	-@erase "$(OUTDIR)\grave_bot.bsc"
	-@erase "$(OUTDIR)\grave_bot.dll"
	-@erase "$(OUTDIR)\grave_bot.exp"
	-@erase "$(OUTDIR)\grave_bot.ilk"
	-@erase "$(OUTDIR)\grave_bot.lib"
	-@erase "$(OUTDIR)\grave_bot.pdb"
	-@erase "grave_bot"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\grave_bot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\grave_bot.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\bot.sbr" \
	"$(INTDIR)\bot_client.sbr" \
	"$(INTDIR)\bot_combat.sbr" \
	"$(INTDIR)\bot_navigate.sbr" \
	"$(INTDIR)\bot_start.sbr" \
	"$(INTDIR)\bot_weapons.sbr" \
	"$(INTDIR)\bot_weapons_hldm.sbr" \
	"$(INTDIR)\bot_weapons_si.sbr" \
	"$(INTDIR)\dll.sbr" \
	"$(INTDIR)\engine.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\linkfunc.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\waypoint.sbr"

"$(OUTDIR)\grave_bot.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\grave_bot.pdb" /debug /machine:I386 /def:".\grave_bot.def" /out:"$(OUTDIR)\grave_bot.dll" /implib:"$(OUTDIR)\grave_bot.lib" 
LINK32_OBJS= \
	"$(INTDIR)\bot.obj" \
	"$(INTDIR)\bot_client.obj" \
	"$(INTDIR)\bot_combat.obj" \
	"$(INTDIR)\bot_navigate.obj" \
	"$(INTDIR)\bot_start.obj" \
	"$(INTDIR)\bot_weapons.obj" \
	"$(INTDIR)\bot_weapons_hldm.obj" \
	"$(INTDIR)\bot_weapons_si.obj" \
	"$(INTDIR)\dll.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\linkfunc.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\waypoint.obj"

"$(OUTDIR)\grave_bot.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetDir=.\Debug
TargetName=grave_bot
InputPath=.\Debug\grave_bot.dll
InputName=grave_bot
SOURCE="$(InputPath)"

".\grave_bot" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
<< 
	

!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"

OUTDIR=.\Debug_Metamod
INTDIR=.\Debug_Metamod
# Begin Custom Macros
OutDir=.\Debug_Metamod
# End Custom Macros

ALL : "$(OUTDIR)\grave_bot_mm.dll" "$(OUTDIR)\grave_bot.bsc" ".\grave_bot_mm"


CLEAN :
	-@erase "$(INTDIR)\bot.obj"
	-@erase "$(INTDIR)\bot.sbr"
	-@erase "$(INTDIR)\bot_client.obj"
	-@erase "$(INTDIR)\bot_client.sbr"
	-@erase "$(INTDIR)\bot_combat.obj"
	-@erase "$(INTDIR)\bot_combat.sbr"
	-@erase "$(INTDIR)\bot_navigate.obj"
	-@erase "$(INTDIR)\bot_navigate.sbr"
	-@erase "$(INTDIR)\bot_start.obj"
	-@erase "$(INTDIR)\bot_start.sbr"
	-@erase "$(INTDIR)\bot_weapons.obj"
	-@erase "$(INTDIR)\bot_weapons.sbr"
	-@erase "$(INTDIR)\bot_weapons_hldm.obj"
	-@erase "$(INTDIR)\bot_weapons_hldm.sbr"
	-@erase "$(INTDIR)\bot_weapons_si.obj"
	-@erase "$(INTDIR)\bot_weapons_si.sbr"
	-@erase "$(INTDIR)\dll.obj"
	-@erase "$(INTDIR)\dll.sbr"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\engine.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\linkfunc.obj"
	-@erase "$(INTDIR)\linkfunc.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\waypoint.obj"
	-@erase "$(INTDIR)\waypoint.sbr"
	-@erase "$(OUTDIR)\grave_bot.bsc"
	-@erase "$(OUTDIR)\grave_bot_mm.dll"
	-@erase "$(OUTDIR)\grave_bot_mm.exp"
	-@erase "$(OUTDIR)\grave_bot_mm.ilk"
	-@erase "$(OUTDIR)\grave_bot_mm.lib"
	-@erase "$(OUTDIR)\grave_bot_mm.pdb"
	-@erase "grave_bot_mm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "METAMOD_BUILD" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\grave_bot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\grave_bot.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\bot.sbr" \
	"$(INTDIR)\bot_client.sbr" \
	"$(INTDIR)\bot_combat.sbr" \
	"$(INTDIR)\bot_navigate.sbr" \
	"$(INTDIR)\bot_start.sbr" \
	"$(INTDIR)\bot_weapons.sbr" \
	"$(INTDIR)\bot_weapons_hldm.sbr" \
	"$(INTDIR)\bot_weapons_si.sbr" \
	"$(INTDIR)\dll.sbr" \
	"$(INTDIR)\engine.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\linkfunc.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\waypoint.sbr"

"$(OUTDIR)\grave_bot.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\grave_bot_mm.pdb" /debug /machine:I386 /def:".\grave_bot.def" /out:"$(OUTDIR)\grave_bot_mm.dll" /implib:"$(OUTDIR)\grave_bot_mm.lib" 
LINK32_OBJS= \
	"$(INTDIR)\bot.obj" \
	"$(INTDIR)\bot_client.obj" \
	"$(INTDIR)\bot_combat.obj" \
	"$(INTDIR)\bot_navigate.obj" \
	"$(INTDIR)\bot_start.obj" \
	"$(INTDIR)\bot_weapons.obj" \
	"$(INTDIR)\bot_weapons_hldm.obj" \
	"$(INTDIR)\bot_weapons_si.obj" \
	"$(INTDIR)\dll.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\linkfunc.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\waypoint.obj"

"$(OUTDIR)\grave_bot_mm.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetDir=.\Debug_Metamod
TargetName=grave_bot_mm
InputPath=.\Debug_Metamod\grave_bot_mm.dll
InputName=grave_bot_mm
SOURCE="$(InputPath)"

".\grave_bot_mm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
<< 
	

!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"

OUTDIR=.\Release_Metamod
INTDIR=.\Release_Metamod
# Begin Custom Macros
OutDir=.\Release_Metamod
# End Custom Macros

ALL : "$(OUTDIR)\grave_bot_mm.dll" ".\grave_bot_mm"


CLEAN :
	-@erase "$(INTDIR)\bot.obj"
	-@erase "$(INTDIR)\bot_client.obj"
	-@erase "$(INTDIR)\bot_combat.obj"
	-@erase "$(INTDIR)\bot_navigate.obj"
	-@erase "$(INTDIR)\bot_start.obj"
	-@erase "$(INTDIR)\bot_weapons.obj"
	-@erase "$(INTDIR)\bot_weapons_hldm.obj"
	-@erase "$(INTDIR)\bot_weapons_si.obj"
	-@erase "$(INTDIR)\dll.obj"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\linkfunc.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\waypoint.obj"
	-@erase "$(OUTDIR)\grave_bot_mm.dll"
	-@erase "$(OUTDIR)\grave_bot_mm.exp"
	-@erase "$(OUTDIR)\grave_bot_mm.lib"
	-@erase "$(OUTDIR)\grave_bot_mm.map"
	-@erase "$(OUTDIR)\grave_bot_mm.pdb"
	-@erase "grave_bot_mm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\metamod" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "METAMOD_BUILD" /D "VALVE_DLL" /Fp"$(INTDIR)\grave_bot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\grave_bot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\grave_bot_mm.pdb" /map:"$(INTDIR)\grave_bot_mm.map" /debug /machine:I386 /def:".\grave_bot.def" /out:"$(OUTDIR)\grave_bot_mm.dll" /implib:"$(OUTDIR)\grave_bot_mm.lib" 
LINK32_OBJS= \
	"$(INTDIR)\bot.obj" \
	"$(INTDIR)\bot_client.obj" \
	"$(INTDIR)\bot_combat.obj" \
	"$(INTDIR)\bot_navigate.obj" \
	"$(INTDIR)\bot_start.obj" \
	"$(INTDIR)\bot_weapons.obj" \
	"$(INTDIR)\bot_weapons_hldm.obj" \
	"$(INTDIR)\bot_weapons_si.obj" \
	"$(INTDIR)\dll.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\linkfunc.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\waypoint.obj"

"$(OUTDIR)\grave_bot_mm.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetDir=.\Release_Metamod
TargetName=grave_bot_mm
InputPath=.\Release_Metamod\grave_bot_mm.dll
InputName=grave_bot_mm
SOURCE="$(InputPath)"

".\grave_bot_mm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\si\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\half-life\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\crabbed\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\valve\dlls 
	copy $(TargetDir)\$(InputName).dll C:\Steam\SteamApps\ghoul@nocensmundus.net\HLServer\si\dlls 
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("grave_bot.dep")
!INCLUDE "grave_bot.dep"
!ELSE 
!MESSAGE Warning: cannot find "grave_bot.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "grave_bot - Win32 Release" || "$(CFG)" == "grave_bot - Win32 Debug" || "$(CFG)" == "grave_bot - Win32 Debug Metamod" || "$(CFG)" == "grave_bot - Win32 Release Metamod"
SOURCE=.\bot.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\bot.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\bot.obj"	"$(INTDIR)\bot.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\bot.obj"	"$(INTDIR)\bot.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\bot.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\bot_client.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\bot_client.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\bot_client.obj"	"$(INTDIR)\bot_client.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\bot_client.obj"	"$(INTDIR)\bot_client.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\bot_client.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\bot_combat.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\bot_combat.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\bot_combat.obj"	"$(INTDIR)\bot_combat.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\bot_combat.obj"	"$(INTDIR)\bot_combat.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\bot_combat.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\bot_navigate.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\bot_navigate.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\bot_navigate.obj"	"$(INTDIR)\bot_navigate.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\bot_navigate.obj"	"$(INTDIR)\bot_navigate.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\bot_navigate.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\bot_start.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\bot_start.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\bot_start.obj"	"$(INTDIR)\bot_start.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\bot_start.obj"	"$(INTDIR)\bot_start.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\bot_start.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\bot_weapons.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\bot_weapons.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\bot_weapons.obj"	"$(INTDIR)\bot_weapons.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\bot_weapons.obj"	"$(INTDIR)\bot_weapons.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\bot_weapons.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\bot_weapons_hldm.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\bot_weapons_hldm.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\bot_weapons_hldm.obj"	"$(INTDIR)\bot_weapons_hldm.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\bot_weapons_hldm.obj"	"$(INTDIR)\bot_weapons_hldm.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\bot_weapons_hldm.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\bot_weapons_si.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\bot_weapons_si.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\bot_weapons_si.obj"	"$(INTDIR)\bot_weapons_si.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\bot_weapons_si.obj"	"$(INTDIR)\bot_weapons_si.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\bot_weapons_si.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dll.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\dll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\dll.obj"	"$(INTDIR)\dll.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\dll.obj"	"$(INTDIR)\dll.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\dll.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\engine.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\engine.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\engine.obj"	"$(INTDIR)\engine.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\engine.obj"	"$(INTDIR)\engine.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\engine.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\h_export.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\h_export.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\h_export.obj"	"$(INTDIR)\h_export.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\h_export.obj"	"$(INTDIR)\h_export.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\h_export.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\linkfunc.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\linkfunc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\linkfunc.obj"	"$(INTDIR)\linkfunc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\linkfunc.obj"	"$(INTDIR)\linkfunc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\linkfunc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\util.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\waypoint.cpp

!IF  "$(CFG)" == "grave_bot - Win32 Release"


"$(INTDIR)\waypoint.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug"


"$(INTDIR)\waypoint.obj"	"$(INTDIR)\waypoint.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Debug Metamod"


"$(INTDIR)\waypoint.obj"	"$(INTDIR)\waypoint.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "grave_bot - Win32 Release Metamod"


"$(INTDIR)\waypoint.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

