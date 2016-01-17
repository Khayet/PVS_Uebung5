all: compile

compile:
	gcc -I. .\hello_world.cpp C:\Windows\System32\OpenCl.dll -lstdc++ -o .\hello_world.exe
