// bmp-to-file.cc
// Usage:
//   bmp-to-file INPUT OUTPUT length
// Takes a bitmap file named INPUT and rips the 54-byte header out and
// appends it to the file OUTPUT. Additionally, if length is provided, 
// it truncates the output to be length bytes.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  // --------------------------------------
  // default parameters and input parsing
  // --------------------------------------

  const char* default_output_filename = "output";
  char* output_filename = const_cast<char*>(default_output_filename);
  int length = 0;

  if(argc<=1) {
    std::cout << "Please specify an input file! Usage:" << std::endl;
    std::cout << "  bmp-to-file INPUT OUTPUT length" << std::endl;
    return 1;
  }

  std::cout << "Parsing arguments..." << std::endl;

  char* input_filename = argv[1];

  if(argc>=3) {
    output_filename = argv[2];
  }

  if(argc==4) {
    length = atoi(argv[2]);

    if(length<=0) {
      std::cout << "Invalid length specified! Please specify a length greater than zero." << std::endl;
      return 1;
    }
  }

  if(argc>4) {
    std::cout << "Too many arguments specified!" << std::endl;
    return 1;
  }
  
  std::cout << "-Using input=" << input_filename << std::endl;
  std::cout << "-Using output=" << output_filename << std::endl;
  std::cout << "-Using length=" << length << std::endl;
  
  // -------------------------------------------------------
  // Open the input file and load it into the buffer
  // -------------------------------------------------------

  FILE* input_file_pointer;
  long input_filesize;
  char* input_file_buffer;
  size_t result;

  // open the file
  input_file_pointer = fopen(input_filename, "rb");
  if(input_file_pointer==NULL) {
    std::cout << "Unable to open the input file!" << std::endl;
    return 1;
  }

  // get the size
  fseek(input_file_pointer, 0, SEEK_END);
  input_filesize = ftell(input_file_pointer);
  rewind(input_file_pointer);


  // check the header information at 0x0E (tells how much remaining header)
  fseek(input_file_pointer, 0x0E, SEEK_CUR);
  int* remaining_header = (int*) malloc(sizeof(int));
  fread(remaining_header, 1, 1, input_file_pointer);
  std::cout << "-Remaining header size=" << remaining_header[0] << std::endl;
  int header_size = remaining_header[0]+0x0E;
  std::cout << "-Total header size=" << header_size << std::endl;

  // Move pointer position past header
  rewind(input_file_pointer);
  fseek(input_file_pointer, header_size, SEEK_CUR);

  // allocate memory to read the file
  input_file_buffer = (char*) malloc(sizeof(char)*(input_filesize-header_size));
  if(input_file_buffer == NULL) {
    std::cout << "Unable to allocate the input file slice!" << std::endl;
    return 1;
  }

  // copy the file into the input_file_buffer:
  result = fread(input_file_buffer,1,input_filesize-header_size,input_file_pointer);

  fclose(input_file_pointer);
  
  std::cout << "-Input file size=" << input_filesize << std::endl;

  // ---------------------------------------------------
  // Open the output file and start writing bitmap data
  // ---------------------------------------------------

  FILE *output_file_pointer;
  if(NULL==(output_file_pointer=fopen(output_filename, "a"))) {
    std::cout << "Unable to open the output file!" << std::endl;
    return 1;
  }
  
  //fseek(output_file_pointer, 1, SEEK_END);
  fwrite(input_file_buffer, 1, input_filesize-54, output_file_pointer);

  // Close files and free data buffers
  fclose(output_file_pointer);
  free(input_file_buffer);

  std::cout << output_filename << " successfully exported" << std::endl;
  std::cout << "File size    = " << ((double)(input_filesize-54))/1024.0/1024.0 << " MB" << std::endl;

  return 0;
}
