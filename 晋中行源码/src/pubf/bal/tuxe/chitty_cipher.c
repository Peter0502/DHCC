/* DES.C modified by lewis */
/* Define SCO_UNIX , PUMA_UNIX */

#include <stdio.h>    
#include <string.h> 
#include <stdlib.h>
#include <time.h> 

#ifndef PUMA_UNIX
#define PUMA_UNIX
#endif

#ifdef SCO_UNIX
#undef SCO_UNIX
#endif

unsigned short ONE= 1; 

/*------------------------------------------------------------------------------
  -  procedure get_bit:                                                     
  -          Get a bit that be indicated by position from an array             
  -  --------------------------------------------------------------------------
  -  input:  unsigned short array[4] - will be selected a bit                   
  -          int  position           - indicate the position of bit        
  -  output: unsigned long           - return 0x00000000 or 0x00000001   
  - --------------------------------------------------------------------------*/
unsigned long get_bit(array,position)
unsigned short array[4];
int position;
{
       unsigned short array_back[4];

#ifdef SCO_UNIX
       array_back[0] = array[1];
       array_back[1] = array[0];
       array_back[2] = array[3];
       array_back[3] = array[2];
#endif
#ifdef PUMA_UNIX
       array_back[0] = array[0];
       array_back[1] = array[1];
       array_back[2] = array[2];
       array_back[3] = array[3];
#endif

    if (position < 17)  
       return (( array_back [0] >> (16 - position)) & ONE);
    if (position < 33)  
       return (( array_back [1] >> (32 - position)) & ONE);
    if (position < 49)  
       return (( array_back [2] >> (48 - position)) & ONE);
    return ((array_back [3] >> (64 - position)) & ONE);
    }  /* end of get_bit  */  

    
            
