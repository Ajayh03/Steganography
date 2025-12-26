#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include<stdio.h>


#define MAX_SECRET_BUF_SIZE 1 
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 5

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *output_image_fname;       //output image file name
    FILE *fptr_output_image;        //file pointer to output image file

    /* Secret File Info */
    char secret_name[50];       //array to store secret file name
    char *output__file_fname;   //address of output file name
    FILE *fptr_output_file;     //file pointer to output file
    char extn_secret_file[MAX_FILE_SUFFIX];     //array to store secret file extension
    char secret_data[MAX_SECRET_BUF_SIZE];      //array to store secret data
    long size_secret_file;      //variable to store secret file size
    int size_secret_file_extn;      //variable to store secret file extension size


} DecodeInfo;


/* Encoding function prototype */


/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decoding(DecodeInfo *decInfo);

/* Copy bmp image header */
Status skip_bmp_header(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Encode secret file extenstion size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn(/*const char *file_extn, */DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode function, which does the real encoding */
Status decode_int_from_lsb(int *size, char *image_buffer);

/* Encode a byte into LSB of image data array */
Status decode_bytes_from_lsb(char *data, char *image_buffer);

/* Function to check and open file with decoded extension */
Status extension_check_file_open(DecodeInfo *decInfo);
#endif