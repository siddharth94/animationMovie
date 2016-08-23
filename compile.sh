cd scene0
./compiler.sh
cd ../scene1
g++ lesson20.cpp -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -I/usr/include/SDL -lGLU -lGL -g
cd ../scene2_start
./compile_linux.sh
cd ../scene2_end
g++ lesson20.cpp -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -I/usr/include/SDL -lGLU -lGL -g
cd ../scene3
g++ lesson20.cpp -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -I/usr/include/SDL -lGLU -lGL -g
cd ../scene4_wa
g++ lesson20.cpp -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -I/usr/include/SDL -lGLU -lGL -g
cd ../final
g++ lesson20.cpp -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -I/usr/include/SDL -lGLU -lGL -g
cd ../post_final
g++ lesson20.cpp -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -I/usr/include/SDL -lGLU -lGL -g