/*----------------------------------------------------------------------------
  -  procedure cipher_function:                                          
  -    Input a 32-bit block(R) of rignt plaintext, and a 48-bit key(K),   
  -    let B1B2B3B4B5B6B7B8 = K Xor E(R)                                 
  -    then outout = P(S1(B1)S2(B2)S3(B3)S4(B4)S5(B5)S6(B6)S7(B7)S8(B8))  
  -  ------------------------------------------------------------------------
  -  input:  unsigned long   Rignt_Block_In                               
  -                            - the right 32 bits of plaintext           
  -          unsigned short  *p_key                                       
  -                            - a pointer that indicates a key of 48 bits 
  -  output: unsigned long   *Right_Block_Out                            
  -                            - a 32-bit output                          
  - ------------------------------------------------------------------------ */  
 cipher_function ( Right_Blocks_In,p_key,Right_Blocks_Out)
 unsigned long  Right_Blocks_In;
 unsigned long  *Right_Blocks_Out;
 unsigned short *p_key;
 {  
 
   int Expansion_Function_E [48]=
   {32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
     8, 9,10,11,12,13,12,13,14,15,16,17,
    16,17,18,19,20,21,20,21,22,23,24,25,
    24,25,26,27,28,29,28,29,30,31,32, 1
    };
    
   int Permutation_Function_P [32] =
   {16, 7,20,21,29,12,28,17,
     1,15,23,26, 5,18,31,10,
     2, 8,24,14,32,27, 3, 9,
    19,13,30, 6,22,11, 4,25
    };
    char  Selection_Function_S1 [64] =
   {14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
     0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
     4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
    15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13
    };
    
 char Selection_Function_S2 [64]=
   {15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
     3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
     0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
    13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9
    };

 char Selection_Function_S3 [64]=
   {10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
    13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
    13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
     1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12
    }; 

 char Selection_Function_S4 [64] =
   { 7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
    13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
    10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
     3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14
    };   
                         
 char Selection_Function_S5 [64] =
   { 2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
    14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
     4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
    11, 8,12, 7, 1,14, 2,13, 6, 15, 0, 9,10, 4, 5,3
    };

 char Selection_Function_S6 [64] =
   {12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
    10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
     9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
     4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13
    };

 char Selection_Function_S7 [64]=
   { 4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
    13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
     1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
     6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12
    };

 char Selection_Function_S8 [64] =
   {13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
     1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
     7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
     2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11
    };   
             
    int i,j;
    unsigned short Blocks_Short [3];
    unsigned short Right_Blocks_Temp [3];
    unsigned short Blocks_Selection [8];
    unsigned short temp_short1;
    unsigned short temp_short2;
    unsigned short Key [3]; 
    unsigned short con17 = 0x000f;
    unsigned short con77 = 0x003f; 
    unsigned short con3 = 0x0003;  
    unsigned short con20 = 0x0020;
    unsigned long temp1[2];
    unsigned long temp2,temp; 
    
                          
    Key[0] = *p_key;
    Key[1] = *(++p_key);
    Key[2] = *(++p_key);
    
   /*  yield 48-bit output from expansion function E     */                       
   for (j=0; j<3; j++)
      for (i=1; i<=16; i++)  
         Right_Blocks_Temp[j] = ( Right_Blocks_Temp [j] << 1)   | 
                                ( (short)( Right_Blocks_In >> 
                                (32 - Expansion_Function_E [16*j+i-1]) )
                                & ONE);
       
    /* B = E(R) + key    */
     
    for (j=0; j<3; j++)
       Blocks_Short [j] = Right_Blocks_Temp[j] ^ Key [j];
      
    /* get Bi, is a block of 6 bits        */
        
       Blocks_Selection [0] =  Blocks_Short [0] >> 10 ;
       Blocks_Selection [1] = (Blocks_Short [0] >> 4) & con77;
       Blocks_Selection [2] = ((Blocks_Short [0] & con17 ) << 2) |
                              (Blocks_Short [1] >> 14);
       Blocks_Selection [3] = (Blocks_Short [1] >> 8) & con77;
       Blocks_Selection [4] = (Blocks_Short [1] >> 2 ) & con77;
       Blocks_Selection [5] = ((Blocks_Short [1] & con3) << 4 ) |
                              (Blocks_Short [2] >> 12 );
       Blocks_Selection [6] = (Blocks_Short [2] >> 6) & con77;
       Blocks_Selection [7] =  Blocks_Short [2] & con77;
                                              
       for (i=1; i<=8; i++)
       {  
         temp_short1 = (Blocks_Selection[i-1] >>1) & con17;
         temp_short2 = (Blocks_Selection[i-1] & ONE) << 4;
         Blocks_Selection [i-1] = (Blocks_Selection[i-1] & con20) | 
                       temp_short1 | temp_short2;
        };                                     
                                              
   /*  get 32-bit output from selection function      */  
   
       *Right_Blocks_Out = (*Right_Blocks_Out << 4 ) |
                          Selection_Function_S1[Blocks_Selection [0] ];
       
       *Right_Blocks_Out = (*Right_Blocks_Out << 4 ) |
                          Selection_Function_S2[Blocks_Selection [1] ];
       
       *Right_Blocks_Out = (*Right_Blocks_Out << 4 ) |
                          Selection_Function_S3[Blocks_Selection [2] ];
    
       *Right_Blocks_Out = (*Right_Blocks_Out << 4 ) |
                          Selection_Function_S4[Blocks_Selection [3] ];
    
       *Right_Blocks_Out = (*Right_Blocks_Out << 4 ) |
                          Selection_Function_S5[Blocks_Selection [4] ];
    
       *Right_Blocks_Out = (*Right_Blocks_Out << 4 ) |
                          Selection_Function_S6[Blocks_Selection [5] ];
    
       *Right_Blocks_Out = (*Right_Blocks_Out << 4 ) |
                          Selection_Function_S7[Blocks_Selection [6] ];     
    
       *Right_Blocks_Out = (*Right_Blocks_Out << 4 ) |
                          Selection_Function_S8[Blocks_Selection [7] ];  
                          
 /* From a 32-bit input,and permutation function P, yields a 32-bit output */        
 
       temp1[0] =  *Right_Blocks_Out;
       for (i=1; i<=32; i++)
            {
             temp2 = temp2 << 1; 
             temp = get_bit(temp1,Permutation_Function_P[i-1]);
             temp2 = temp | temp2 ;
             } 
               
       *Right_Blocks_Out = temp2; 
       return (0);
     }  /* end of cipher_function */

                                      

                                     
