#!/bin/bash

mkdir -p DistributionTool/build/lib
mkdir -p tmp_build

cp -as $VITASDK/arm-vita-eabi/lib/python3.11 tmp_build
cp -as $(realpath ../pygame_sdl2/src/pygame_sdl2) tmp_build
cp -as $(realpath ../renpy/renpy) tmp_build

#Compile .py files for renpy & pygame alongside regular python3, remove the rest, put it in a nice zip
find tmp_build -mindepth 2 -type f -exec mv -t tmp_build {} +
find tmp_build -type d -empty -delete
pushd tmp_build
find . -type l -not -name \*.py -delete
find . -type d -empty -delete
python3 -OO -m compileall -b .
find . -type l -not -name \*.pyc -delete
mv pygame_sdl2 python3.11
mv renpy python3.11
cd python3.11
mv * ..
cd ..
find . -type d -empty -delete
7z a -tzip ../python311.zip .
popd
mv python311.zip DistributionTool/build/lib/python311.zip

#Move renpy/common to its own directory
mkdir -p DistributionTool/build/renpy
cp -r $(realpath ../renpy/renpy/common) DistributionTool/build/renpy/common
cp $(realpath ../renpy/renpy.py) DistributionTool/build/renpy.py

#Copy every C file from renpy & pygame as modules
mkdir -p source/module
rsync -avm --include='*/' --include='*.c' --exclude='*' ../renpy/module/ source/module/
rsync -avm --include='*/' --include='*.c' --exclude='*' ../pygame_sdl2/ source/module/
find source/module -mindepth 2 -type f -exec mv -t source/module {} +
find source/module -type d -empty -delete

#Remove the naughty/unnecessary ones 
pushd source/module
rm tinyfiledialogs.c _renpytfd.c sdl2.c pygame_sdl2.mixer.c pygame_sdl2.font.c pygame_sdl2.mixer_music.c
popd

#Copy every header to include
mkdir -p include/module include/module/pygame_sdl2
rsync -avm --include='*/' --include='*.h' --exclude='*' ../pygame_sdl2/ include/module/pygame_sdl2
find include/module/pygame_sdl2 -mindepth 2 -type f -exec mv -t include/module/pygame_sdl2 {} +
mv include/module/pygame_sdl2/surface.h include/module/pygame_sdl2/src
rsync -avm --include='*/' --include='*.h' --exclude='*' ../renpy/module/ include/module
mv source/module/hydrogen.c include/module/libhydrogen
find include/module -type d -empty -delete





