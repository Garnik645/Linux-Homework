#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <cerrno>

#define BUFFER_SIZE 16

// struct for storing source and destination file descriptor
struct copyfd
{
	int src;
	int dst;

	// open files and get file descriptor
	copyfd(int margc, char** margv)
	{
		// check if source path argument was given
		if(margc < 2)
		{
			std::cerr << "No file to read" << std::endl;
			exit(1);
		}

		// get input source file path
		const char* filename1 = margv[1];

		// open source file only for reading
		src = open(filename1, O_RDONLY);
		
		// check if source file was not opened due to error
		if(src < 0)
		{
			std::cerr << "Something went wrong while opening the source file. Error " << errno << std::endl;
			exit(errno);
		}

		// open file if destination path was given
		if(margc > 2)
		{
			// get input destination path
			const char* filename2 = margv[2];

			// open destination file only for writing, delete its content
			// if it doesn't exist, create one
			dst = open(filename2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
			
			// check if file was not opened due to error
			if(dst < 0)
			{
				std::cerr << "Something went wrong while opening the destination file. Error " << errno << std::endl;
				exit(errno);
			}
		}
		// if not, create destination file
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
	}

	// close source and destination files
	~copyfd()
	{
		close(src);
		close(dst);
	}
};

int main(int argc, char** argv)
{
	// get and store file descriptor of source and destination files
	copyfd fd(argc, argv);

	// allocate a buffer to read from file
	char* buffer = new char[BUFFER_SIZE + 1];
	
	// start copying the file
	while(true)
	{
		// read BUFFER_SIZE bytes from source file into buffer
		int readBytes = read(fd.src, buffer, BUFFER_SIZE);
		
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
		int written = write(fd.dst, buffer, readBytes);
		
		// check if could not write due to error
		if(written < 0)
		{
			std::cerr << "Could not write buffer due to error " << errno << std::endl;
			exit(errno);
		}
	}

	// delete allocated memory	
	delete[] buffer;

	return 0;
}
