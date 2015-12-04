#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#define YYDESKEY "1234567890ABCDEF"
int exch_pc_1_c[28]={
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36};
int exch_pc_1_d[28]={
	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4};
int exch_pc_2[48]={
	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21,  10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32};

int exch_ls_count[16]={
	0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0 };

int exch_ip_tab[64]={
	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6,
	64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7};
int _exch_ip_tab[64]={
	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22 ,62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25};

int exch_e_r[48]={
	32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
	 8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};

int exch_P[32]={
	16,  7, 20, 21, 29, 12, 28, 17,
	 1, 15, 23, 26,  5, 18, 31, 10,
	 2,  8, 24, 14, 32, 27,  3,  9,
	19, 13, 30,  6, 22, 11,  4, 25};

int exch_SSS[8][4][16]={
	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,

	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,

	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,

	 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,

	 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,

	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,

	 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,

	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11};

static unsigned char C[17][29],D[17][29],K[17][49],c;



void DesUnDesPack(unsigned char bit[65], unsigned char block[9], unsigned int blockLen)
{
    unsigned int i, j;
    unsigned char t;

    for (i = 0; i < blockLen; i++) {
        t = block[i];

        for (j = 0; j < 8; j++)
            bit[8 * i + j] = (unsigned char)((t >> (7 - j)) & 1);
    }
}

void DesPack(unsigned char block[9], unsigned char bit[65], unsigned int blockLen)
{
    unsigned int i, j;
    unsigned char t;

    for (i = 0; i < blockLen; i++) {
        t = 0;

        for (j = 0; j < 8; j++)
            t |= bit[8 * i + j] << (7 - j);

        block[i] = t;
    }
}


void exch_LS(char bits[29], char buffer[29], int count)
{
    int i;

    if ( count ) { 
       for ( i = 0; i < 26; i++ )
           buffer[i] = bits[i + 2];

       buffer[26] = bits[0];
       buffer[27] = bits[1];
    }
    else {
       for ( i = 0; i < 27; i++ )
           buffer[i]=bits[i+1];

       buffer[27]=bits[0];
    }
}

void exch_son(char cc[29], char dd[29], char kk[49])
{
    int i;
    char buffer[57];

    buffer[56] = '\0';

    for ( i = 0; i < 28; i++ ) {
        buffer[i] = cc[i];
        buffer[i + 28] = dd[i];
    }

    for ( i = 0; i < 48; i++ )
        kk[i] = buffer[exch_pc_2[i] - 1];
}

void exch_setkey(unsigned char bits[65])
{
    int i,j;

    for ( i = 0; i < 28; i++ )
         C[0][i] = bits[(exch_pc_1_c[i]) - 1];
 
    for ( i = 0; i < 28; i++)
        D[0][i] = bits[(exch_pc_1_d[i]) - 1];

    for ( j = 0; j < 16; j++) {
        exch_LS((char*)C[j], (char*)C[j + 1], exch_ls_count[j]);
        exch_LS((char*)D[j], (char*)D[j + 1], exch_ls_count[j]);
        exch_son((char*)C[j + 1], (char*)D[j + 1], (char*)K[j + 1]);
    }
}


void exch_ip(unsigned char text[65], char ll[34], char rr[34])
{
    int i;

    for ( i = 0; i < 33; i++ )
        ll[i] = text[exch_ip_tab[i] - 1];

    for ( i = 0; i < 33; i++ )
        rr[i] = text[exch_ip_tab[i + 33] - 1];
}

void _exch_ip(unsigned char text[65], char ll[34], char rr[34])
{
    int i;
    char tmp[65];

    for ( i = 0; i < 33; i++ )
        tmp[i] = ll[i];

    for ( i = 33; i < 64; i++ )
        tmp[i] = rr[i - 33];

    for ( i = 0; i < 64; i++ )
        text[i] = tmp[_exch_ip_tab[i] - 1];
}


