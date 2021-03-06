# Boris
My first Brood War AI! He's not capable of much, but he's mostly functional!
He is built on BWAPI 4.4.0 as a Client.
Starcraft Brood War 1.16.1 required.
**Pre-compiled binary comes in _release_ folder.**

Boris only knows one thing: 9pool, then swarm the enemy spawn point.
He is also not very smart on maps that aren't 1v1. He has a ~100% win rate vs. vanilla Zerg AI, ~70% vs Terran, and absolutely gets dunked on by Protoss, or non-vanilla AI.

### Known issues:
 - Boris might still have some frames where Zerglings are idle, or a building isn't being constructed when it should be. _It's actually pretty bad in v3_
 - Boris might send units to the very upper left or the center of the map. He also occasionally sends the swarm to the wrong place, which also usually leads to his doom. Not sure what causes this yet.
 - Boris has some extremely rare nullptr-related crashes. Trying to fix it.
 - Boris performs poorly on maps designed for more than 4 players, but still functions. However, by the time he usually finds the enemy, his zergling rush strategy is ineffective.
 - Boris treats Zerglings like old Conscripts and just suicides them mindlessly at the (first) enemy spawn point. Multiple enemy bases, enemies, or spawn points are an issue.

### To compile Boris in Visual Studio:
 - Clone or download [BWAPI 4.4.0](https://bwapi.github.io) first as this is required!
 - Clone or download the repository, and place the folder into your (bwapi install folder)/bwapi
 - Open _Boris.sln_ (or add _Boris.vcxproj_ to your bwapi.sln) (I use Visual Studio 2019, it may not work on older versions)

 - If you don't place this in your BWAPI folder, you will have to make sure all references to BWAPI are accounted for.
 - I develop by adding bots to bwapi.sln as new projects, and then making my own repositories afterward. Not sure if this causes issues?
 
 ### Changelog:
 ###### v1
  - initial release
 ###### v2
  - updated to scout maps besides 1v1
  - updated zergling initial attack code to be more efficient instead of "just go to the first available spawn point"
 ###### v3
  - updated scouting code so units that aren't scouts can also discover the enemy meaningfully
  - more improvements to zergling attack code, but I still have that wierd idling period

### Thanks to these people from the SSCAIT community for helping me learn C++:
 - NiteKat
 - PurpleWave
 - Nathan Roth (Antiga/Iruian)
 - N00byEdge
 - dishbutt
 - Ankmairdor
 - Sonko for his great Make Computer Do Things streams where he and guest developers make AIs from scratch!
