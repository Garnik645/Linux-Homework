#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#define BUFFER_SIZE 16

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		std::cerr << "No file to read" << std::endl;
		exit(1);
	}
	const char* filename1 = argv[1];
	const char* filename2 = argv[2];
	
	int src = open(filename1, O_RDONLY);
	if(src < 0)
	{
		std::cerr << "Something went wrong while opening the file. Error " << errno << std::endl;
		exit(errno);
	}
	
	int dst = open(filename2, O_WRONLY);
	if(dst < 0)
	{
		std::cerr << "Something went wrong while opening the file. Error " << errno << std::endl;
		exit(errno);
	}
	
	char* buffer = new char[BUFFER_SIZE + 1];
	
	while(true)
	{
		int readBytes = read(src, buffer, BUFFER_SIZE);
		
		if(readBytes < 0)
		{
			std::cerr << "Could not read from file due to error " << errno << std::endl;
			exit(errno);
		}
		
		if(readBytes == 0)
			break;
		
		int written = write(dst, buffer, readBytes);
		
		if(written < 0)
		{
			std::cerr << "Could not write buffer due to error " << errno << std::endl;
			exit(errno);
		}
	}
	
	delete [] buffer;
	
	close(src);
	close(dst);
	
	return 0;
}