void exch_s_box(char aa[49], char bb[34])
{
    int i,j,k,m;
    int y,z;
    char ss[8];

    m = 0;

    for ( i = 0; i < 8; i++ ) {
        j = 6 * i;
        y = aa[j] * 2 + aa[j + 5];
        z = aa[j + 1] * 8 + aa[j + 2] * 4 + aa[j + 3] * 2 + aa[j + 4];
        ss[i] = exch_SSS[i][y][z];
        y = 0x08;

        for ( k = 0; k < 4; k++ ) {
            bb[m++] = (ss[i] / y) & 1;
            y /= 2;
        }
    }
}

void exch_F(int n, char ll[34],char rr[34], char LL[34], char RR[34])
{
    int i;
    char buffer[50],tmp[34];

    buffer[49] = '\0';
    tmp[33] = '\0';


    for ( i = 0; i < 49; i++ )
        buffer[i] = rr[exch_e_r[i] - 1];

    for ( i = 0; i < 49; i++ )
        buffer[i] = (buffer[i] + K[n][i]) & 1;

        exch_s_box(buffer, tmp);

    for ( i = 0; i < 33; i++ )
        buffer[i] = tmp[exch_P[i] - 1];

    for ( i = 0; i < 33; i++ ) 
        RR[i] = (buffer[i] + ll[i]) & 1;

    for ( i = 0; i < 33; i++ )
        LL[i] = rr[i];
}


void exch_encrypt0(unsigned char text[65], unsigned char mtext[65])
{
    char ll[34],rr[34],LL[34],RR[34];
    int i,j;
    ll[33]=rr[33]=LL[33]=RR[33]='\0';

    exch_ip(text,ll,rr);

    for ( i = 1; i < 17; i++ ) {
        exch_F ( i, ll, rr, LL, RR );
        for (j=0;j<33;j++) {
            ll[j] = LL[j];
            rr[j] = RR[j];
        }
    }

    _exch_ip(mtext, rr, ll);
}


void exch_discrypt0(unsigned char mtext[65], unsigned char text[65])
{
    char ll[34],rr[34],LL[34],RR[34];
    int i,j;

    ll[33] = rr[33] = LL[33] = RR[33] = '\0';
    exch_ip(mtext, ll, rr);

    for ( i = 16; i > 0; i-- ) {
        exch_F(i, ll, rr, LL, RR);
        for ( j = 0; j < 33; j++ ) {
            ll[j] = LL[j];
            rr[j] = RR[j];
        }
    }

    _exch_ip(text, rr, ll);
}

void exch_DES(unsigned char key[65],unsigned char text[65], unsigned char mtext[65])
{
    exch_setkey(key);
    exch_encrypt0(text, mtext);
}

void _exch_DES(unsigned char key[65],unsigned char mtext[65], unsigned char text[65])
{
    exch_setkey(key);
    exch_discrypt0(mtext,text);
}

char * EDE(char key[17],char original[9])
{ 
    unsigned char /*text[9],*/mtext[9],key1[9],key2[9];
    unsigned char btext[65],bmtext[65],bkey1[65],bkey2[65];
    int c;

    for ( c = 0; c < 17; c++ )  
        C[c][28] = D[c][28] = K[c][48] = '\0';

    mtext[8] = key1[8] = btext[64] = bmtext[64] = bkey1[64] = '\0';

    memmove( key1, key, 8 );
    memmove( key2, key + 8, 8 );
 
 /*把 byte->bit */  
    DesUnDesPack( (unsigned char*)btext, (unsigned char*)original, 8 );
    DesUnDesPack( (unsigned char*)bkey1, (unsigned char*)key1, 8 );
    DesUnDesPack( (unsigned char*)bkey2, (unsigned char*)key2, 8 );

/* bit key(bkey1) & bit text(btext) ---> bit encrypt text(bmtext) */

    exch_DES( bkey1, btext, bmtext );
    _exch_DES( bkey2, bmtext, btext );
    exch_DES( bkey1, btext, bmtext ); 
    DesPack ( (unsigned char*)original, (unsigned char*)bmtext,8 );
    original[8]='\0';

    return( original);
}

