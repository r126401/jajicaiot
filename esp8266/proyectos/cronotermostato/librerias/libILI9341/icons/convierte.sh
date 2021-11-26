#!/bin/bash
#
# Recorre todos los archivos del directorio actual y los muestra
#
bpp='_16bpp'
for i in $(ls *.png -C1)
do

echo convert $i a $i.bmp
convert $i -type truecolor $i.bmp



#convert $i -type truecolor $i
done


for i in $(ls *.bmp -C1)
do
echo convert $i a $i_16bpp
file=$i$'_16bpp'
wine ../tools/imagenesToBpp/wlcd_img_gen/wlcd_img_gen.exe $i bpp/$file 16 -c

done
rm *.bmp

wine ../tools/bppToLcdBinaryGenerator/wfof_gen.exe bpp 4 ../include/wfof_data.h ../include/wfof_idxs.h -u
#cp ../include/* ../../../include/