/*------------------------------------------------------------------------------
  -  procedure get_kn:                                                    
  -    Input a 56-bit block of CiDi, applying permuted choice 2 ,        
  -    get  a 48-bit key(K),                                             
  -  -------------------------------------------------------------------------
  -  input:  unsigned long   cd[2] - 56 bits input of key                 
  -  output: unsigned short  *p_key                                       
                             - a pointer that indicates a key of 48 bits  
  - ------------------------------------------------------------------------- */ 
  get_kn(cd,p_kn)
   unsigned long cd[2];
   unsigned short *p_kn;
   { 
   
   int Permuted_Choice_2 [56] =
   {14,17,11,24, 1, 5, 3,28,15, 6,21,10,
    23,19,12, 4,26, 8,16, 7,27,20,13, 2,
    41,52,31,37,47,55,30,40,51,45,33,48,
    44,49,39,56,34,53,46,42,50,36,29,32
    };

     int i,j; 
     unsigned long temp; 
     unsigned short kn[3];
     
     for (j=0; j<3; j++)
        for (i=1; i<=16; i++)
            {
             kn[j] = kn[j] << 1;
             temp = get_bit(cd,Permuted_Choice_2[j*16+i-1]);
             kn[j] = kn[j] | (short)temp ;
             } 
     *p_kn = kn[0];
     *(++p_kn) = kn[1];
     *(++p_kn) = kn[2];
     return (0);
   }   /* end of get_kn   */  
             
  /*----------------------------------------------------------------------------
  -  procedure rotate_shift:                                               
  -    Input a 32-bit block , rotate shift shift_number bit                
  -  ---------------------------------------------------------------------------
  -  input & output:  unsigned long   *cd0                                 
  -                         - a pointer that indicated                     
  -                           a 32-bit block to be rotate shifted          
  -  input :      int shift_number  - the bit number to be rotate shifted          
  - -------------------------------------------------------------------------*/                  
  rotate_shift(cd0,shift_number)
   unsigned long *cd0;
   int shift_number;
   {
      int i;
      unsigned long temp;
      unsigned long con28 = 0x0ffffffe;
      
      for (i=1; i<= shift_number; i++)
        {
         temp = (*cd0 >> 27 ) & ONE;
         *cd0 = ((*cd0 << 1) & con28 )| temp;
         }; 
      return (0);    
    }   /*  end of rorate_shift   */
        
