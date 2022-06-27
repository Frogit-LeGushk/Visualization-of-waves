run: main.cpp
	g++ main.cpp -lglfw -lGL -o main && ./main

clean: main
	-rm main 2/>dev/null
