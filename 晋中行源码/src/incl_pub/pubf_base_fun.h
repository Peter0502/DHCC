/** base目录下 函数声明部分 **/
/** pubf_base_ac.c **/

/** pubf_base_data.c **/
void pub_base_GetDate(int *year, int *month, int *day);
char * pub_base_strpack( char *str );
char *get_zd_data( char *sjbm, char *data );
char *set_zd_data( char *sjbm, char *data );
char get_zd_char( char *sjbm, char *data );
char set_zd_char( char *sjbm, char data );
short get_zd_short( char *sjbm, short *shortdata );
short set_zd_short( char *sjbm, short shortdata );
long get_zd_long( char *sjbm, long *longdata );
long set_zd_long( char *sjbm, long longdata );
float get_zd_float( char *sjbm, float *floatdata );
float set_zd_float( char *sjbm, float floatdata );
double get_zd_double( char *sjbm, double *doubledata );
double set_zd_double( char *sjbm, double doubledata );
double floor(double);
int    pubf_base_putmsg(char msgend,char *msgtype,char *msgdata,int msglen);

/** pubf_base_date.c **/
void pub_base_get_date(int *year, int *month, int *day);
long pub_base_daynumSUB(long date1, int num);

/** pubf_base_db.c **/
long pub_base_open_database();
long pub_base_close_database();

/** pubf_base_des.c **/
void pub_base_EnDes(long txdate, char *acno, char *passwd );

/** pubf_base_file.c **/

/** pubf_base_intst_acm.c **/

/** pubf_base_number.c **/
unsigned char pub_base_GetDigit(unsigned char ch);
unsigned char pub_base_Combine( unsigned char *Twochar);
double pub_base_PubDround_1 ( double lx );

/** pubf_base_str.c **/
char *pub_base_DelSpace(char *s, int n );
char * pub_base_strpack( char *str );

/** pubf_base_time.c **/
char *pub_base_GetTime( char *format );
void pub_base_GetTime2(int *hour, int *minute, int *second);

/** pubf_base_trace.c **/

/** pubf_base_wbclt.c **/

/** pubf_base_medium.c **/
/** int pue_base_check_promedi(char *prdt_num,char *mdm_num1,
							char *mdm_num2,char *mdm_num3)
**/
/**int pub_base_patch_book(xym,int upt_num, char *ac_num); **/

/** pubf_base_charge.c **/

/** pubf_base_product.c **/

