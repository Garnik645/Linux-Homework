#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <cerrno>

#define BUFFER_SIZE 16

int main(int argc, char** argv)
{
	// check if we have right amount of arguments to read and write to
	if(argc < 2)
	{
		std::cerr << "No file to read" << std::endl;
		exit(1);
	}

	// get input source file path
	const char* filename1 = argv[1];

	// open source file only for reading
	int src = open(filename1, O_RDONLY);
	
	// check if source file was not opened due to error
	if(src < 0)
	{
		std::cerr << "Something went wrong while opening the source file. Error " << errno << std::endl;
		exit(errno);
	}
	
	// variable for destination file descriptor
	int dst;

	// open file if destination path was given
	if(argc > 2)
	{
		// get input destination file path
		const char* filename2 = argv[2];

		// open destination file only for writing
		dst = open(filename2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
		
		// check if file was not opened due to error
		if(dst < 0)
		{
			std::cerr << "Something went wrong while opening the destination file. Error " << errno << std::endl;
			exit(errno);
		}
	}
	// if not create destination file
	else
	{
		// create destination.txt file
		dst = creat("destination.txt", S_IRUSR | S_IWUSR);

		// check if file was not created due to error
		if(dst < 0)
		{
			std::cerr << "Something went wrong while creating the destination file. Error " << errno << std::endl;
			exit(errno);
		}
	}

	// allocate a buffer to read from file
	char* buffer = new char[BUFFER_SIZE + 1];
	
	// start copying the file
	while(true)
	{
		// read BUFFER_SIZE bytes from source file into buffer
		int readBytes = read(src, buffer, BUFFER_SIZE);
		
		// check if could not read due to error
		if(readBytes < 0)
		{
			std::cerr << "Could not read from file due to error " << errno << std::endl;
			exit(errno);
		}
		
		// stop reading because reached the end
		if(readBytes == 0)
			break;
		
		// write content of buffer into destination file
		int written = write(dst, buffer, readBytes);
		
		// check if could not write due to error
		if(written < 0)
		{
			std::cerr << "Could not write buffer due to error " << errno << std::endl;
			exit(errno);
		}
	}

	// delete allocated memory	
	delete[] buffer;

	// close source and destination files		
	close(src);
	close(dst);

	return 0;
}
