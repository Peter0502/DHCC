#include <stdio.h>
#include <string.h>

void dhc_setkey(char *ckey);
void dhc_encrypt(char *block,int ENCRYPTION_FLAG);
static void des_permu(int wkin[],int wkout[],char perm[],int num);
static void des_vxor(int a[],int b[],int t);
static int  des_power(int a,int b);


static int m_subkey[16][48];
static char IP[64] =                        /* initial permutation P */
       { 58,50,42,34,26,18,10, 2,
         60,52,44,36,28,20,12, 4,
         62,54,46,38,30,22,14, 6,
         64,56,48,40,32,24,16, 8,
         57,49,41,33,25,17, 9, 1,
         59,51,43,35,27,19,11, 3,
         61,53,45,37,29,21,13, 5,
         63,55,47,39,31,23,15, 7 };

static char FP[64] =
       { 40, 8,48,16,56,24,64,32,
         39, 7,47,15,55,23,63,31,
         38, 6,46,14,54,22,62,30,
         37, 5,45,13,53,21,61,29,
         36, 4,44,12,52,20,60,28,
         35, 3,43,11,51,19,59,27,
         34, 2,42,10,50,18,58,26,
         33, 1,41, 9,49,17,57,25 };

static char E[48] =
       { 32, 1, 2, 3, 4, 5,
          4, 5, 6, 7, 8, 9,
          8, 9,10,11,12,13,
         12,13,14,15,16,17,
         16,17,18,19,20,21,
         20,21,22,23,24,25,
         24,25,26,27,28,29,
         28,29,30,31,32, 1 };

static char PC1[56] =
       { 57,49,41,33,25,17, 9,
          1,58,50,42,34,26,18,
         10, 2,59,51,43,35,27,
         19,11, 3,60,52,44,36,
         63,55,47,39,31,23,15,
          7,62,54,46,38,30,22,
         14, 6,61,53,45,37,29,
         21,13, 5,28,20,12,4 };


/* Number of Bits of Circular Shifts for Each Cycle */
static char keyrot[16] =
       { 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };

/* Choice Permutation To Select 48 Key Bits */
static char PC2[48] =
       { 14,17,11,24, 1, 5,
          3,28,15, 6,21,10,
         23,19,12, 4,26, 8,
         16, 7,27,20,13, 2,
         41,52,31,37,47,55,
         30,40,51,45,33,48,
         44,49,39,56,34,53,
         46,42,50,36,29,32 };

/* S-Box Table For The DES */
static char S[8][64] =
       { 14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,  /* s[1] */
          0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
          4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
         15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13,

         15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,  /* s[2] */
          3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
          0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
         13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9,

         10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,  /* s[3] */
         13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
         13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
          1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12,

          7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,  /* s[4] */
         13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
         10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
          3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14,

          2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,  /* s[5] */
         14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
          4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
         11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3,

         12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,  /* s[6] */
         10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
          9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
          4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13,

          4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,  /* s[7] */
         13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
          1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
          6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12,

         13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,  /* s[8] */
          1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
          7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
          2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11 };

/* Permutation Box P */
static char P[32] =
       { 16, 7,20,21,
         29,12,28,17,
          1,15,23,26,
          5,18,31,10,
          2, 8,24,14,
         32,27, 3, 9,
         19,13,30, 6,
         22,11, 4,25 };


/* The following code is written by Liew inorder to do the des descrypt and enscrypt  */
/* Key to use, stored as an array of Longs */
void dhc_setkey(char *ckey){
  int key[64];
  int wk[56],wk2[48];   /* Storage for the two 28-bit key halves C and D */
  int i,j,k,temp;
  for(i=0;i<64;i++){
	key[i]=ckey[i]-'0';
  }
  des_permu(key, wk, PC1, 56);       /* Permute key with PC1    */

  for (i=0;i<16;i++) {      /* Form sixteen subkeys    */
      for (j=0;j<keyrot[i];j++) {
          temp = wk[0];
          for (k=0;k<27;k++) wk[k] = wk[k+1];
          wk[27] = temp;
      }

      for (j=0;j<keyrot[i];j++) {
          temp = wk[28];
          for (k=28;k<55;k++)
              wk[k] = wk[k+1];
          wk[55] = temp;
      }
      des_permu(wk, wk2, PC2, 48);   /* Apply PC2 to form subkey */
      for (j=0;j<48;j++) m_subkey[i][j] = wk2[j];
  }
}

