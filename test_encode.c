#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

/* Check operation type */
OperationType check_operation_type(char *argv[])      //function to check whether encoding or decoding is to be performed
{
   if(strcmp(argv[1],"-e") == 0)    //check for encoding operation by checking the 1 st argument of the main function
   {
      return e_encode;     
   }
   else if(strcmp(argv[1],"-d") == 0)    //check for decoding operation by checking the 1 st argument of the main function
   {
      return e_decode;
   }
   else
   {
      return e_unsupported;      //return unsupported if neither encoding nor decoding
   }
}

int main( int argc , char *argv[] )
{
   if(argc == 1)
   {
      printf("ERROR in passing encode arguments !\n");
      return 0;
   }
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    int ret = check_operation_type(argv);    //call the function to check operation type
    if(ret == e_encode)    
    {
      if(argc >= 4)     //check for minimum number of arguments for encoding
      {
        int ret_e = read_and_validate_encode_args(argv,&encInfo);     //call the function to read and validate encode arguments
        {
            if(ret_e == e_success)     
            {
               if(do_encoding(&encInfo) == e_success)    //call the function to perform encoding
               {
                  printf("ENCODED SUCCESSFULLY\n");      //print success message if returned success from do_encoding function
                  return 0; 
               }
               else
               {
                  printf("ENCODING UNSUCCESSFUL\n");    //print failure message if returned failure from do_encoding function
                  return 0;
               }
            }
            else
            {
               printf("ERROR in passing encode arguments !\n ");    //print error message if encode arguments are invalid
               return 0;
            } 
        }
      } 
      else
      {
         printf("ERROR in passing encode arguments !\n");      //print error message if insufficient arguments for encoding
         return 0;
      } 
    }
    else if(ret == e_decode)
    {
      if(argc >=3)
      {
         int ret1 = read_and_validate_decode_args(argv,&decInfo);    //call the function to read and validate decode arguments
         if(ret1 == e_success) 
         {
           if(do_decoding(&decInfo) == e_success)
           {
              printf("DECODING SUCCESSFULL\n");    //print success message if returned success from do_decoding function
              return 0; 
           }
           else
           {
               printf("DECODING UNSUCESSFULL\n");    //print failure message if returned failure from do_decoding function
               return 0;
           }
         }
         else
         {
            printf("ERROR in passing decode arguments !\n");      //print error message if decode arguments are invalid
            return 0;
         }
      }
      else
      {
         printf("ERROR in passing decode arguments !\n");      //print error message if insufficient arguments for decoding
         return 0;
      }
    }
    else
    {
        printf("ERROR in passing arguments !\n");    //print error message if operation type is unsupported
        return 0;
    }
    

}
