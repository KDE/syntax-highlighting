#!/bin/bash

for i in `ls tests/auto/output/*.html`; do
    refFile=`echo $i | sed -e s,build,src, | sed -e s,output,reference, | sed -e s,.html,.ref.html,`
    cp -v $i $refFile
done

## >Settings >Configure Kate >Fonts & Colors >Highlitghing Text Styles >Scripts/Bash >Option >Change colors to some distinct color
## 1- In following line the -ucode should not be colored as option

pacman -Syu --needed intel-ucode grub
pacman -syu --needed intel-ucode grub
