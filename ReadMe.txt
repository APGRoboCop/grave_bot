===========================================================================================
===============================Grave Bot===Readme===29/June/18=============================
===========================================================================================

ChangeLog v0.4-beta2:-
>> Both builds now support Meta API 5:13

>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
ChangeLog v0.4-beta1:-
>> Non-Metamod GraveBot build support removed
>> Fixed the h_export.cpp for Linux Server to detect SI build as 'si.so' not 'si_i386.so'
>> Both builds optimised for SSE2
>> Win32 build recompiled by VS2013
>> Linux build recompiled by G++ 4.8

+--------+
| NOTICE |
+--------+
Grave Bot is an experimental program for Half-Life.  I'm not liable for any horrible damage
sustained by your computer if you install Grave Bot.  Why that would happen, I'm not sure,
but never the less don't point fingers towards me!  Grave Bot was NOT created with the
intention of gaining wide spread server use.  It's purpose is more centered around single
use, LAN games, or private servers, and as a demonstration to the community of what is
possible inside the Half-Life engine.  The bot does have bugs; it is not perfect, I'm
sorry.  It's the best I could do for now, especially considering Half-Life 2 is out. :P

See the guide below to install Grave Bot, and the grave_bot_commands.txt file for a list of
commands.  

+--------------+
| INSTALLATION |
+--------------+
Anyway, to install Grave Bot, extract the contents of the zip file to your Half-Life
directory.  This should place all Grave Bot files in the correct folders.  If not, refer
to the list below.  There are three different ways to install Grave Bot, Steam, Non-Steam,
and Metamod.  Please use the list below.  Note that third party mods on Steam may want to
use the Non-Steam method.  See the notice below.

+-----------------+
| STEAM (Windows) |
+-----------------+

1. Run Steam.
2. Right click Half-Life (even for S&I) in the Play games menu and select properties.
3. Push the Launch options button.
4. Add "-dll dlls\grave_bot.dll" to the text box and press okay.
5. Run HL (or S&I) and start a multiplayer game!

NOTICE: This will override the DLL settings for ALL third party mods that run under HL!
Use the NON-STEAM method to avoid this issue.  

+---------------------+
| NON-STEAM (Windows) |
+---------------------+

1. Browse into the mod folder (Half-Life\valve or Half-Life\si).
2. Open "liblist.gam" in a text editor.
3. Comment out the "gamedll" line with two forward flashes like so:
	//gamedll "dlls\hl.dll"
   Add a new gamedll line below that which reads:
	gamedll "dlls\grave_bot.dll"
4. Save and exit the file.
5. Run HLDM (or S&I) and start a multiplayer game!

+-------------------+
| NON-STEAM (Linux) |
+-------------------+

1. Browse into the mod folder (Half-Life\valve or Half-Life\si).
2. Open "liblist.gam" in a text editor.
3. Comment out the "gamedll_linux" line with two forward flashes like so:
	//gamedll_linux "dlls\hl_i386.so"
   Add a new gamedll line below that which reads:
	gamedll_linux "dlls\gravebot_i386.so"
4. Save and exit the file.
5. Run HLDM (or S&I) and start a multiplayer game!

+-----------------------------+
| METAMOD (Windows and Linux) |
+-----------------------------+

See the metamod website for information on installing metamod plugins.  Installing Grave
Bot using metamod is only recommended for use on servers where more than one non-bot will
be playing, such as public and private servers.  If you're going to just test the bot, it
might be easier to use the standard method.

http://www.metamod.org/

+----------------+
| FILE STRUCTURE |
+----------------+
If Grave Bot is not working correctly, make sure all files are in their proper locations.

modfolder\dlls\grave_bot.dll
modfolder\dlls\grave_bot_mm.dll
modfolders\dlls\gravebot_i386.so
modfolders\dlls\gravebot_mm_i386.so
modfolder\grave_bot.cfg
modfolder\grave_bot_names.txt
modfolder\maps\*.gbw

+--------------------+
| Credits and Thanks |
+--------------------+

Ghoul: Programming, Website, some Waypointing

RoboCop: Minor fixes

CoCoNUT: Waypointing

dub: Help with the S&I looping issue

MrBozo: Providing the necessary resources to get Grave Bot working with S&I

Pierre-Marie Baty: Helped me with many things :P

botman: Writing the original HPB bot!
