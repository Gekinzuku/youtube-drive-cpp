// file-to-bmp.cc
// Usage:
//   file-to-bmp INPUT width height slice OUTPUT
// Takes a file named INPUT and converts its raw data into a bitmap 
// image of size width-by-height. However, since the data may not necessarily
// be exactly size width*height, slice specifies the window of data to write 
// to the bitmap

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  // --------------------------------------
  // default parameters and input parsing
  // --------------------------------------

  int width = 1024;
  int height = 768;
  int slice = 0;
  const char* default_output_filename = "output.bmp";
  char* output_filename = const_cast<char*>(default_output_filename);

  if(argc<=1) {
    std::cout << "Please specify an input file! Usage:" << std::endl;
    std::cout << "  file-to-bmp INPUT width height slice OUTPUT" << std::endl;
    return 1;
  }

  std::cout << "Parsing arguments..." << std::endl;

  char* input_filename = argv[1];
  if(argc>=4) {
    width = atoi(argv[2]);
    height = atoi(argv[3]);

    if(width<=0) {
      std::cout << "Invalid width specified! Please specify a width greater than zero." << std::endl;
      return 1;
    }

    if(height<=0) {
      std::cout << "Invalid height specified! Please specify a height greater than zero." << std::endl;
      return 1;
    }
  }

  if(argc>=5) {
    slice = atoi(argv[4]);

    if(slice<0) {
      std::cout << "Invalid slice specified! Please specify a slice of zero or greater." << std::endl;
      return 1;
    }
  }

  if(argc==6) {
    output_filename = argv[5];
  }

  if(argc>6) {
    std::cout << "Too many arguments specified!" << std::endl;
    return 1;
  }
  const int total_pixels = width*height;

  std::cout << "-Using input=" << input_filename << std::endl;
  std::cout << "-Using width=" << width << std::endl;
  std::cout << "-Using height=" << height << std::endl;
  std::cout << "-Using slice=" << slice << std::endl;
  std::cout << "-Using output=" << output_filename << std::endl;

  int datasize = 3*total_pixels;   // number of RGB entries for the image
  int filesize = 54 + datasize;    // file size in bytes (3bytes per pixel for 256bit)

  // -------------------------------------------------------
  // Open the input file and load the slice into the buffer
  // -------------------------------------------------------

  FILE* input_file_pointer;
  long input_filesize;
  char* input_file_slice;
  size_t result;

  input_file_pointer = fopen(input_filename, "rb");
  if(input_file_pointer==NULL) {
    std::cout << "Unable to open the input file!" << std::endl;
    return 1;
  }

  // obtain file size:
  fseek(input_file_pointer, 0, SEEK_END);
  input_filesize = ftell(input_file_pointer);
  rewind(input_file_pointer);

  // allocate memory to contain the whole file:
  input_file_slice = (char*) malloc(sizeof(char)*datasize);
  if(input_file_slice == NULL) {
    std::cout << "Unable to allocate the input file slice!" << std::endl;
    return 1;
  }

  fseek(input_file_pointer, slice*datasize, SEEK_CUR);

  if(slice*datasize > input_filesize) {
    std::cout << "Slice " << slice << " is out of bounds!" << std::endl;
    return 1;
  }

  // copy the file into the input_file_slice:
  size_t read_size = (size_t) std::min((long)datasize, std::max(input_filesize-slice*datasize,(long)0));
  std::cout << "-Read size=" << read_size << std::endl;
  result = fread(input_file_slice,1,read_size,input_file_pointer);

  // if(result != input_filesize && result != datasize) {
  //   std::cout << "Unable to read the input file!" << std::endl;
  //   std::cout << "Result=" << result << std::endl;
  //   return 1;
  // }

  fclose(input_file_pointer);
  
  std::cout << "-Input file size=" << input_filesize << std::endl;
  std::cout << "-Slice size=" << total_pixels << std::endl;

  // ---------------------------------------------------
  // Open the output file and start writing bitmap data
  // ---------------------------------------------------

  FILE *output_file_pointer;
  if(NULL==(output_file_pointer=fopen(output_filename, "w"))) {
    std::cout << "Unable to open the output file!" << std::endl;
    return 1;
  }

  // Define the header information
  unsigned char bminput_file_pointerheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
  unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};

  bminput_file_pointerheader[ 2] = (unsigned char)(filesize    ); // write the bytes in reverse order
  bminput_file_pointerheader[ 3] = (unsigned char)(filesize>> 8);
  bminput_file_pointerheader[ 4] = (unsigned char)(filesize>>16);
  bminput_file_pointerheader[ 5] = (unsigned char)(filesize>>24);

  bmpinfoheader[ 4] = (unsigned char)(    width    );
  bmpinfoheader[ 5] = (unsigned char)(    width>> 8);
  bmpinfoheader[ 6] = (unsigned char)(    width>>16);
  bmpinfoheader[ 7] = (unsigned char)(    width>>24);
  bmpinfoheader[ 8] = (unsigned char)(   height    );
  bmpinfoheader[ 9] = (unsigned char)(   height>> 8);
  bmpinfoheader[10] = (unsigned char)(   height>>16);
  bmpinfoheader[11] = (unsigned char)(   height>>24);
  bmpinfoheader[20] = (unsigned char)(datasize    );
  bmpinfoheader[21] = (unsigned char)(datasize>>8 );
  bmpinfoheader[22] = (unsigned char)(datasize>>16);
  bmpinfoheader[23] = (unsigned char)(datasize>>24);


  // Output the header information together
  fwrite(bminput_file_pointerheader,1,14,output_file_pointer);
  fwrite(bmpinfoheader,1,40,output_file_pointer);
  // Write the pixel array data
  fwrite(input_file_slice, 1, datasize, output_file_pointer);

  // Close files and free data buffers
  fclose(output_file_pointer);
  free(input_file_slice);

  std::cout << output_filename << " successfully exported" << std::endl;
  std::cout << "Image size   = " << width << "x" << height << std::endl;
  std::cout << "File size    = " << ((double)filesize)/1024.0/1024.0 << " MB" << std::endl;

  return 0;
}
