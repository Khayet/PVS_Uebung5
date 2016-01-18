all: compile

compile:
	gcc -I. .\matmult.cpp C:\Windows\System32\OpenCl.dll -lstdc++ -o .\matmult.exe
