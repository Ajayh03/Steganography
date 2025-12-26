#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image,EncodeInfo *encInfo)
{
    uint width, height;
    // Seek to 18th byte 
    fseek(fptr_image, 18, SEEK_SET);       

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    
    //store image capacity in EncodeInfo structure which is RGB bytes + header size +EOF
    encInfo->image_capacity = (height * width * 3)+55;     


    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");     //open source image file in read mode
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");       //open secret file in read mode
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");     //open stego image file in write mode
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)     //read and validate the encode arguments 
{
    if(argv[2][0] != '.')       //check for source image file name and check for atleast 1 character before .
    {
        char *ret_val = strstr(argv[2],".bmp");
        if(ret_val != NULL && strlen(ret_val) == 4)      //check for .bmp extension
        {
            encInfo->src_image_fname = argv[2]; 
        }
        else
        {
            return e_failure;       //return failure if not .bmp
        }
    }
    else
    {
        return e_failure;       //return failure if first character is .
    }

    if(argv[3][0] != '.')       //check for secret file name and check for atleast 1 character before .
    {
        if(strstr(argv[3],".bmp") || strstr(argv[3],".c") || strstr(argv[3],".txt") || strstr(argv[3],".sh") || strstr(argv[3],".h"))       //check for valid extensions
        {
            encInfo->secret_fname = argv[3];    //assign secret file name
            char *str;
            str = strchr(encInfo->secret_fname,'.');
            strcpy(encInfo->extn_secret_file,str);      //copy the extension of secret file          
        }
        else
        {
            return e_failure;       //return failure if invalid extension
        }
    }
    else
    {
        return e_failure;       //return failure if first character is .
    }

    if(argv[4] != NULL)     //check for stego image file name
    {
        if(argv[4][0] != '.')       //check for atleast 1 character before .
        {
            char *ret_val1 = strstr(argv[4],".bmp");
            if(ret_val1 != NULL && strlen(ret_val1)==4)      //check for .bmp extension
            {
                encInfo->stego_image_fname = argv[4];       //assign stego image file name    
            }
            else
            {
                return e_failure;    //return failure if not .bmp   
            }
        }
        else
        {
            return e_failure;       //return failure if first character is .
        }
    }
    else    //if stego image file name is not provided
    {
        encInfo->stego_image_fname = "default.bmp";     //assign default stego image file name as default.bmp
    }
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)     //copy bmp header from source image file to destination image file
{
    char header[54];    //array to store bmp header
    rewind(fptr_src_image);     //set file position to beginning of source image file
    rewind(fptr_dest_image);    //set file position to beginning of destination image file

    fread(header,1,54,fptr_src_image);      //read bmp header from source image file
    fwrite(header,1,54,fptr_dest_image);    //write bmp header to destination image file    
    if(ftell(fptr_src_image) ==  ftell(fptr_dest_image))
    {
         return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)       //encode the magic string into lsb of image bytes
{
    char magic[8];      //array to store 8 bytes from image file for encoding magic string
    for(uint j=0;j<strlen(magic_string);j++)
    {
        char ch = magic_string[j];          //get each character from magic string
        fread(magic,1,8,encInfo->fptr_src_image);       //read 8 bytes from source image file
        encode_byte_to_lsb(ch,magic);       //encode the character to lsb of 8 bytes
        fwrite(magic,1,8,encInfo->fptr_stego_image);        //write the encoded bytes to stego image file
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)        //function for encoding a byte to lsb of 8 bytes from image file
{
  int i;
  for(i=7;i>=0;i--)
  {
     image_buffer[7-i] = image_buffer[7-i] & (~1);       //clear the least significant bit of each byte 
     if((data & (1<<i)) !=0)        //check if the ith bit of data is 1
     {
       image_buffer[7-i] = image_buffer[7-i] | 1;       //set the least significant bit to 1
     }
  }
  return e_success;
}

Status encode_int_to_lsb(int value, char *image_buffer)      //function for encoding an integer to lsb of 32 bytes from image file
{
  int i;
  for(i=31;i>=0;i--)
  {
     image_buffer[31-i] = image_buffer[31-i] & (~1);        //same as above function with 32 bytes as it is integer
     if((value & (1<<i)) !=0)
     {
       image_buffer[31-i] = image_buffer[31-i] | 1;
     }
  }
  return e_success;
}

Status check_capacity(EncodeInfo *encInfo)      //function to check if the image has enough capacity to hold the secret data
{  
   int check_ret = get_image_size_for_bmp(encInfo->fptr_src_image,encInfo);     //get image size for source bmp file
   int extnlen = strlen(encInfo->extn_secret_file);     //get length of secret file extension
   int capacity =((strlen(MAGIC_STRING) + sizeof(int) + extnlen + sizeof(int) + get_file_size(encInfo->fptr_secret)) * 8 )+ 54;     //calculate the required capacity as magic string + secret file extension size + secret file size + secret file data size in bits + bmp header size
   if(check_ret >= capacity)        //check if image size is greater than or equal to required capacity
   {
    return e_success;
   }
   else
   {
    return e_failure;
   }
}

uint get_file_size(FILE *fptr)      //function to get the size of a secret file
{
    fseek(fptr,0,SEEK_END);         //set file position to end of file
    int off_pos = ftell(fptr);      //get the offset position which is equal to size of file      
    return off_pos;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)      //function to encode the secret file extension size 
{
  char arr[size];       
  fread(arr,1,32,encInfo->fptr_src_image);       //read 32 bytes from source image file
  encode_int_to_lsb(strlen(encInfo->extn_secret_file),arr);     //encode the length of secret file extension to lsb of 32 bytes
  fwrite(arr,1,32,encInfo->fptr_stego_image);   //write the encoded bytes to stego image file
  
  return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)      //function to encode the secret file extension
{
   char secret_ext[8];      //array to store 8 bytes from image file for encoding secret file extension
   for(uint i=0;i<strlen(file_extn);i++)
   {
     fread(secret_ext,8,1,encInfo->fptr_src_image);     //read 8 bytes from source image file
     encode_byte_to_lsb(file_extn[i],secret_ext);       //encode each character of secret file extension to lsb of 8 bytes
     fwrite(secret_ext,8,1,encInfo->fptr_stego_image);   //write the encoded bytes to stego image file
   }
   
    if(ftell(encInfo->fptr_src_image) ==  ftell(encInfo->fptr_stego_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    } 
    
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)     //function to encode the secret file size
{
    char file_size_arr[32];     //array to store 32 bytes from image file for encoding secret file size
    fread(file_size_arr,32,1,encInfo->fptr_src_image);      //read 32 bytes from source image file
    encode_int_to_lsb(file_size,file_size_arr);     //encode the secret file size to lsb of 32 bytes    
    fwrite(file_size_arr,32,1,encInfo->fptr_stego_image);       //write the encoded bytes to stego image file
    if(ftell(encInfo->fptr_src_image) ==  ftell(encInfo->fptr_stego_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_data(EncodeInfo *encInfo)     //function to encode the secret file data
{
   rewind(encInfo->fptr_secret);    
   char secret_arr[8];      //array to store 8 bytes from image file for encoding secret file data
   encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);     
   rewind(encInfo->fptr_secret);        //set file position to beginning of secret file, which is needed to read the secret file data
   char ch;
   for(int i=0;i<encInfo->size_secret_file;i++)     //loop to read each character from secret file
   {
    fread(&ch,1,1,encInfo->fptr_secret);    //read a character from secret file        
    fread(secret_arr,8,1,encInfo->fptr_src_image);      //read 8 bytes from source image file
    encode_byte_to_lsb(ch,secret_arr);     //encode the character to lsb of 8 bytes
    fwrite(secret_arr,8,1,encInfo->fptr_stego_image);       //write the encoded bytes to stego image file
   } 
    if(ftell(encInfo->fptr_src_image) ==  ftell(encInfo->fptr_stego_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }

}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest,EncodeInfo *encInfo)     //function to copy the remaining image data from source image file to stego image file for getting the same size and image as source image file
{
   uint src_pos1 = ftell(fptr_src);     //get current file position of source image file
   uint  src_pos2 = encInfo->image_capacity;        //get the total size of source image file which is stored in image_capacity variable
   uint size = src_pos2 - src_pos1;     //calculate the remaining size to be copied
   char arr[size];          //array to store the remaining bytes from source image file

    fread(arr,1,size,fptr_src);     //read the remaining bytes from source image file
    fwrite(arr,1,size,fptr_dest);       //write the remaining bytes to stego image file

   
    if(ftell(fptr_src) ==  ftell(fptr_dest))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
   
}


Status do_encoding(EncodeInfo *encInfo)     //function to perform the encoding process by calling all the necessary functions
{
    if(open_files(encInfo) == e_success)
    {
       if(check_capacity(encInfo) == e_success)
       {
           if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
           {
               if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
               {
                   if(encode_secret_file_extn_size(32, encInfo) == e_success)
                   {
                       if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
                       {
                           if(encode_secret_file_size(get_file_size(encInfo->fptr_secret), encInfo)== e_success)
                           {
                              if(encode_secret_file_data(encInfo) == e_success)
                              {
                                  if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo) == e_success)
                                  {
                                    fclose(encInfo->fptr_stego_image);       //close the stego image file
                                    fclose(encInfo->fptr_secret);           //close the secret file
                                    fclose(encInfo->fptr_src_image);       //close the source image file
                                    return e_success;       //return success if all functions are executed successfully and return to main function where do decoding is called
                                  }
                                  else
                                  {
                                    return e_failure;
                                  }
                              }
                              else
                              {
                                  return e_failure;
                              }
                           }
                           else
                           {
                              return e_failure;
                           } 
                       }
                       else
                       {
                        return e_failure;
                       }
                   }
                   else
                   {
                   return e_failure;
                   }
               }
               else
               {
                return e_failure;
               }
           }
           else
           {
            return e_failure;
           }
       }
       else
       {
        return e_failure;
       }  
    }
    else
    {
        return e_failure;
    }
   
}
