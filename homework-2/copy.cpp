#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <cerrno>

#define BUFFER_MAX_SIZE 128

struct File
{
	// file descriptor
	int fd;

	// true if file descriptor points to a real file
	bool flag;

	File() : fd(0), flag(false){}

	// open file
	File(const char* filename, mode_t md)
	: fd(open(filename, md))
	, flag(true)
	{
		// check if file was not opened due to error
		if(fd < 0)
		{
			std::cerr << "Something went wrong while opening the file. Error " << errno << std::endl;
			exit(errno);
		}
	}

	// open file
	File(const char* filename, mode_t md1, mode_t md2)
	: fd(open(filename, md1, md2))
	, flag(true)
	{
		// check if file was not opened due to error
		if(fd < 0)
		{
			std::cerr << "Something went wrong while opening the file. Error " << errno << std::endl;
			exit(errno);
		}
	}

	// move assignment
	File& operator=(File&& other)
	{
		fd = other.fd;
		if(other.flag == true)
		{
			flag = true;
			other.flag = false;
		}
		return *this;
	}

	// destructor
	~File()
	{
		// close file if it points to a real file
		if(flag)
		{
			close(fd);
		}
	}

	// create a hole inside a file
	void make_hole(off_t x)
	{
		lseek(fd, x, SEEK_END);
	}

	// return file logical size
	// set cursor at the end of the file
	off_t log_size() const
	{
		return lseek(fd, 0, SEEK_END);
	}

	// return file current cursor position
	off_t cursor_pos() const
	{
		return lseek(fd, 0, SEEK_CUR);
	}
};

void copy(const File& src, File& dst)
{
	// create a hole in destination with logical size of source
	dst.make_hole(src.log_size());

	// set the cursor at the begining of source
	off_t pos = lseek(src.fd, 0, SEEK_SET);

	bool flag = false;

	while(lseek(src.fd, 0, SEEK_DATA) >= 0)
	{
		pos = src.cursor_pos();
		off_t next_hole = lseek(src.fd, 0, SEEK_HOLE);
		if(next_hole < 0)
		{
			next_hole = lseek(src.fd, 0, SEEK_END) + 1;
			flag = true;
		}	
		lseek(src.fd, pos, SEEK_SET);
		while(src.cursor_pos() < next_hole)
		{
			off_t dif = next_hole - pos;
			int buff_size = std::max((int)dif, BUFFER_MAX_SIZE);
			char* buffer = new char[buff_size + 2];

			// read from source file into buffer
			int readBytes = read(src.fd, buffer, buff_size);
			
			// check if could not read due to error
			if(readBytes < 0)
			{
				std::cerr << "Could not read from file due to error " << errno << std::endl;
				exit(errno);
			}
			
			// write content of buffer into destination file
			int written = write(dst.fd, buffer, readBytes);
			
			// check if could not write due to error
			if(written < 0)
			{
				std::cerr << "Could not write buffer due to error " << errno << std::endl;
				exit(errno);
			}

			delete[] buffer;
		}
		if(flag)
		{
			break;
		}
	}
}

int main(int argc, char** argv)
{
	// check if source path argument was given
	if(argc < 2)
	{
		std::cerr << "No file to read" << std::endl;
		exit(1);
	}

	// open read-only first file
	File file1(argv[1], O_RDONLY);

	File file2;
	if(argc > 2)
	{
		// open write-only second file if it exists
		file2 = File(argv[2], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
	}
	else
	{
		// create write-only second file
		file2 = File("destination.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
	}
	
	// copy data and holes from the first file to second file
	copy(file1, file2);

	return 0;
}