/*************************************************************
* �� �� ��:  rpt994.c
* ���������� �ձ����ӡ
* ��    ��:  
* �������:  2003��08��15��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret && ret!=100) {\
		sprintf( acErrMsg, "sql error[%d]",ret ); \
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
#define PAGE_CNT 48

struct gl_mst_c	s_gl_mst;
struct gl_mst_c	sGl_mst;
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
	char   old_br_no[8];  /*  add by mt */
	int    loop_cnt=0; /** aaa **/
	int    cnt=0;  /** aaa***/

	struct com_sys_parm_c s_com_sys_parm;
	struct com_item_c s_com_item;

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
 memset(old_br_no,0,sizeof(old_br_no)); /*  add  by martin */

	/* ��ʼ��ȫ�ֱ��� */
	pub_base_sysinit();
    
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;

	/*strcpy( filename , "BRrbb" );	*/
	strcpy( filename , "RPT994" );
	/*strcpy( vpfm, "bbrjb" );*/
	strcpy( vpfm, "RPT994");

/*	ret = Gl_mst_Dec_Sel( g_pub_tx.reply, "cur_no='01' and  (rdd_amt<>0 or rcd_amt<>0 or dr_bal<>0 or cr_bal<>0 or\
	 ldd_bal<>0 or lcd_bal<>0) ORDER BY acc_hrt" );*/
	 ret = RPT_Gl_Dec_Sel ( g_pub_tx.reply ,"gl_mst","com_item", "gl_mst.cur_no='01' and \
	      gl_mst.acc_hrt = com_item.acc_hrt order by gl_mst.br_no,com_item.acc_no" );
	ERR_DEAL
	
	ttlnum=0;
	loop_cnt=0; /** aaa ***/
	while(1)
	{
		ret = RPT_Gl_Fet_Sel( &s_gl_mst,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				if( tag==0 )
				{
					flag = 1;					 /** aaa ***/
					ret = print_body_A( );  /** aaa ***/
					ret = print_tail_B( );
				}	
				else/* if( tag==2 )*/
					ret = print_tail_C( );
				ERR_DEAL
				fclose(fp);   /** aaa **/
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
		
		if( !pub_base_CompDblVal(s_gl_mst.dr_bal,0.00) &&
			!pub_base_CompDblVal(s_gl_mst.cr_bal,0.00) &&
			!pub_base_CompDblVal(s_gl_mst.ldd_bal,0.00) &&
			!pub_base_CompDblVal(s_gl_mst.lcd_bal,0.00) &&
			!pub_base_CompDblVal(s_gl_mst.rdd_amt,0.00) &&
			!pub_base_CompDblVal(s_gl_mst.rcd_amt,0.00)  )
			{
				memset(&sGl_mst, 0x00, sizeof(struct gl_mst_c));
				pub_base_strpack( s_gl_mst.acc_hrt ); 
			  ret = Com_item_Sel( g_pub_tx.reply, &s_com_item, "acc_hrt='%s'", s_gl_mst.acc_hrt );
				
				if( strlen(s_com_item.acc_no)==7 )
				{
			        continue;
			  }
			  
				ret=Gl_mst_Sel( g_pub_tx.reply , &sGl_mst , "cur_no='01' and acc_hrt like '%s%%' \
				and (rdd_amt<>0 or rcd_amt<>0 or dr_bal<>0 or cr_bal<>0 or ldd_bal<>0 or lcd_bal<>0)" , s_com_item.acc_no);
				 if( ret==100 ) 
				 {
		/*		 if( !pub_base_CompDblVal(sGl_mst.dr_bal,0.00) &&
			       !pub_base_CompDblVal(sGl_mst.cr_bal,0.00) &&
			       !pub_base_CompDblVal(sGl_mst.mdr_bal,0.00) &&
			       !pub_base_CompDblVal(sGl_mst.mcr_bal,0.00) &&
			       !pub_base_CompDblVal(sGl_mst.mdr_amt,0.00) &&
			       !pub_base_CompDblVal(sGl_mst.mcr_amt,0.00)  )
		*/
				   continue;
			    }
			  
			 }
			 
			 

    if(memcmp(old_br_no,s_gl_mst.br_no,sizeof(s_gl_mst.br_no)-1) )  /* add by martin  start*/
    {
    	if(strlen(old_br_no))
    	{
    		 tag = 0;
    	   old_acc_head[0] = 'A';
	       old_acc_head[1] = '\0';
	       ret = print_tail_C( );
			   ERR_DEAL
			   fclose(fp);
			 
	    }
	    page = 1;
			line_file = 0;
			
			loop_cnt = 0;   /** aaa **/
			cnt = sql_count( "gl_mst" , "cur_no='01' and br_no='%s'", s_gl_mst.br_no ); /** aaaa **/
		  sprintf(acErrMsg,"����[%s]�м�¼[%d]",s_gl_mst.br_no,cnt);
	    WRITEMSG
			/* ɾ��ԭ�ļ� */
				ret = pub_rpt_rmfile( s_gl_mst.br_no , filename , 0 );
				ERR_DEAL

				/* ���ļ� */	
				ret = pub_rpt_openfile( &fp, s_gl_mst.br_no , filename );
				ERR_DEAL
				
			memset(old_br_no,0,sizeof(old_br_no));   
   	  memcpy(old_br_no,s_gl_mst.br_no,sizeof(s_gl_mst.br_no)-1); 
   	}                                                             /*  add by mt end  */
    
		/* ��Ŀ��任ʱ */
		if( s_gl_mst.acc_hrt[0] != old_acc_head[0] )
		{
			if( old_acc_head[0] == 'A' )				/* �ʲ��� */
			{
				strcpy( acc_type , "1" );
				old_acc_head[0] = '1';
				ERR_DEAL
				
				ret = print_head( );  /*add by martin*/
				ERR_DEAL
			}
			else 
			{
				/* ��ӡҳβ ���⿪ʼ*/
				if( old_acc_head[0] == '5' )
				{
					flag = 1;
					ret = print_tail_A( );
					ERR_DEAL
					
					fprintf (fp,"\f");
					
					flag = 0;
					line_file = 0; /**/
				}
				else 
				{
			/*by martin		if( tag==0 )
						ret = print_tail_B( );
					else*/
					 if( tag==2 ) 
						ret = print_tail_C( );
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

			old_acc_head[0] = s_gl_mst.acc_hrt[0] ;
			
/*				line_file = 1;
			
			if( !ttlnum )
			{
    			page = 1;

				* ɾ��ԭ�ļ� *
				ret = pub_rpt_rmfile( s_gl_mst.br_no , filename , 0 );
				ERR_DEAL

				* ���ļ� *
				ret = pub_rpt_openfile( &fp, s_gl_mst.br_no , filename );
				ERR_DEAL
				
    

			} add by mt and next hang */

			/* ��ӡҳ�� *   ban martin
			if( tag == 0 )
				ret = print_head_B( );
			else if( tag==1 )
				ret = print_head_C( );
			ERR_DEAL
		}

    	* ��ӡ��Ŀ��Ϣ *
		if( tag==0 )
			ret = print_body_B( );
		else 
			ret = print_body_C( );
		ERR_DEAL
		
		  line_file++;
   */
     if( tag==1 )                  /*   add by mt  */
			{
				page++;
				ret = print_head_C( );
			  ERR_DEAL 
		  } else if(tag != 2){       /*���ⲻ�����*/
			 	 ret = print_body_A( );
			   ERR_DEAL
			   line_file++;
			}

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
			fprintf (fp,"\f");

			/* ��ӡҳ�� */
			if( tag==0 )
				ret = print_head();
			else
				ret = print_head_C();
			ERR_DEAL
		}
/*		ttlnum++;   ban */
	}
	/* ��ӡ��Ŀ��Ϣ add by mt*/
		if( tag==0 )
			ret = print_body_B( );
		else 
			ret = print_body_C( );
		ERR_DEAL

		line_file++;
		loop_cnt++; /** aaa **/

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
			
			fprintf (fp,"\f");
			
			if(loop_cnt>=cnt) /** aaa **/
      	continue;       /** aaa **/

			/* ��ӡҳ�� */
			if( tag==0 )
				ret = print_head();
			else
				ret = print_head_C();
			ERR_DEAL
		}
		ttlnum++;
	}/*  add by mt */
	RPT_Gl_Clo_Sel( );
/*	fclose(fp);  */
	
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
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
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
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);

	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

	strcpy( vrec,"NN" );
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
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"HH" );
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