/*-----------------------------------------------------------------------------
  -  procedure function_KS:                                                
  -    Input a 64-bit block of key, applying permuted choice 1 then        
  -    yield a 56-bit C0D0. key(K), C0D0 left shift and applying permuted  
  -    choice 2, yield 48-bit key Kn,  for n = 1,2,...,16.                 
  -  --------------------------------------------------------------------------
  -  input:  unsigned long   Key      - 64-bit key                         
  -  output: unsigned short  *p_K                                          
  -                    - a pointer that indicates a 48-element array of key 
  - ------------------------------------------------------------------------- */                                                                      
  function_KS (Key, p_K)
   unsigned long Key [2];             
   unsigned short  *p_K ;
   { 
    int Permuted_Choice_1 [56]  =
   {57,49,41,33,25,17, 9, 1,58,50,42,34,26,18,
    10, 2,59,51,43,35,27,19,11, 3,60,52,44,36,
    63,55,47,39,31,23,15, 7,62,54,46,38,30,22,
    14, 6,61,53,45,37,29,21,13, 5,28,20,12, 4
    };
     int Shift_Number_Const [16]=
   { 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
    };
        
     int i;
     unsigned long cd [2];
     unsigned long cc,dd; 
     unsigned long temp;
     unsigned short K[48];
     
     /*  create c0  */
     for (i = 1; i<=28; i++)
         {
           cd [0] = cd [0] << 1; 
           temp = get_bit(Key,Permuted_Choice_1[i - 1]);
           cd [0] = temp | cd[0];
         };           
     /* create d0  */     
     for (i = 29; i<= 56; i++)
         { 
           cd [1] = cd [1] << 1; 
           temp =  get_bit(Key,Permuted_Choice_1[i-1]);
           cd [1] =  temp  | cd[1];
         };
       
       cc = cd[0];
       dd = cd[1];
     for (i=1; i<=16;i++)
     { 
       cd[0] = cc;
       cd[1] = dd;
       rotate_shift(&cd[0],Shift_Number_Const[i-1]);     
       rotate_shift(&cd[1],Shift_Number_Const[i-1]);
       
     /* keep them for next loop */
       cc = cd[0];
       dd = cd[1];
     
       cd[0] = (cd[0] << 4) | (cd[1] >> 24);
       cd[1] = cd[1] << 8;
          
       get_kn(cd,&K[(i-1)*3]);
     
     }; 
     
     for (i=1; i<=48;i++)
         *(p_K++) = K[i-1];
     return (0);
   }  /*  end of function_KS  */
      


 
  /*---------------------------------------------------------------------------
  -  procedure DEA:                                                        
  -    Data Encryption Algorithm.                                          
  -    Input a 64-bit block(R) of plaintext, and a 64-bit key,             
  -    yield a 64-bit cipher text.                                        
  -  --------------------------------------------------------------------------
  -  input:  unsigned long   plaintext                                     
  -                            -  64 bits of plaintext                     
  -          unsigned short  key                                           
  -                            -  64 bits of key                           
  -  output: unsigned long   *p_enciphered   - a 64-bit cipher text        
  - --------------------------------------------------------------------------*/      
  int DEA(plaintext,key,p_enciphered)
   unsigned long plaintext[2];
   unsigned long key[2] ;
   unsigned long *p_enciphered;
   {  
    int Initial_Permutation [64]= 
   {58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
    57,49,41,33,25,17,9,1, 59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7
     };
    
   int Inverse_Initial_Permutation [64] =
   {40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,
    38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,
    36,4,44,12,52,20,60,28,35,3,43,11,51,19,59,27,
    34,2,42,10,50,18,58,26,33,1,41, 9,49,17,57,25
    };

     int i,j;
     unsigned long temp_text[2];
     unsigned short kn [48];
     unsigned long rr;
     unsigned long temp;
     unsigned long enciphered_text[2]; 
                       
 
     for (j=0; j<=1; j++)
     for (i=1; i<=32; i++)
        {
         temp_text[j] = temp_text[j] << 1;
         temp = get_bit(plaintext,Initial_Permutation[32*j+i-1]);
         temp_text[j] = temp 
                        | temp_text[j];
         };
                       
                            
     function_KS(key,kn);
     
     /*  16 iterations of the computation */                  
     for (i=1; i<=16; i++)
        {
          cipher_function(temp_text[1],&kn[(i-1)*3],&rr);
          enciphered_text[0] = temp_text[1];
          temp_text[1] = temp_text[0] ^ rr;
          temp_text[0] = enciphered_text[0];
         }  
         
          enciphered_text[1] = temp_text[0];
          enciphered_text[0] = temp_text[1];  
          temp_text[0] = enciphered_text[0];
          temp_text[1] = enciphered_text[1]; 
                            
     /* Inverse of Initial Permutation  */
     for (j=0; j<=1; j++)
     for (i=1; i<=32; i++)
        {
         enciphered_text[j] = enciphered_text[j] << 1; 
         temp =  get_bit(temp_text,Inverse_Initial_Permutation[j*32+i-1]);
         enciphered_text[j] = temp | enciphered_text[j];
         };                       
         
      *p_enciphered = enciphered_text[0];
      *(++p_enciphered) = enciphered_text[1]; 
      return (0);
    }  /* end of DEA  */     
    
 /*-------------------------------------------------------------------------
  -  procedure de_DEA:                                                        
  -    decryption using Data Encryption Algorithm.                                          
  -    Input a 64-bit block(R) of cipher text, and a 64-bit key,             
  -    yield a 64-bit plaintext.                                        
  -  ----------------------------------------------------------------------
  -  input:  unsigned long   ciphertext                                     
  -                            -  64 bits of ciphertext                     
  -          unsigned short  key                                           
  -                            -  64 bits of key                           
  -  output: unsigned long   *p_plaintext   - a 64-bit plaintext        
  - ---------------------------------------------------------------------- */      
  int de_DEA(ciphertext,key,p_plaintext)
   unsigned long ciphertext[2];
   unsigned long key[2] ;
   unsigned long *p_plaintext;
   { 
    int Initial_Permutation [64]= 
   {58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
    57,49,41,33,25,17,9,1, 59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7
     };
    
   int Inverse_Initial_Permutation [64] =
   {40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,
    38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,
    36,4,44,12,52,20,60,28,35,3,43,11,51,19,59,27,
    34,2,42,10,50,18,58,26,33,1,41, 9,49,17,57,25
    };

     int i,j;
     unsigned long temp_text[2];
     unsigned short kn [48];
     unsigned long rr;
     unsigned long temp;
     unsigned long plaintext[2]; 
                       
 
     for (j=0; j<=1; j++)
     for (i=1; i<=32; i++)
        {
         temp_text[j] = temp_text[j] << 1;
         temp = get_bit(ciphertext,Initial_Permutation[32*j+i-1]);
         temp_text[j] = temp 
                        | temp_text[j];
         };
                       
                            
     function_KS(key,kn);
     
     /*  16 iterations of the computation */                  
     for (i=1; i<=16; i++)
        {
          cipher_function(temp_text[1],&kn[(16-i)*3],&rr);
          plaintext[0] = temp_text[1];
          temp_text[1] = temp_text[0] ^ rr;
          temp_text[0] = plaintext[0];
         }  
         
          plaintext[1] = temp_text[0];
          plaintext[0] = temp_text[1];  
          temp_text[0] = plaintext[0];
          temp_text[1] = plaintext[1]; 
                            
     /* Inverse of Initial Permutation  */
     for (j=0; j<=1; j++)
     for (i=1; i<=32; i++)
        {
         plaintext[j] = plaintext[j] << 1; 
         temp =  get_bit(temp_text,Inverse_Initial_Permutation[j*32+i-1]);
         plaintext[j] = temp | plaintext[j];
         };                       
         
      *p_plaintext = plaintext[0];
      *(++p_plaintext) = plaintext[1]; 
      return (0);
    }  /* end of de_DEA   */    
        
    

