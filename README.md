# Blade-Mistress
This archived depot contains Blade Mistress, a small MMO from the early 2000's that was created by Thom Robertson.  It was influenced heavily by MUDs and was meant to be low bandwidth yet still provide a fun, unique experience to players.

The game had unique features for the time, such as:

* Customizable guild towers
* Dungeons that randomly chose players as owners, who could then edit the dungeon for other players
* Pet drakes and a full breeding system for them
* "Chants" players could do together with various outcomes
* Talking trees
* Collectable totems that give the owning guild a bonus

Even after almost 20 years, there are still players interested in playing and modifying the game, which is humbling.  And so we have decided to release this code into the public domain so they could add new features and fix bugs.

# Buliding
Building requires Visual Studio 2017 and has only been tested on Windows 10 64bit.  The following components need to be installed:

* Windows 8.1 SDK
* MFC and ATL support

# Deploying
The Scripts directory has 2 scripts, one that creates a Client folder heirarchy and one that creates a Server heirarchy.  Both require Release executables to be built.  The Client structure is also what is used by the Update system. The contents of StagingFilesClient can go on any web server, in any sub folder.  That full URL should then be used in the Launcher as the update URL.

# The Original Blade Mistress Readme
** Blade Mistress **

by Thom Robertson

Thank you for playing Blade Mistress!

Blade Mistress is a server-centric, ultra-low bandwidth, persistant world,
massively multiplayer, 3D graphical fantasy role-playing game.

What does ultra-low bandwidth mean?  It means that bandwidth
costs big bucks, so the only way to bring this game to you is
to design the game from the ground up with a commitment to using
as little network bandwidth as possible.  The result is a game
map that is divided into large squares, and you move from
square to square, instead of just going anywhere you want.

check out our web page at http://www.blademistress.com for
the latest info, and a link to our forums.