char * _EDE(char key[17], char cipher[9])
{ 
    unsigned char /*text[9],*/mtext[9],key1[9],key2[9];
    unsigned char btext[65],bmtext[65],bkey1[65],bkey2[65];
    int c;

    for ( c = 0; c < 17; c++ )  
        C[c][28] = D[c][28] = K[c][48] = '\0';

    mtext[8] = key1[8] = btext[64] = bmtext[64] = bkey1[64] = '\0';

    memmove( key1, key,8 );
    memmove( key2, key + 8, 8 );

  
 /*把 byte->bit */  
   DesUnDesPack( (unsigned char*)btext, (unsigned char*)cipher, 8 );
   DesUnDesPack( (unsigned char*)bkey1, (unsigned char*)key1, 8 );
   DesUnDesPack( (unsigned char*)bkey2, (unsigned char*)key2, 8 );

/* bit key(bkey1) & bit text(btext) ---> bit encrypt text(bmtext) */

    _exch_DES( bkey1, btext, bmtext );
    exch_DES( bkey2, bmtext, btext );
    _exch_DES( bkey1, btext, bmtext ); 
    DesPack( (unsigned char*)cipher, (unsigned char*)bmtext, 8 );
    cipher[8]='\0';

    return( cipher);
}

char * encry (char key[9], char original[9])
{
    unsigned char bkey[65],boriginal[65],bmtext[65];
    int i;
	
    for ( i = 0; i < 17; i++ )  
        C[c][28] = D[c][28] = K[c][48] = '\0';

    key[8] = original[8] = bkey[64] = boriginal[64] = bmtext[64] = '\0';


 /*把 byte->bit */  
    DesUnDesPack( (unsigned char*)boriginal, (unsigned char*)original, 8 );
    DesUnDesPack( (unsigned char*)bkey, (unsigned char*)key, 8 );
   

/* bit key(bkey) & bit text(btext) ---> bit encry text(bmtext) */
    exch_DES( bkey, boriginal, bmtext );

    DesPack( (unsigned char*)original, (unsigned char*)bmtext, 8);
    original[8] = '\0';

    return( original);
}

char * decrypt(char key[9], char cipher[9])
{ 
    unsigned char btext[65],bcipher[65],bkey[65];
    int i,j,k;

    for ( i = 0; i < 17; i++ )  
        C[c][28] = D[c][28] = K[c][48] = '\0';

    cipher[8] = key[8] = bcipher[64] = btext[64] = bkey[64] = '\0';


  
 /*把 byte->bit */  
    DesUnDesPack( (unsigned char*)bcipher, (unsigned char*)cipher, 8 );
    DesUnDesPack( (unsigned char*)bkey, (unsigned char*)key, 8 );

/* bit key(bkey1) & bit text(btext) ---> bit encry text(bmtext) */

    _exch_DES( bkey, bcipher, btext );

    DesPack( (unsigned char*)cipher, (unsigned char*)btext, 8 );
    cipher[8] = '\0';

    return( cipher );
}


void rtrim (char* str )
{
 	char	*s = str;

	while ( *s )
		++s;

	--s;
	while ( s >= str )
		if ( (*s==' ') || (*s=='\t') || (*s=='\r') || (*s=='\n') )
			--s;
		else
			break;

	* ( s + 1 ) = 0;

	return;
}


void str_tran(char *str)
{
    
   char str1[1000],str2[1000],*p1;
   int i,j,k;

   strcpy(str1,str);
   j = k = 0;

/**** 去连续空格 ****/
   for ( i = 0; str1[i] != '\0'; i++ ) {
       if ( str1[i] != ' ' || j == 0) {
          str2[k] = str1[i];
          k++;
          if ( str1[i] == ' ')
             j++;
          else 
             j = 0;
       } 
   }

   str2[k] = '\0';
   k = 0;

/**** 去首空格 ****/
   if ( str2[0] == ' ') {
      p1 = str2;
      p1++;
      strcpy(str2,p1);
   }

/**** 去尾空格 ****/
   if ( str2[strlen(str2) - 1] == ' ')
      str2[strlen(str2) -1] = '\0';

/**** 去[A-Z] [a-z] [0-9] [' ' , .] 外字符 ****/
   for (i=0 ; str2[i]!='\0'; i++) {
       if ( !(str2[i]&128) && ((str2[i] >= 'a' && str2[i] <= 'z') || \
            (str2[i] >= 'A' && str2[i] <= 'Z') || \
            (str2[i] >= '0' && str2[i] <= '9') || \
            str2[i] == ' ' || str2[i] == '.' || str2[i] == ','))
           str1[k++] = str2[i]; 
   } 

   str1[k] = '\0';
   strcpy(str,str1);
}


