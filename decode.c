#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)     //read and validate the decode arguments
{
    if(argv[2][0] != '.')   //check for output image file name and check for atleast 1 character before .
    {
       char *val_d = strstr(argv[2],".bmp");
       if(val_d != NULL && strlen(val_d) == 4)   //check for .bmp extension
       {
            decInfo->output_image_fname = argv[2];   //assign output image file name which is given as input for decoding
       }
       else
       {
            return e_failure;
       }
    }
    else
    {
        return e_failure;   //return failure if first character is .
    }

    if(argv[3] != NULL)     //check whether secret file name is provided
    {
        if(argv[3][0] != '.')   //check for atleast 1 character before .
        {
           strcpy(decInfo->secret_name,argv[3]);    //copy the secret file name to secret_name array
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        decInfo->secret_name[0]='\0';   //if secret file name is not provided, assign null to first index of secret_name array
    }

    
    return e_success;
}

Status open_files_decoding(DecodeInfo *decInfo)
{
    decInfo->fptr_output_image = fopen(decInfo->output_image_fname,"r");        //open the image file in read mode

    if (decInfo->fptr_output_image == NULL)     //check for NULL file pointer
    {
    	perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo->output_image_fname);

    	return e_failure;
    }
    return e_success;
}

Status skip_bmp_header(DecodeInfo *decInfo)     //function to skip the bmp header of 54 bytes
{
    fseek(decInfo->fptr_output_image,54,SEEK_SET);   //skip the first 54 bytes of bmp header
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)    //function to decode the magic string from lsb of image bytes
{   
   uint i;
   char magic;      //variable to store each character of magic string
   char arr[8];     //array to store 8 bytes from image file for decoding magic string
   char magic_arr[5];   //array to store the decoded magic string
   for(i=0;i<strlen(magic_string);i++)
   {
    fread(arr,8,1,decInfo->fptr_output_image);      //read 8 bytes from output image file
    decode_bytes_from_lsb(&magic,arr);      //decode each character of magic string from lsb of 8 bytes
    magic_arr[i] = magic;           //store the decoded character in magic_arr array
   }
   magic_arr[i] = '\0';
   if(strcmp(magic_arr,magic_string) == 0)      //compare the decoded magic string with original magic string only if they are same return success that is to check it is the correct encoded image file
   {
    return e_success;
   }
   else
   {
    return e_failure;
   }
}

Status decode_bytes_from_lsb(char *ascii, char *image_buffer)       //function for decoding a byte from lsb of 8 bytes from image file
{
  *ascii=0;    
  int sum;
  for(int i=0;i<8;i++)      //loop to read each byte
  {
    image_buffer[i] = image_buffer[i] & 1;      //extract the least significant bit of each byte
    if(image_buffer[i] == 1)      //check if the lsb is 1
    {
      sum = 1;     
      for(int j=0;j < (7-i);j++)       //loop to calculate the ascii value of the byte     
      {
        sum = sum*2;
      }
       *ascii = *ascii + sum;
    }    

  }
  return e_success;

}

Status decode_int_from_lsb(int *size,char *image_buffer)    //function for decoding an integer from lsb of 32 bytes from image file
{
  *size=0;
  int sum;
  for(int i=0;i<32;i++)
  {
    image_buffer[i] = image_buffer[i] & 1;
    if(image_buffer[i] == 1)
    {
      sum = 1;
      for(int j=0;j < (31-i);j++)           //same as above function with 32 bytes as it is integer
      {
        sum = sum*2;
      }
       *size = *size + sum;
    }  
  }
  
  return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)        //function to decode the secret file extension size
{
    int size = 0;
    char arr[32];
    fread(arr,1,32,decInfo->fptr_output_image);     //read 32 bytes from output image file
    decode_int_from_lsb(&size,arr);    //decode the secret file extension size from lsb of 32 bytes 
    decInfo->size_secret_file_extn = size;      //store the decoded size in size_secret_file_extn variable
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)     //function to decode the secret file extension
{
    char extn[decInfo->size_secret_file_extn];      //array to store bytes from image file for decoding secret file extension   
    char ch;
    int i;
    for(i=0;i<decInfo->size_secret_file_extn;i++)
    {
        fread(extn,1,8,decInfo->fptr_output_image);     //read 8 bytes from output image file
        decode_bytes_from_lsb(&ch,extn);    //decode each character of secret file extension from lsb of 8 bytes    
        decInfo->extn_secret_file[i] = ch;      //store the decoded character in extn_secret_file array
    } 

    return e_success;
}

Status extension_check_file_open(DecodeInfo *decInfo)       //function to check and open file with decoded extension
{
    if(decInfo->secret_name[0] != '\0')     //check whether secret file name is provided
    {
       char *add = strchr(decInfo->secret_name,'.');    //get the address of . in the secret file name
       if(add != NULL)
       {
            strcpy(add,decInfo->extn_secret_file);   //copy the decoded extension to the secret file name
            decInfo->output__file_fname = decInfo->secret_name;  //point secret file name to output__file_fname
       }
       else
       {
            strcat(decInfo->secret_name,decInfo->extn_secret_file);  
            decInfo->output__file_fname = decInfo->secret_name;    
       }
    }
    else
    {
        strcpy(decInfo->secret_name, "secretde");     //copy default secret file name "secretde" to secret_name array
        strcat(decInfo->secret_name,decInfo->extn_secret_file);  //concatenate the decoded extension to default secret file name
        decInfo->output__file_fname = decInfo->secret_name;     //point secret file name to output__file_fname
    }

    decInfo->fptr_output_file = fopen(decInfo->output__file_fname,"w");    //open the output file in write mode

    if ( decInfo->fptr_output_file == NULL )    //check for NULL file pointer   
    {
        printf("file not created \n");
        return e_failure;
    }
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)     //function to decode the secret file size
{
    char sec_f_size[32];
    int value=0;
    fread(sec_f_size,1,32,decInfo->fptr_output_image);      //read 32 bytes from output image file
    decode_int_from_lsb(&value,sec_f_size);     //decode the secret file size from lsb of 32 bytes
    decInfo->size_secret_file = value;      //store the decoded size in size_secret_file variable
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)     //function to decode the secret file data
{
    char arr[8];
    char ch;
    for(int i=0;i<decInfo->size_secret_file;i++)
    {
        fread(arr,1,8,decInfo->fptr_output_image);      //read 8 bytes from output image file
        decode_bytes_from_lsb(&ch,arr);     //decode each character of secret file data from lsb of 8 bytes
        fprintf(decInfo->fptr_output_file,"%c",ch);     //write the decoded character to output file
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)   //function to perform the decoding process by calling all the necessary functions 
{
    if(open_files_decoding(decInfo) == e_success)
    {
        if(skip_bmp_header(decInfo) == e_success)
        {
            if(decode_magic_string(MAGIC_STRING,decInfo) == e_success)
            {
                if(decode_secret_file_extn_size(decInfo) == e_success)
                {
                   if(decode_secret_file_extn(decInfo) == e_success)
                   {
                       if(extension_check_file_open(decInfo) == e_success)
                       {
                           if(decode_secret_file_size(decInfo) == e_success)
                           {
                              if(decode_secret_file_data(decInfo) == e_success)
                              {
                                    fclose(decInfo->fptr_output_file);      //close the output file pointer
                                    fclose(decInfo->fptr_output_image);     //close the output image file pointer
                                    return e_success;               //return success if all functions are executed successfully
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
