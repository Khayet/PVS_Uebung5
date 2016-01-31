all: compile

compile:
	gcc -I. .\matmult.cpp C:\Windows\System32\OpenCl.dll -lstdc++ -std=c++11 -o .\matmult.exe
