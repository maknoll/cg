files="frame_*.tga"
for i in $files
do
  newname=$(echo "$i" | sed -e 's/\.tga$/.png/')
  convert $i $newname
done
