clear
echo "Builing Vector Vertex"
./shader_compile.sh && cmake .. && make -j6 && ./VectorVertex 