/* ���ڴ�ӡ����  add by mt*/
int print_body_A( )
{
	char vrec[3];

	strcpy( vrec,"NN" );
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

/* �������п�Ŀ���ܴ�ӡ���� */
int print_tail_A( )
{
	char vrec[3];

	strcpy( vrec,"NN" );
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
		case 'A': /* �������� */ 
			sprintf( t_str , "%4d��%2d��%2d��" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* ��Ŀ���� */
			pub_base_strpack( s_gl_mst.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item,"acc_hrt='%s'", \
								s_gl_mst.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'C': /* �������� */
			ret = pub_base_getbrname( s_gl_mst.br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_gl_mst.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* ���ս跢���� */
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.rdd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.rdd_amt ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'F': /* ���մ������� */
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'G': /* ���ս���� */
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.dr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.dr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* ���մ���� */
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.cr_bal); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.cr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'R': /*  ���ս���� */
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.ldd_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.ldd_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'S': /*  ���մ���� */
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'X': /* ���� */
			if( flag==0 )
			{
				if( acc_type[0]=='1' )
					strcpy( tmp_acc_name,"�ʲ���ϼ�" );
				else if( acc_type[0]=='2' )
					strcpy( tmp_acc_name,"��ծ��ϼ�" );
				else if( acc_type[0]=='3' )
					strcpy( tmp_acc_name,"������Ȩ����ϼ�" );
				else if( acc_type[0]=='4' )
					strcpy( tmp_acc_name,"�ʲ���ծ��ϼ�" );
				else if( acc_type[0]=='5' )
					strcpy( tmp_acc_name,"������ϼ�" );
			}
			else
				strcpy( tmp_acc_name,"���ڿ�Ŀ��ϼ�" );

			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Y': /* ���� */
			/***
			strcpy( tmp_acc_name, "���ڿ�Ŀ�ϼ�" );
			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);***/
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal+s_gl_mst.ldd_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal+s_gl_mst.ldd_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Z':
			sprintf( amt_tmp, "%.2lf" , s_gl_mst.cr_bal+s_gl_mst.dr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.cr_bal+s_gl_mst.dr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'I': /* ��Ŀ�� */
			pub_base_strpack( s_gl_mst.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item, "acc_hrt='%s'", \
								s_gl_mst.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'J':/* ���ս跢���� */
			vtcp_log( "acc_type====================[%s]",acc_type );
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_mst","ldd_bal",&tmp_amt,"acc_hrt[1]='%c' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0] );
			}
			else 
			{
				ret = sql_sum_double( "gl_mst","ldd_bal",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'M':	/* ���մ������� */
			vtcp_log( "acc_type====================[%s]",acc_type );
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_mst","lcd_bal",&tmp_amt,"acc_hrt[1]='%c' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0] );
			}
			else 
			{
				ret = sql_sum_double( "gl_mst","lcd_bal",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K': /* ���ս跢���� */
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
		case 'L': /* ���մ������� */
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
		case 'O': /* ���ս���� */
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
		case 'P': /* ���մ���� */
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
