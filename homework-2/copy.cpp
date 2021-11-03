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
	bool real;

	File() : fd(0), real(false){}

	// open file
	File(const char* filename, mode_t md)
	: fd(open(filename, md))
	, real(true)
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
	, real(true)
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
		// get file descriptor if this file doesn't point to a real file
		if(other.real && !real)
		{
			fd = other.fd;
			real = true;
			other.real = false;
		}
		return *this;
	}

	// return logical size of file
	off_t log_size()
	{
		// save current cursor position
		off_t temp = lseek(fd, 0, SEEK_CUR);

		// get file logical size
		off_t size = lseek(fd, 0, SEEK_END);

		// return curser to its previous position
		lseek(fd, temp, SEEK_SET);

		// return logical size
		return size;
	}

	// return phisical size of file
	off_t ph_size()
	{
		// save current cursor position
		off_t temp = lseek(fd, 0, SEEK_CUR);

		// variable to store size of file
		off_t size = 0;

		// move cursor to the start of data in source
		off_t last = lseek(fd, 0, SEEK_DATA);

		// go over file
		while(true){

			// try jump to next beginning of hole
			off_t data = lseek(fd, last, SEEK_HOLE);

			// there was some data if we got to next hole
			if(data > 0)
			{
				size += data - last;
				last = data;
			}

			// reached the end of file
			if(data == 0){
				break;
			}

			// something went wrong
			if(data < 0){
				std::cerr << "Something went wrong. Error " << errno << std::endl;
				exit(errno);
			}

			// try jump to next beginning of data
			last = lseek(fd, last, SEEK_DATA);

			// reached the end of file
			if(last == -1 && errno == ENXIO){
				break;
			}

			// something went wrong
			if(last < 0){
				std::cerr << "Something went wrong. Error " << errno << std::endl;
				exit(errno);
			}
		}

		// return curser to its previous position
		lseek(fd, temp, SEEK_SET);

		return size;
	}

	// destructor
	~File()
	{
		// close file if it points to a real file
		if(real)
		{
			close(fd);
		}
	}
};

// function to copy data and holes from the source file into destination
void copy(const File& src, File& dst)
{
	// create a hole in destination with logical size of source
	lseek(dst.fd, lseek(src.fd, 0, SEEK_END), SEEK_END);

	// move cursor to the start of data in source
	int last = lseek(src.fd, 0, SEEK_DATA);

	// go over file
    while(true){

        // try jump to next beginning of hole
        int data = lseek(src.fd, last, SEEK_HOLE);

        // there was some data if we got to next hole
        if(data > 0)
		{
			// set the cursor at last
			lseek(src.fd, last, SEEK_SET);
			lseek(dst.fd, last, SEEK_SET);

			// allocate buffer
			int buffer_size = std::min(data - last, BUFFER_MAX_SIZE);
			char* buffer = new char[buffer_size];

			// go over this segment of data
			int i = last;
			while(i < data)
			{
				// read from source
				int rd_bytes = read(src.fd, buffer, buffer_size);

				// check if could not read due to error
				if(rd_bytes < 0)
				{
					std::cerr << "Something went wrong. Error " << errno << std::endl;
					exit(errno);
				}

				// write into destination
				int wt_bytes = write(dst.fd, buffer, buffer_size);

				// check if could not write due to error
				if(wt_bytes < 0)
				{
					std::cerr << "Something went wrong. Error " << errno << std::endl;
					exit(errno);
				}

				i += buffer_size;
			}

			last = data;

			// delete allocated memory
			delete[] buffer;
        }

		// reached the end of file
        if(data == 0){
            break;
        }

		// something went wrong
        if(data < 0){
            std::cerr << "Something went wrong. Error " << errno << std::endl;
            exit(errno);
		}

		// try jump to next beginning of data
		last = lseek(src.fd, last, SEEK_DATA);

		// reached the end of file
		if(last == -1 && errno == ENXIO){
            break;
        }

        // something went wrong
        if(last < 0){
            std::cerr << "Something went wrong. Error " << errno << std::endl;
            exit(errno);
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

	std::cout << "Logical size of source: " << file1.log_size() << std::endl;
	std::cout << "Physical size of source: " << file1.ph_size() << std::endl;

	// copy file1 to file2
	copy(file1, file2);

	return 0;
}