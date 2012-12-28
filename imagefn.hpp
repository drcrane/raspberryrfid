#ifndef __IMAGEFN_HPP
#define __IMAGEFN_HPP

extern "C" {
int get_jpeg_size(char *filename, unsigned long int *width, unsigned long int *height);
int decode_jpeg_from_buffer(void ** rawbuffer, unsigned long int ** height, unsigned long int ** width, char * jpegbuffer, unsigned long int jpegbuffersz);
}

#endif // __IMAGEFN_HPP