void get_mac(unsigned char *mak, unsigned char *strmac, long mac_len)
{
    unsigned char 	strtmp[9];
    unsigned char	macblk[9];
    int 	i=0;
    int 	j=0;
    
    memset(	strtmp,	0x00,sizeof( strtmp ) );
    memset(	macblk,	0x00,sizeof( macblk ) );

    for(i = 0; i < mac_len / 8; i++) {
    
    	memset( strtmp, 0x00, sizeof( strtmp ));
        memcpy( strtmp, strmac + i * 8, 8 );
        
        if (i > 0) {
           for(j = 0; j < 8; j++)
              macblk[j] = macblk[j] ^ strtmp[j]; 
        } 
        else
           memcpy( macblk, strtmp, 8 );
		
    	encry( (char*)mak, (char*)macblk );

   }

   memcpy( strmac, macblk, 8 );
   strmac[8] = '\0';

}

int gen_mac(unsigned char* key, unsigned char* str_mab, unsigned char* str_mac )
{
   unsigned char 	*	tmp_mab;
    unsigned char		strtmp[3];
    int 		str_len;
    int			mac_len;
    int			i;
    int			ret;   

	memset( strtmp, 0x00, sizeof( strtmp ));
			
	str_tran((char*)str_mab);
	
	str_len = strlen( (char*)str_mab ) * sizeof( unsigned char );
	mac_len = 8 * ( (str_len + 7) / 8 );
	tmp_mab = ( unsigned char * )malloc( mac_len + 1 );
	memset( (char*)tmp_mab, 0, mac_len + 1 );
	strcpy( (char*)tmp_mab, (char*)str_mab);

	for ( i = str_len; i < mac_len; i++ )
		tmp_mab[i] = 0x00;
	tmp_mab[mac_len] = '\0';
		
    get_mac( key, tmp_mab, mac_len);

    for ( i = 0; i < 4; i++ ) 	{
        sprintf((char*)strtmp, "%02X", (unsigned char)tmp_mab[i]);
        memcpy((char*)str_mac + i * 2, (char*)strtmp, 2);
    }
    str_mac[8] = '\0';
    

	free(tmp_mab);
    
    return 0;
}


void str2hex( unsigned char	*str, unsigned char	*hex )
{
	int		i;

	memset(hex, 0, sizeof(hex));

	sprintf((char*)hex, "%02X%02X%02X%02X%02X%02X%02X%02X", str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);
	hex[16] = 0;
}

void str16hex(unsigned char	*str, unsigned char	*hex )
{
	int		i;

	memset(hex, 0, sizeof(hex));

	sprintf((char*)hex, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7], str[8], str[9], str[10], str[11], str[12], str[13], str[14], str[15]);
	hex[32] = 0;
}

void hex2str(unsigned char	*hex, unsigned char	*str )
{
	int		i;
	unsigned char	proc_str[8+1];

	memset(proc_str, 0, sizeof(proc_str));

	for(i = 0; i < 8; i++ )
	{
	    if (hex[2 * i] > 0x39)
			proc_str[i] = 0xFF & ((hex[2 * i] - 0x07) << 4);
	    else
			proc_str[i] = 0xFF & (hex[2 * i] << 4);

		if (hex[2 * i + 1] > 0x39)
		   proc_str[i] = (proc_str[i] | 0x0F) & ((hex[2 * i + 1] - 0x07) | 0xF0);
		else
		   proc_str[i] = (proc_str[i] | 0x0F) & (hex[2 * i + 1] | 0xF0);
	}

	proc_str[8] = 0;

	memcpy( str, proc_str, 8 );

	str[8] = 0;
}

