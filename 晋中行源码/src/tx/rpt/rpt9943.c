/*************************************************************
* �� �� ��:  rpt9943.c
* ���������� �ձ����ӡ
* ��    ��:  
* �������:  2006��08��19��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "gl_mst_c.h"
#include "com_item_c.h"
#define PAGE_CNT 43

struct gl_mst_c	s_gl_mst;
int flag=0;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
char acc_type[2];
int opncnt;
FILE *fp; 
int ret = 0;

int get_ratio_lsqd(int bli,int bll,char str[100]);

rpt994( )
{
	int tag = 0;
	int    ttlnum;
	char   old_acc_head[2];
	char   filename[100];

	struct com_sys_parm_c s_com_sys_parm;

	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
         
	ret=sql_begin(); /*������*/
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	old_acc_head[0] = 'A';
	old_acc_head[1] = '\0';

	/* ��ʼ��ȫ�ֱ��� */
	pub_base_sysinit();
    
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;

	strcpy( filename , "BHDRPT103" );	
	strcpy( vpfm, "BHDRPT103" );

	ret = Gl_mst_Dec_Sel( g_pub_tx.reply, "cur_no='01' ORDER BY acc_hrt" );
	ERR_DEAL
	
	ttlnum=0;
	
	while(1)
	{
		ret = Gl_mst_Fet_Sel( &s_gl_mst,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				if( tag==0 )/*����ڿ�Ŀ��β*/
					ret = print_tail_B( );
				else if( tag==2 )/*������Ŀ��β*/
					ret = print_tail_C( );
				ERR_DEAL
			}
			else 
			{
				sprintf( acErrMsg, "����û��Ҫ��ӡ����ˮ!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}

		/* ��Ŀ��任ʱ */
		if( s_gl_mst.acc_hrt[0] != old_acc_head[0] )
		{
			if( old_acc_head[0] == 'A' )/* �ʲ���(��ʼ��ӡʱ) */
			{
				strcpy( acc_type , "1" );
				old_acc_head[0] = '1';
				ERR_DEAL
			}
			else 
			{
				/* ��ӡҳβ */
				if( old_acc_head[0] == '5' )/*������*/
				{
					flag = 1;/*�Ƿ��ӡ�ϼ����ı�־*/
					ret = print_tail_A( );/*��ӡ���ںϼ���*/
					ERR_DEAL
					flag = 0;
				}
				else 
				{
					if( tag==0 )
						ret = print_tail_B( );/*����ڿ�Ŀ��β*/
					else if( tag==2 ) 
						ret = print_tail_C( );/*������Ŀ��β*/
					ERR_DEAL
				}

				switch( old_acc_head[0] )
				{
					case '1':							/* ��ծ�� */
							strcpy( acc_type , "2" );
							break;
					case '2':							/* ������Ȩ���� */
							strcpy( acc_type , "3" );
							break;
					case '3':							/* �ʲ���ծ��ͬ�� */
							strcpy( acc_type , "4" );
							break;
					case '4':							/* ������ */
							strcpy( acc_type , "5" );
							break;
					case '5':							/* ������ */
							tag = 1;					/* �����־ */
							strcpy( acc_type , "6" );
							break;
					case '6':							/* ������ */
							tag = 2;					/* �����־ */
							strcpy( acc_type , "7" );
							break;
				 	default: 
							break;
				}
			}

			old_acc_head[0] = s_gl_mst.acc_hrt[0] ;/*��Ŀ�����ֱ任*/
			
			line_file = 1;/*������ʼ��*/
			
			if( !ttlnum )/*ttlnum???????????*/
			{
				page = 1;

				/* ɾ��ԭ�ļ� */
				ret = pub_rpt_rmfile( s_gl_mst.br_no , filename , 0 );
				ERR_DEAL

				/* ���ļ� */	
				ret = pub_rpt_openfile( &fp, s_gl_mst.br_no , filename );
				ERR_DEAL
			}

			/* ��ӡҳ�� */
			if( tag == 0 )/*����*/
				ret = print_head_B( );
			else if( tag==1 )/*����*/
				ret = print_head_C( );
			ERR_DEAL
		}
		/*��Ŀ��û�б仯����ӡ������*/
    	/* ��ӡ��Ŀ��Ϣ */
		if( tag==0 )/*���ڿ�Ŀ*/
			ret = print_body_B( );
		else 
			ret = print_body_C( );/*�����Ŀ*/
		ERR_DEAL

		line_file++;/*����++*/

		if( line_file>PAGE_CNT )			
		{
			line_file=0;
			page++;

			/* ��ӡҳβ */
			if( tag==0 )
				ret = print_tail_B();
			else
				ret = print_tail_C();
			ERR_DEAL

			/* ��ӡҳ�� */
			if( tag==0 )
				ret = print_head();
			else
				ret = print_head_C();
			ERR_DEAL
		}
		ttlnum++;
	}

	Gl_mst_Clo_Sel( );
	fclose(fp);
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ձ������ɴ���ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�ձ������ɴ���ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ���ڻ�ҳ��ӡ���� */
int print_head( )
{
	char vrec[3];
	
	vtcp_log("AAAAAAAAAAA----------------------[%s]",acc_type );
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ���ڴ�ӡ���� */
int print_head_B( )
{
	char vrec[3];
	
	vtcp_log("BBBBBBBBBBBB----------------------[%s]",acc_type );
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);

	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

	strcpy( vrec,"GG" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* �����ӡ���� */
int print_head_C( )
{
	char vrec[3];
	
	vtcp_log("AAAAAAAAAAA----------------------[%s]",acc_type );
	strcpy( vrec,"PP" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"DD" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}
/*���⻻ҳ��ӡ����*/
int print_head_D()
{
	char vrec[3];
	vtcp_log("TTTTTTTTTTTT------------------[%s]",acc_type);
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	strcpy( vrec,"JJ" );
	ret = Make_yeb_sub(vrec);
	strcpy( vrec,"DD" );
	ret = Make_yeb_sub(vrec);
}
/* ���ڴ�ӡ��β */
int print_tail_B( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* �����ӡ��β */
int print_tail_C( )
{
	char vrec[3];

	strcpy( vrec,"FF" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ���ڴ�ӡ���� */
int print_body_B( )
{
	char vrec[3];

	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* �����ӡ���� */
int print_body_C( )
{
	char vrec[3];

	strcpy( vrec,"EE" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* �������п�Ŀ���ܴ�ӡ���� ������Ҫ�޸� */
int print_tail_A( )
{
	char vrec[3];

	strcpy( vrec,"GG" );
	ret = Make_yeb_sub(vrec);

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ��ֵ���� */
/* ˵����bli ��������ַ����� ... bll�������ռ�ĳ��� ... str�������ֵ */
int get_ratio_lsqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	struct com_item_c s_com_item;

	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[41];
	char amt_tmp[41]; 
	char tmp_inf[41];
	char tmp_acc_name[41];
	char fmt[20]; 
	char br_name[71];
	double tmp_amt;
	int i=0; 

	switch( bli ) 
	{ 
		case 'B':/*�������*/
			sprintf(fmt,"%5s",bll);
			sprintf(str,fmt,s_gl_mst.br_no);
			break;
		case 'C':/*��������*/
			ret=pub_base_getbrname(s_gl_mst.br_no,br_name);
			ERR_DEAL
			pub_base_strpack(br_name);
			sprintf(fmt,"%s",bll);
			sprintf(str,fmt,br_name);
			break;
		case 'D':/*��ӡ����*/
			sprintf( t_str ,"%4d��%2d��%2d��", g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'I':/*��Ŀ���*/
			pub_base_strpack( s_gl_mst.acc_hrt );
            ret = Com_item_Sel( g_pub_tx.reply, &s_com_item, "acc_hrt='%s'", \
                                  s_gl_mst.acc_hrt );
	   		ERR_DEAL
	   		sprintf( t_str , "%s" , s_com_item.acc_no );
	   		sprintf(fmt, "%%-%ds", bll);
	   		sprintf(str, fmt, t_str);
			break;
		case 'E': /*��Ŀ����*/
			pub_base_strpack( s_gl_mst.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item,"acc_hrt='%s'", \
								s_gl_mst.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'F':/*���ջ���*/
			break;
		case 'G':/*���ս跽���*/
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.ldd_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.ldd_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H':/*���մ������*/
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'J':/*���ս跽������*/
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.rdd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.rdd_amt ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K':/*���մ���������*/
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L':/*���ջ���*/
			break;
		case 'M':/*����ĩ�跽���*/
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.dr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.dr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'N':/*����ĩ�������*/
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O':/*ҵ����*/
			break;
		case 'X': /* ���� */
			if( flag==0 )
			{
				if( acc_type[0]=='1' )
					strcpy( tmp_acc_name,"һ.�ʲ���ϼ�" );
				else if( acc_type[0]=='2' )
					strcpy( tmp_acc_name,"��.��ծ��ϼ�" );
				else if( acc_type[0]=='3' )
					strcpy( tmp_acc_name,"��.������Ȩ����ϼ�" );
				else if( acc_type[0]=='4' )
					strcpy( tmp_acc_name,"��.�ʲ���ծ��ϼ�" );
				else if( acc_type[0]=='5' )
					strcpy( tmp_acc_name,"��.������ϼ�" );
			}
			else
				strcpy( tmp_acc_name,"���ڿ�Ŀ��ϼ�" );
			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Y': /* ���� */
			strcpy( tmp_acc_name, "���ڿ�Ŀ�ϼ�" );
			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'R': /* ���ս跢���� */
			vtcp_log( "acc_type====================[%s]",acc_type );
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_mst","rdd_amt",&tmp_amt,"acc_hrt[1]='%c' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0] );
			}
			else 
			{
				ret = sql_sum_double( "gl_mst","rdd_amt",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'T': /* ���մ������� */
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_mst","rcd_amt",&tmp_amt,"acc_hrt[1]='%c' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0] );
			}
			else 
			{
				ret = sql_sum_double( "gl_mst","rcd_amt",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'U': /* ���ս���� */
			if( flag==0  )
			{
				ret = sql_sum_double( "gl_mst","dr_bal",&tmp_amt,"acc_hrt[1]='%c'  and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0] );
			}
			else
			{
				ret = sql_sum_double( "gl_mst","dr_bal",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'V': /* ���մ���� */
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_mst","cr_bal",&tmp_amt,"acc_hrt[1]='%c' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' )", acc_type[0] );
			}
			else
			{
				ret = sql_sum_double( "gl_mst","cr_bal",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
