clear
echo "Builing Vector Vertex"
./shader_compile.sh && cmake .. && make -j5
paplay /usr/share/sounds/ubuntu/notifications/Mallet.ogg
 ./VectorVertex 