void dhc_encrypt(char *block,int ENCRYPTION_FLAG)
{
  int work[64];      /*  64-bit work2g store       */
  int work2[64];
  int work3[64];
  int rwork[32];
  int lwork[32];
  int twork[32];
  int t2work[32];
  int erwork[48];
  int sval[8];
  int i,j,k,row,col,rocol,m,kk,ll;
  for(i=0;i<64;i++)
	work2[i]=block[i]-'0';
  des_permu(work2, work, IP, 64);        /*  Apply IP to input block    */

  for (i=0;i<32;i++) lwork[i] = work[i];
  for (i=0;i<32;i++) rwork[i] = work[i+32];
  for (i=0;i<16;i++) {                /*  Perform the 16 rounds      */
      des_permu(rwork, erwork, E, 48);
      /* expanded input data R(i) to 48 bits using fn E   */

      if (ENCRYPTION_FLAG == 0)
          kk = i;
      else
          kk = 15 - i;
      des_vxor(m_subkey[kk],erwork,48);

      for (j=0;j<48;j +=6) {
          row = des_power(erwork[j+0],1) + des_power(erwork[j+5],0);
          col = des_power(erwork[j+1],3) + des_power(erwork[j+2],2) +
                des_power(erwork[j+3],1) + des_power(erwork[j+4],0);
          rocol = row * 16 + col;
          m = j / 6;
          sval[m] = S[m][rocol];
      }

      for (j=0;j<32;j +=4) {
          m = j / 4;
          if (sval[m] ==  0) { twork[j]=0; twork[j+1]=0; twork[j+2]=0 ; twork[j+3] = 0;}
          if (sval[m] ==  1) { twork[j]=0; twork[j+1]=0; twork[j+2]=0 ; twork[j+3] = 1;}
          if (sval[m] ==  2) { twork[j]=0; twork[j+1]=0; twork[j+2]=1 ; twork[j+3] = 0;}
          if (sval[m] ==  3) { twork[j]=0; twork[j+1]=0; twork[j+2]=1 ; twork[j+3] = 1;}
          if (sval[m] ==  4) { twork[j]=0; twork[j+1]=1; twork[j+2]=0 ; twork[j+3] = 0;}
          if (sval[m] ==  5) { twork[j]=0; twork[j+1]=1; twork[j+2]=0 ; twork[j+3] = 1;}
          if (sval[m] ==  6) { twork[j]=0; twork[j+1]=1; twork[j+2]=1 ; twork[j+3] = 0;}
          if (sval[m] ==  7) { twork[j]=0; twork[j+1]=1; twork[j+2]=1 ; twork[j+3] = 1;}
          if (sval[m] ==  8) { twork[j]=1; twork[j+1]=0; twork[j+2]=0 ; twork[j+3] = 0;}
          if (sval[m] ==  9) { twork[j]=1; twork[j+1]=0; twork[j+2]=0 ; twork[j+3] = 1;}
          if (sval[m] == 10) { twork[j]=1; twork[j+1]=0; twork[j+2]=1 ; twork[j+3] = 0;}
          if (sval[m] == 11) { twork[j]=1; twork[j+1]=0; twork[j+2]=1 ; twork[j+3] = 1;}
          if (sval[m] == 12) { twork[j]=1; twork[j+1]=1; twork[j+2]=0 ; twork[j+3] = 0;}
          if (sval[m] == 13) { twork[j]=1; twork[j+1]=1; twork[j+2]=0 ; twork[j+3] = 1;}
          if (sval[m] == 14) { twork[j]=1; twork[j+1]=1; twork[j+2]=1 ; twork[j+3] = 0;}
          if (sval[m] == 15) { twork[j]=1; twork[j+1]=1; twork[j+2]=1 ; twork[j+3] = 1;}
      }

      des_permu(twork, t2work, P, 32);   /*  32-bit des_permutation P to B  */
      des_vxor(lwork,t2work,32);

      for (j=0;j<32;j++) lwork[j] = rwork[j];
      for (j=0;j<32;j++) rwork[j] = t2work[j];
  }

  for (j=0 ;j<32;j++) work[j] = rwork[j];
  for (j=32;j<64;j++) work[j] = lwork[j - 32];
  des_permu(work, work2, FP ,64);        /*  Perform Inverse IP      */
  for(i=0;i<64;i++){
	block[i]=work2[i]+'0';
  }
}




static void des_permu(int wkin[],int wkout[],char perm[],int num)
/* Input 64-bit block after des_permutation  */
/* Output 64-bit block to be des_permuted    */
/* Permutation array                     */
{
  int i,m;

  for (i=0;i<num;i++) {
      m = perm[i] - 1;
      wkout[i] = wkin[m];
  }
}



static void des_vxor(int a[],int b[],int t)
{
   int k,j,temp;

   for (k=0;k<t;k++) {
       if (a[k] == 0 && b[k] == 0)  temp = 0;
       if (a[k] == 1 && b[k] == 1)  temp = 0;
       if (a[k] == 0 && b[k] == 1)  temp = 1;
       if (a[k] == 1 && b[k] == 0)  temp = 1;
       b[k] = temp;
   }
}



static int des_power(int a,int b)
{
   switch (b)
   {
       case 0 : if (a == 1) return(1); else return(0);
       case 1 : if (a == 1) return(2); else return(0);
       case 2 : if (a == 1) return(4); else return(0);
       case 3 : if (a == 1) return(8); else return(0);
   }
}

