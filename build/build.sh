clear
echo "Builing Vector Vertex"
./shader_compile.sh && cmake .. && make -j5 && ./VectorVertex 
