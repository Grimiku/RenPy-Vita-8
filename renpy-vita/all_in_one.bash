#!/bin/bash

mkdir -p DistributionTool/lib
mkdir -p tmp_build

cp -as $VITASDK/arm-vita-eabi/lib/python3.11 tmp_build
cp -as $(realpath ../pygame_sdl2/src/pygame_sdl2) tmp_build
cp -as $(realpath ../renpy-8/renpy) tmp_build

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
mv python311.zip DistributionTool/lib/python311.zip

#Move renpy/common to its own directory
mkdir -p DistributionTool/renpy
cp -r $(realpath ../renpy-8/renpy/common) DistributionTool/renpy/common
cp $(realpath ../renpy-8/renpy.py) DistributionTool/renpy.py

#Compile and copy every C file from renpy & pygame as modules
mkdir -p source/module
cd ../pygame_sdl2
rm -rf gen3-static
PYGAME_SDL2_STATIC=1 python3 setup.py
cd ../renpy-8/module
rm -rf gen3-static
RENPY_DEPS_INSTALL=$VITASDK/arm-vita-eabi RENPY_STATIC=1 python3 setup.py
cd ../..
rsync -avm --include='*/' --include='*.c' --exclude='*' renpy-8/module/ renpy-vita/source/module
rsync -avm --include='*/' --include='*.c' --exclude='*' pygame_sdl2/ renpy-vita/source/module
find renpy-vita/source/module -mindepth 2 -type f -exec mv -t renpy-vita/source/module {} +
find renpy-vita/source/module -type d -empty -delete

#Remove the naughty/unnecessary ones 
pushd renpy-vita/source/module
rm tinyfiledialogs.c _renpytfd.c sdl2.c pygame_sdl2.mixer.c pygame_sdl2.font.c pygame_sdl2.mixer_music.c
popd

#Copy every header to include
mkdir -p renpy-vita/include/module renpy-vita/include/module/pygame_sdl2
rsync -avm --include='*/' --include='*.h' --exclude='*' pygame_sdl2/ renpy-vita/include/module/pygame_sdl2
find renpy-vita/include/module/pygame_sdl2 -mindepth 2 -type f -exec mv -t renpy-vita/include/module/pygame_sdl2 {} +
mv renpy-vita/include/module/pygame_sdl2/surface.h renpy-vita/include/module/pygame_sdl2/src
rsync -avm --include='*/' --include='*.h' --exclude='*' renpy-8/module/ renpy-vita/include/module
mv renpy-vita/source/module/hydrogen.c renpy-vita/include/module/libhydrogen
find renpy-vita/include/module -type d -empty -delete

#Try building
cd renpy-vita
cmake -Bbuild
cd build
make -j${nproc}
cd ..
mv build/eboot.bin DistributionTool/
