# Natural Selection v3.2.3
Updated by ALIEN BIRD ARMAGEDDON

This is a complete rebuild of the game [Natural Selection] for Windows, Linux, and OS X. It includes updates and fixes by Alien, pierow, Prefix, fmoraw, Bacsu, and others.

## Downloads

Install the game using one of the following methods:

**[Natural Selection Launcher](https://github.com/EterniumDev/NaturalLauncher/releases/)** (recommended for Windows) The launcher can install, update, or repair the game.

**[Manual installation](https://github.com/EterniumDev/NS/releases)** (Linux / OSX / Windows)

As the game is a Half-Life mod, Steam and Half-Life installations are required to play the game.

A fresh install of NS comes with updated config files containing everything you need to get playing on the standard settings most players prefer.  There are also official marine and alien config files that can be uncommented so your binds change when joining the respective teams, but they are off by default as to not override any binds users wish to change within the game menus.  Those that wish to use the Nine Legends competitive pack can do so through the settings in advanced options without having to download or install it.

If you wish to experience the game's default settings and binds from NS 3.2 you can type `exec olddefaults.cfg` in console before loading a map. A file named `backupb4old.cfg` will be saved with your previous settings if you wish to return to those after.  Note that gamma ramp is no longer a working feature, so brightness will be a slightly different than it originally was, but the game was always very dark by default and still is with that config.

## Changes from ENSL

Updates include:

- You can customize your primary HUD colours using the commands
"cl_marinehudcolor_red"
"cl_marinehudcolor_green"
"cl_marinehudcolor_blue"
"cl_alienhudcolor_red"
"cl_alienhudcolor_green"
"cl_alienhudcolor_blue"
- Onos can no longer devour other Onos in AVA
- Focus gorge spit now travels faster
- Bile bomb can now destroy dropped items
- Longjump module is now precached and can now be spawned in or used by mods without crashing the game
- Erroneous move speed increase while walking near walls fixed (Strafing into a wall increased move speed by up to 50%)
- Skulk walljump speed now affected by celerity
- KillReward CVar added, allowing control over how resources are granted upon killing a player
Default kill rewards now use Bounty, gain 1 res + 1 res per each frag a player has upon killing them, reset frags on death
- Welder can now cleanse webbing and can destroy nearby webs
- Welder can now build structures and constructs 50% faster than normal
- Welder now recharges your armor for 5% of repairing dealt
- sv_airaccelerate can now be edited freely, allowing surfs maps to work in NS
- Catalysts now clear parasites and cleanse webbing and stuns
- Primal Scream, Catalyst and Webbed effect text messages no longer blink
- The command cl_showspeed 1 no longer requires sv_cheats 1 to be used
- Command r_detailtextures readded
- Electrified TFs and RTs can no longer attack while recycling and will no longer appear to be electrified
- AutoSwap CVar added, allowing disabling auto swap on weapon pickup (currently server wide)
- Welder now deals x2 damage vs structures
- Marine's Pistols are now dropped on death
- Extra zoom levels for marine minimap added

## Bugs
- Process hangs on exit sometimes
- Arrow keys don't work for the commander
- Chatbox eats inputs occasionally
- Some mice experience issues with a previous fix for centering the cursor when the popupmenu is open
- Trying to shoot the shotgun while reloading is very fickle and has poor syncronization with the server. A balance-conscientious rework is planned, but yet to be made.  
- HUD elements are stretched and/or poorly placed in widescreen. User options/commands for hud element placement are planned.
- Clicking on the scoreboard can cause +attack to be stuck after closing the scoreboard until firing again.
- FPS dependant fire rates and player physics.  These have significantly improved with the Half-Life update and are also a non-issue now that other FPS dependency fixes are in the game and the game can be played at high frame rates, where fire rates don't vary. Very high framerates cause collision issues and other engine limitations, so an fps of 200 or 250 is recommended unless fixed.
- Flashing available alien upgrade icons (defense, movement and sensory) tend to disappear when a player dies and respawns. The icons won't appear until the alien chooses an upgrade.
- func_train and func_tracktrain entities don't reset to their original positions when a round has ended
- If parts of map have negative Z coordinates, clicking a structure can deselect it or select antoher structure. Make sure all parts of the map have positive Z coordinate.

If you find more please report them!

## Compiling

For Windows, compilation should be working if you have VS2017 installed.

For Linux:

First you need some libraries. On Ubuntu it is:

```sh
apt-get install build-essential git gdb gcc-multilib g++-multilib libc6-i386 libcurl4-openssl-dev:i386 libpng12-dev:i386
``` 

Then you will need to get the files:
```sh
git clone https://github.com/fmoraw/NS.git
``` 

Then to build `` and use `make hl_cll` for the client and `make ns` for the server.

```sh
cd NS/main/source/linux
```

For client use `make hl_cll` and for server use `make ns`


If you get the this error when running the app: `Fatal Error - could not load library (client.so)`, With a high chance it is because of some `UNDEFINED SYMBOLS` in the shared library. But you can check this with this command:

``` sh 
ldd -r -d client.so
``` 

Make sure you have vgui.so copied to cl_dll folder too on Linux.

## Debugging

If you want to debug:
```sh
LD_LIBRARY_PATH=".:$LD_LIBRARY_PATH" gdb ./hl_linux r -game ns -dev -steam
``` 
Due to the new engine and the nature of Linux I had to make a lot of changes! You can find them with grep -Ril `@Linux`.

For MacOS & lldb:
```
DYLD_LIBRARY_PATH=".:$DYLD_LIBRARY_PATH" /Applications/Xcode.app/Contents/Developer/usr/bin/lldb -- ./hl_osx r -game ns -dev -steam -windowed
```
Have to use XCode's lldb to get around https://stackoverflow.com/a/33589760 (Lack of environment variables). `-windowed` as windowed mode makes it easier to deal with crashes on MacOS.

### Hosting a server

Since servers are really rare at this point in time here is a tutorial how to set up an [Natural Selection] server with [hlds]. 

1. Follow these steps: https://developer.valvesoftware.com/wiki/SteamCMD
2. Copy the [Natural Selection] directory to steamcmd/servers/
3. Run the game : 
```sh
./hlds_run -game ns -autoupdate +map ns_eclipse + sv_secure 1 + port 27015 + ip 127.0.0.1 +hostname "Natural Selection" +maxplayers 32
``` 
If you are behind a NAT(Router) make sure to open at least those ports: 
- 27015 UDP (game transmission, pings) 
- 26900 UDP (VAC service) -- automatically increments if used in case of additional server processes

`Under no circumstances expose your host to the internet!!!` For more information follow [this link](https://developer.valvesoftware.com/wiki/Half-Life_Dedicated_Server).

In order to check if you server is connected to the steam servers copy the following url in your browser and replace `<your IP address>` with your external ip:
`http://api.steampowered.com/ISteamApps/GetServersAtAddress/v0001?addr=<your IP address>&format=json`

There is an updated version of metamod called [metamodp](http://metamod-p.sourceforge.net/). Since players are also pretty rare you might want to install [rcbot] and therefore you have to download those files:
 - [rcbot without dlls](http://filebase.bots-united.com/index.php?act=download&id=210)
 - [rcbot linux dlls](http://filebase.bots-united.com/index.php?act=download&id=428)

### Usefull links

* [Usefull collection of ns files server addons, maps and everything ](http://www.brywright.co.uk/downloads/files/index.php?dir=natural-selection/)
* [Great collection of ns guides/tutorials](http://www.naturalns.clanservers.com/forums/index.php?topic=5.0)
*  [rcbot downloads](http://filebase.bots-united.com/index.php?act=category&id=19)
*  [dillinger.io markdown editor](http://dillinger.io/)

   [Natural Selection]: <http://unknownworlds.com/ns/>
   [Unknownworlds Entertainment]:<https://github.com/unknownworlds/NS>
   [Steam]: <http://store.steampowered.com/about/>
   [hlds]:<https://developer.valvesoftware.com/wiki/Half-Life_Dedicated_Server>
   [rcbot]:<http://filebase.bots-united.com/index.php?act=category&id=19>


Many thanks to GiGaBiTe.


Half Life 1 SDK LICENSE
=======================

https://github.com/ValveSoftware/halflife/blob/master/README.md

You may, free of charge, download and use the SDK to develop a modified Valve game running on the Half-Life engine. You may distribute your modified Valve game in source and object code form, but only for free. Terms of use for Valve games are found in the Steam Subscriber Agreement located here: http://store.steampowered.com/subscriber_agreement/

You may copy, modify, and distribute the SDK and any modifications you make to the SDK in source and object code form, but only for free. Any distribution of this SDK must include this license.txt and third_party_licenses.txt.

DISCLAIMER OF WARRANTIES. THE SOURCE SDK AND ANY OTHER MATERIAL DOWNLOADED BY LICENSEE IS PROVIDED “AS IS”. VALVE AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES WITH RESPECT TO THE SDK, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE AND FITNESS FOR A PARTICULAR PURPOSE.

LIMITATION OF LIABILITY. IN NO EVENT SHALL VALVE OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THE ENGINE AND/OR THE SDK, EVEN IF VALVE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

Natural Selection copyright and trademarks
==========================================
All artwork, sounds, audio, screenshots, text and code in Natural Selection, Zen of Sudoku, Spark engine and Natural Selection 2 are Copyright © 2014 Unknown Worlds Entertainment, Inc (http://www.unknownworlds.com).

The mark Natural Selection was first represented in association with video-game software in June of 2001, and was first used in commerce around January, 2002. Natural Selection is Registered with the U.S. Patent and Trademark Office (No. 4,179,393).

Natural Selection license
=========================
See COPYING.txt for the GNU GENERAL PUBLIC LICENSE

EXCLUDED CODE: The code described below and contained in the Natural Selection Source Code release is not part of the Program covered by the GPL and is expressly excluded from its terms. You are solely responsible for obtaining from the copyright holder a license for such code and complying with the applicable license terms.

EXCLUDED CODE AND LIBRARIES
- Half Life 1 SDK LICENSE (Copyright(C) Valve Corp.)
- FMOD 3.7.5
- Lua 5.0 (http://lua.org)
- Particle system library by David McAllister (http://www.cs.unc.edu/techreports/00-007.pdf).

Original code and design by Charlie Cleveland (charlie@unknownworlds.com, @flayra).

Many contributions from Karl Patrick (karl.patrick@gmail.com), Petter Rønningen <tankefugl@gmail.com>, Harry Walsh <harry.walsh@gmail.com>, and probably lots of people I forgot.

