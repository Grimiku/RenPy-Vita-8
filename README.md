# RenPy-Vita-8
This is an updated version of [SonicMastr's Ren'Py 7 Vita port](https://github.com/SonicMastr/renpy-vita/tree/main) that changes the underlying library from Python2.7 to Python3.11 and bumps the supported features from late 2021 up to end of 2024.

## Instructions for regular users
This repo presupposes that <ins>**you have your Ren'Py game already installed on the Vita**</ins>. Please use the [Ren'Py PSVita Distribution Tool](https://github.com/SonicMastr/renpy-vita/releases/tag/v1.0) to create your VPK if you haven't done it yet and install your game before proceeding.

1) Go into [Release section of this repo](https://github.com/Grimiku/RenPy-Vita-8/releases/) and download the latest .zip archive (ie. _RenPy_Vita_8_v0.1.zip_);
2) Open the app folder of your game on the Vita (ie. _ux0:app/BRBRB001/_) and delete everything apart from _game/_ and _sce_sys/_ folders;
3) Unpack everything from the .zip from step 1) into the folder from step 2);
4) Proceed to run the game as you normally would by selecting it from LiveArea.

## Build instructions for some crazy people who want to build this, ignore otherwise
One of the many things I learned during the update process for this particual port is that every build instruction becomes outdated extremely fast. Another thing is that vast majority of the build process boils down to work outside of the main repo (ie. VITASDK setup, SDL2 setup, other dependencies). That's why below are just major points to look out for while maybe at a later stage more detailed steps will follow:

* Requires linux enviroment or WSL on Windows;
* Requires Python 3.11;
* Works under Cython 0.29.37 but not >3.0;
* Requires rsync for the bash script to work;
* Requires SDL2 built with -DVIDEO_VITA_PVR=ON flag;
* Requires you make sure openssl-1.1.1 is recognized by your vitasdk during cmake;
* Requires a couple of additional modifications to CPython3.11 for Vita and additional libraries. You can take a look at [my fork](https://github.com/Grimiku/cpython-vita) and modify your vitasdk with it or just run your own stuff based on my commits there;
* Requires modifying vitasdk freetype package to compile with harfbuzz;
* _all_in_one.bash_ will try to do all the work for you but expect it will throw errors;
* Requires a lot of patience.