void hex16str(unsigned char	*hex, unsigned char	*str )
{
	int		i;
	unsigned char	proc_str[16+1];

	memset(proc_str, 0, sizeof(proc_str));

	for(i = 0; i < 16; i++ )
	{
	    if (hex[2 * i] > 0x39)
			proc_str[i] = 0xFF & ((hex[2 * i] - 0x07) << 4);
	    else
			proc_str[i] = 0xFF & (hex[2 * i] << 4);

		if (hex[2 * i + 1] > 0x39)
		   proc_str[i] = (proc_str[i] | 0x0F) & ((hex[2 * i + 1] - 0x07) | 0xF0);
		else
		   proc_str[i] = (proc_str[i] | 0x0F) & (hex[2 * i + 1] | 0xF0);
	}

	proc_str[16] = 0;

	memcpy( str, proc_str, 16 );

	str[16] = 0;
}
/****bcd->asc***/
int iBcdToAsc(char *pcBCD,char *pcASC,int iLength){
	int i;
	for(i=0;i<iLength;i++){
		unsigned char ch;
		ch=(unsigned char)pcBCD[i];
		ch=ch>>4;
		if(ch>=10){
			pcASC[2*i]=ch-10+'A';
		}else{
			pcASC[2*i]=ch+'0';
		}
		ch=(unsigned char)pcBCD[i];
		ch=ch &0x0f;
		if(ch>=10){
			pcASC[2*i+1]=ch-10+'A';
		}else{
			pcASC[2*i+1]=ch+'0';
		}
	}
	return(0);
}
/****asc->bcd***/
int iAscToBcd(char *pcASC,char *pcBCD,int iLength){
	int i;
	for(i=0;i<iLength/2;i++){
		unsigned char ch1,ch2;
		ch1=(unsigned char)pcASC[i*2];
		ch2=(unsigned char)pcASC[i*2+1];
		if(ch1>='a' && ch1<='f')
			ch1=ch1-'a'+0xa;
		else if(ch1>='A' && ch1<='F')
			ch1=ch1-'A'+0xa;
		else
			ch1=ch1-'0';
		if(ch2>='a' && ch2<='f')
			ch2=ch2-'a'+0xa;
		else if(ch2>='A' && ch2<='F')
			ch2=ch2-'A'+0xa;
		else
			ch2=ch2-'0';
		pcBCD[i]=(ch1<<4)|ch2;
	}
	return(0);
}
/*****验印系统加密函数****/
/****clpswd-明文,enpswd-密文，16位****/
int YYDES(char *clpswd,char *enpswd){
	char pcTmpBuf[17];
	char pcBitKey[9];
	int n;
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	memset(pcBitKey,'\0',sizeof(pcBitKey));
	memcpy(pcTmpBuf,clpswd,6);
	iAscToBcd(YYDESKEY,pcBitKey,16);
	encry(pcBitKey,pcTmpBuf);
	iBcdToAsc(pcTmpBuf,enpswd,8);
	return(0);
}
/****clpswd-明文,enpswd-密文，16位****/
int YYUNDES(char *enpswd,char *clpswd){
	char pcTmpBuf[17];
	char pcBitKey[9];
	int n;
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	memset(pcBitKey,'\0',sizeof(pcBitKey));
	iAscToBcd(enpswd,pcTmpBuf,16);
	iAscToBcd(YYDESKEY,pcBitKey,16);
	decrypt(pcBitKey,pcTmpBuf);
	memcpy(clpswd,pcTmpBuf,6);
	return(0);	
}
/*********
int main(){
	char buf[17];
	char buf2[17];
	memset(buf,'\0',sizeof(buf));
	memset(buf2,'\0',sizeof(buf2));
	YYDES(" 11111",buf);
	YYUNDES(buf,buf2);
	printf("[%s]\n",buf2);
	return(0);
}
***********/
