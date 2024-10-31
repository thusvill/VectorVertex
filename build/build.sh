clear
echo "Builing Vector Vertex"
./shader_compile.sh && cmake .. && make -j3 
paplay /usr/share/sounds/ubuntu/notifications/Mallet.ogg
 ./VectorVertex 