/*---------------------------------------------------------------------------
  -  procedure EnMAC:                                                        
  -    Message Authentication Code                                         
  -    Input a 64-bit block of plaintext, a 64-bit encryption key,         
  -    and a length of authentication elements, yield a 64-bit cipher text.
  -  -----------------------------------------------------------------------
  -  input:  char macdata[300] - authentication elements                   
  -          int  length       - the length of authentication elements     
  -          char key[8]       - encryption key                            
  -  output: unsigned long     - a 64-bit cipher text                      
  - ----------------------------------------------------------------------- */    
   unsigned long EnMAC (macdata,length,key,mac)
    char *macdata;  
    int  length;
    char key[8];
    char mac[8];     
    {
     int i,j;
     unsigned long outdata[8]={0,0,0,0,0,0,0,0};
     char inverse_c[8]; 
     char inverse_key[8]; 
     char data[8];                         
     char outdata1[8]; 
   
     unsigned short zero = 0x00;
     unsigned short con377 = 0xff; 
                      
      for (i=0;i<=7;i++)
      outdata1[i] = (char)outdata[i]; 
      
#ifdef SCO_UNIX
      inverse_key[0] = key[3];
      inverse_key[1] = key[2];
      inverse_key[2] = key[1];
      inverse_key[3] = key[0];
      inverse_key[4] = key[7];
      inverse_key[5] = key[6];
      inverse_key[6] = key[5];
      inverse_key[7] = key[4];
#endif
#ifdef PUMA_UNIX
      inverse_key[0] = key[0];
      inverse_key[1] = key[1];
      inverse_key[2] = key[2];
      inverse_key[3] = key[3];
      inverse_key[4] = key[4];
      inverse_key[5] = key[5];
      inverse_key[6] = key[6];
      inverse_key[7] = key[7];
#endif

      length = length -1;
                               
     for (i=0; i<=length; i=i+8)
     { 
      
#ifdef SCO_UNIX
      inverse_c[0] = macdata[i+3];
      inverse_c[1] = macdata[i+2];
      inverse_c[2] = macdata[i+1];
      inverse_c[3] = macdata[i];
      inverse_c[4] = macdata[i+7];
      inverse_c[5] = macdata[i+6];
      inverse_c[6] = macdata[i+5];
      inverse_c[7] = macdata[i+4];
                                   
      if ((i+1) > length ) inverse_c[2] = (char)zero;
      if ((i+2) > length ) inverse_c[1] = (char)zero; 
      if ((i+3) > length ) inverse_c[0] = (char)zero;
      if ((i+4) > length ) inverse_c[7] = (char)zero;  
      if ((i+5) > length ) inverse_c[6] = (char)zero;
      if ((i+6) > length ) inverse_c[5] = (char)zero;
      if ((i+7) > length ) inverse_c[4] = (char)zero;
#endif                                   
#ifdef PUMA_UNIX
      inverse_c[0] = macdata[i];
      inverse_c[1] = macdata[i+1];
      inverse_c[2] = macdata[i+2];
      inverse_c[3] = macdata[i+3];
      inverse_c[4] = macdata[i+4];
      inverse_c[5] = macdata[i+5];
      inverse_c[6] = macdata[i+6];
      inverse_c[7] = macdata[i+7];
                                     
      if ((i+1) > length ) inverse_c[1] = (char)zero;
      if ((i+2) > length ) inverse_c[2] = (char)zero; 
      if ((i+3) > length ) inverse_c[3] = (char)zero;
      if ((i+4) > length ) inverse_c[4] = (char)zero;  
      if ((i+5) > length ) inverse_c[5] = (char)zero;
      if ((i+6) > length ) inverse_c[6] = (char)zero;
      if ((i+7) > length ) inverse_c[7] = (char)zero;
#endif
          
       for (j=0; j<8; j++)
         data[j] = outdata1[j] ^ inverse_c[j]; 
      
      
      DEA(data,inverse_key,outdata1);   
 
      }; 

#ifdef SCO_UNIX     
      mac[0] = outdata1[3];   
      mac[1] = outdata1[2]; 
      mac[2] = outdata1[1]; 
      mac[3] = outdata1[0]; 
      mac[4] = outdata1[7]; 
      mac[5] = outdata1[6]; 
      mac[6] = outdata1[5]; 
      mac[7] = outdata1[4]; 
#endif
#ifdef PUMA_UNIX
      mac[0] = outdata1[0];   
      mac[1] = outdata1[1]; 
      mac[2] = outdata1[2]; 
      mac[3] = outdata1[3]; 
      mac[4] = outdata1[4]; 
      mac[5] = outdata1[5]; 
      mac[6] = outdata1[6]; 
      mac[7] = outdata1[7]; 
#endif

     return (0);
    } /* end of EnMAC */

  /*--------------------------------------------------------------------------
  -  procedure DeMAC:                                                        
  -    Message Authentication Code                                         
  -    Input a 64-bit block of plaintext, a 64-bit encryption key,         
  -    and a length of authentication elements, yield a 64-bit cipher text.
  -  ------------------------------------------------------------------------
  -  input:  char macdata[300] - authentication elements                   
  -          int  length       - the length of authentication elements     
  -          char key[8]       - encryption key                            
  -  output: unsigned long     - a 64-bit cipher text                      
  - ------------------------------------------------------------------------ */    
   unsigned long DeMAC (macdata,length,key,mac)
    char macdata[300];  
    int  length;
    char key[8];
    char mac[8];     
    {
     int i,j;
     unsigned long outdata[8]={0,0,0,0,0,0,0,0};
     char inverse_c[8]; 
     char inverse_key[8]; 
     char data[8];                         
     char outdata1[8];
    
     unsigned short zero = 0x00;
     unsigned short con377 = 0xff; 
                      
      for (i=0;i<=7;i++)
      outdata1[i] = (char)outdata[i]; 
      
#ifdef SCO_UNIX
      inverse_key[0] = key[3];
      inverse_key[1] = key[2];
      inverse_key[2] = key[1];
      inverse_key[3] = key[0];
      inverse_key[4] = key[7];
      inverse_key[5] = key[6];
      inverse_key[6] = key[5];
      inverse_key[7] = key[4];
#endif
#ifdef PUMA_UNIX
      inverse_key[0] = key[0];
      inverse_key[1] = key[1];
      inverse_key[2] = key[2];
      inverse_key[3] = key[3];
      inverse_key[4] = key[4];
      inverse_key[5] = key[5];
      inverse_key[6] = key[6];
      inverse_key[7] = key[7];
#endif
                               
      length = length -1;
                               
     for (i=0; i<=length; i=i+8)
     { 
      
#ifdef SCO_UNIX
      inverse_c[0] = macdata[i+3];
      inverse_c[1] = macdata[i+2];
      inverse_c[2] = macdata[i+1];
      inverse_c[3] = macdata[i];
      inverse_c[4] = macdata[i+7];
      inverse_c[5] = macdata[i+6];
      inverse_c[6] = macdata[i+5];
      inverse_c[7] = macdata[i+4];
                                   
      if ((i+1) > length ) inverse_c[2] = (char)zero;
      if ((i+2) > length ) inverse_c[1] = (char)zero; 
      if ((i+3) > length ) inverse_c[0] = (char)zero;
      if ((i+4) > length ) inverse_c[7] = (char)zero;  
      if ((i+5) > length ) inverse_c[6] = (char)zero;
      if ((i+6) > length ) inverse_c[5] = (char)zero;
      if ((i+7) > length ) inverse_c[4] = (char)zero;
#endif                                   
#ifdef PUMA_UNIX
      inverse_c[0] = macdata[i];
      inverse_c[1] = macdata[i+1];
      inverse_c[2] = macdata[i+2];
      inverse_c[3] = macdata[i+3];
      inverse_c[4] = macdata[i+4];
      inverse_c[5] = macdata[i+5];
      inverse_c[6] = macdata[i+6];
      inverse_c[7] = macdata[i+7];
                                     
      if ((i+1) > length ) inverse_c[1] = (char)zero;
      if ((i+2) > length ) inverse_c[2] = (char)zero; 
      if ((i+3) > length ) inverse_c[3] = (char)zero;
      if ((i+4) > length ) inverse_c[4] = (char)zero;  
      if ((i+5) > length ) inverse_c[5] = (char)zero;
      if ((i+6) > length ) inverse_c[6] = (char)zero;
      if ((i+7) > length ) inverse_c[7] = (char)zero;
#endif
                                   
          
       for (j=0; j<8; j++)
         data[j] = outdata1[j] ^ inverse_c[j];
      
       
                
        de_DEA(data,inverse_key,outdata1);
      
      
      }; 
     
#ifdef SCO_UNIX     
      mac[0] = outdata1[3];   
      mac[1] = outdata1[2]; 
      mac[2] = outdata1[1]; 
      mac[3] = outdata1[0]; 
      mac[4] = outdata1[7]; 
      mac[5] = outdata1[6]; 
      mac[6] = outdata1[5]; 
      mac[7] = outdata1[4]; 
#endif
#ifdef PUMA_UNIX
      mac[0] = outdata1[0];   
      mac[1] = outdata1[1]; 
      mac[2] = outdata1[2]; 
      mac[3] = outdata1[3]; 
      mac[4] = outdata1[4]; 
      mac[5] = outdata1[5]; 
      mac[6] = outdata1[6]; 
      mac[7] = outdata1[7]; 
#endif
       
     return (0);
    } /* end of DeMAC */

                                     
   /*--------------------------------------------------------
    - unit testing case:    t_u_mac_01                   
    - MAC =  F3E9DC4E                                       
    ---------------------------------------------------------
   */
    unsigned long t_u_mac_01()
    {
      char inputdata[ ]=
  "80 07 14 127 TRNSFR USD 1234567,89 FRM ACCNT 48020166 TO ACCNT 40210178 QWERT ";
      int  length,i;       
      char crypt_key[8];
      unsigned short aa [8]={0xE6,0xA1,0x2F,0x07,0x9D,0x15,0xC4,0x37};
      unsigned short bb [8]={0x5C,0X5B,0X21,0X58,0XF9,0XD8,0XED,0X9B};
      unsigned long mac[2]; 
      unsigned long *ptr;
      char macdata[8];
                        
       for (i=0; i<=7;i++)             
      crypt_key[i] = (char) aa[i];
      
      length = 78; 
                      
                      
       EnMAC(inputdata,length,crypt_key,macdata);
       ptr =(unsigned long *) macdata;
       mac [0] = *ptr;
       mac [1] = *(++ptr);
      return(0);  
     }  /*  end of t_u_mac_01   */
                 
  
         
   int char_to_hex(ch8,hex,num)    
   char ch8[16]; 
   char hex[32];
   int  num;
   {
  
    unsigned char con17 = 0x0f;
    int i;
    char temp;
    char tt[2];
    
    for (i=0;i<= num - 1;i++)
    {       
     tt[0] =  ((ch8[i] >> 4) & con17) + 0x30;  
     temp = tt[0];
     if (temp > 0x39)                          
     hex[i*2] = temp + 0x07;
         else hex[i*2] = temp ;
     tt[0]= ((ch8[i] & con17) + 0x30);           
     temp = tt[0];                            
     if (temp > 0x39)
     hex[i*2+1] = temp + 0x07;
         else hex[i*2+1] = temp ;
     
    };   
    return (0);
    } /* end of char_to_hex */  
     
   
   int VerifyParity(keydata)
   char keydata[8];
    {  
     unsigned char con01 = 0x01;
     int i;
     unsigned short j; 
     unsigned short one = 1;   
               
     
     for (i=0;i<=7;i++)
     { j=0;
      j= j+ (keydata[i] >> 7) & con01;
      j= j+ (keydata[i] >> 6) & con01;  
      j= j+ (keydata[i] >> 5) & con01; 
      j= j+ (keydata[i] >> 4) & con01;
      j= j+ (keydata[i] >> 3) & con01;  
      j= j+ (keydata[i] >> 2) & con01; 
      j= j+ (keydata[i] >> 1) & con01; 
 
      j =j+ ( keydata[i] & con01);   
      if (j != one) return(1);
      };            
     return(0);
    } /* end of VerifyParity    */   
  
    
   int MakeParity(keydata)
   char keydata[8];
      {  
       unsigned char con01 = 0x01;
       int i;
       unsigned short j;
          
       
       for (i=0;i<=7;i++)
       { j=0;
         j= j+ (keydata[i] >> 7) & con01;
         j= j+ (keydata[i] >> 6) & con01;  
         j= j+ (keydata[i] >> 5) & con01; 
         j= j+ (keydata[i] >> 4) & con01;
         j= j+ (keydata[i] >> 3) & con01;  
         j= j+ (keydata[i] >> 2) & con01; 
         j= j+ (keydata[i] >> 1) & con01;
          
         keydata[i] = keydata[i]  & 0xFE; 
         
         if (j != con01) keydata[i] = keydata[i] + con01;
       };           
       return(0);
    } /* end of MakeParity    */ 
  
     
   int hex_to_char(hex,ch8,num)
   char hex[32];
   char ch8[16];
   int  num;
   {
    int i; 
    char ch; 
    char temp1; 
    char temp2;
    
    for (i=0;i<= num - 1;i++)
    {        
      ch = hex[i*2]; 
   
      if (ch > 0x39)  temp1 = hex[i*2] - 0x37;
                      else  temp1 = hex[i*2] - 0x30;
      ch =hex[i*2+1]; 
      if (ch > 0x39)  temp2 = hex[i*2+1] - 0x37;
                      else  temp2 = hex[i*2+1] - 0x30; 
                                   
      ch8[i] = (temp1 << 4 )| temp2; 
           
    };   
    return (0);
    } /* end of hex_to_char */
   
 
      
   int get_field(msg_in,i,length,got_field) 
   char *msg_in;
   int *i;
   int length;
   char got_field[50];
   {
        memcpy(got_field,&msg_in[*i],length);  
       *i = *i+length;   
       return(0);
    }  /*   end of get_field   */   
  
                                                           
   int GetRand(rand_num)
   char rand_num[8];
    {  
      int stime;
      long ltime;
                                   
           ltime = time(NULL);
           stime = (unsigned int) ltime/2;
           srand(stime);
           sprintf(rand_num, "%h", rand());
           return(0);
     }  /*   end of GetRand  */   
       
   
   int EncipherSingleKey(bmk,kd,key)
   unsigned char bmk[16];
   unsigned char kd[8]; 
   unsigned char key[8];
   {
     unsigned char temp[8];
     
     EnMAC(kd,8,bmk,key);
     DeMAC(key,8,&bmk[8],temp);
     EnMAC(temp,8,bmk,key);
     return(0);      
    }   /* EncipherSingleKey  */
    
   int DecipherSingleKey(bmk,kd,key)
   unsigned char bmk[16];
   unsigned char kd[8]; 
   unsigned char key[8];
   {
     unsigned char temp[8];
     
     DeMAC(kd,8,bmk,key);
     EnMAC(key,8,&bmk[8],temp);
     DeMAC(temp,8,bmk,key);
     return(0);      
    }   /* decipher_key  */  
    
   int EncipherPairKey(mk,bmk,key)
   unsigned char mk[16];
   unsigned char bmk[16]; 
   unsigned char key[16];
   {
     unsigned char temp[8];
     unsigned char cipher[8];
     
     EnMAC(bmk,8,mk,temp);
     DeMAC(temp,8,&mk[8],cipher);
     EnMAC(cipher,8,mk,key); 
     
     EnMAC(&bmk[8],8,mk,temp);
     DeMAC(temp,8,&mk[8],cipher);
     EnMAC(cipher,8,mk,&key[8]);     
     return(0);        
    }   /* EncipherPairKey  */    
    
   int DecipherPairKey(mk,bmk,key)
   unsigned char mk[16];
   unsigned char bmk[16]; 
   unsigned char key[16];
   {
     unsigned char temp[8];
     unsigned char cipher[8];
     
     DeMAC(bmk,8,mk,temp);
     EnMAC(temp,8,&mk[8],cipher);
     DeMAC(cipher,8,mk,key); 
     
     DeMAC(&bmk[8],8,mk,temp);
     EnMAC(temp,8,&mk[8],cipher);
     DeMAC(cipher,8,mk,&key[8]);     
     return(0);        
    }   /* DecipherPairKey  */    
     
   int check_sum(in_data,out_data,llen)
   char *in_data;
   char out_data[4];
   int llen;
   {
    int i;
    unsigned short sum;
    unsigned char data[8];
    char hexdata[16];
    unsigned short con17 = 0x00ff;
    unsigned short temp;
    
    
    sum = 0;
    for(i=0;i<=llen-1;i++)
      sum = sum + in_data[i]; 

    temp = sum & con17;
    data[1] = (char)temp;
    temp = sum >> 8;
    data[0] = (char)temp;
  
    char_to_hex(data,hexdata,4);  
    memset(out_data,0x00,4); 
    strncpy(out_data,hexdata,4);
    return(0);
    }  /* end of sheck_sum */

void GenerateSingleKey(char * SingleKey)
{
	GetRand(SingleKey);
	MakeParity(SingleKey);
}

void GeneratePairKey(char * PairKey)
{
	GetRand(PairKey);
	MakeParity(PairKey);
	GetRand(PairKey + 8);
	MakeParity(PairKey + 8);
}

void DecipherHalfKey(unsigned char postmk[8], unsigned char kd[8],
                     unsigned char key[8])
{
	DeMAC(kd,8,postmk,key);
}  

void EncipherHalfKey(unsigned char postmk[8], unsigned char kd[8],
                     unsigned char key[8])
{
	EnMAC(kd,8,postmk,key);
}  
 
