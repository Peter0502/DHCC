/*************************************************
* �� �� ��:  spE607.c
* ���������� ��������(��Ŀ)
*
* ��    ��:  andy
* ������ڣ� 2004-07-04
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "com_item_c.h"
#include "com_tel_c.h"
#include "mo_dc_inf_c.h"
#include "dc_log_c.h"

int spE607()
{

		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];

		char titstr[1024];
		char tel[5];
		char old_acc_hrt[6];
		
		int ttlnum=0;			/**��ȡ��������**/
		int num=0;	
		int i=0;
		int ret=0;
		
		double	dr_cash_amt=0.00;
		long	dr_cash_cnt=0;
		double	dr_ch_amt=0.00;
		long	dr_ch_cnt=0;
		double	dc_cash_amt=0.00;
		long	dc_cash_cnt=0;
		double	dc_ch_amt=0.00;
		long	dc_ch_cnt=0;
		
		char old_acc_no_lvl1[8],old_acc_no_lvl2[8],old_acc_no_lvl3[8];
									
		FILE *fp;
	
	struct dc_log_c		v_dc_log;
	struct com_item_c	v_com_item;
	struct com_tel_c	v_com_tel;
	struct com_item_c	v_com_item_lvl1;
	struct com_item_c	v_com_item_lvl2;
	struct com_item_c	v_com_item_lvl3;
	struct mo_dc_inf_c	v_mo_dc_inf;
				
	memset( &v_dc_log , 0x00 , sizeof(struct trace_log_c));
	memset( &v_com_item , 0x00 , sizeof(struct com_item_c));
	memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
	memset( &v_com_item_lvl1 , 0x00 , sizeof(struct com_item_c));
	memset( &v_com_item_lvl2 , 0x00 , sizeof(struct com_item_c));
	memset( &v_com_item_lvl3 , 0x00 , sizeof(struct com_item_c));
	memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
						
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( old_acc_no_lvl1,0,sizeof(old_acc_no_lvl1) );
	memset( old_acc_no_lvl2,0,sizeof(old_acc_no_lvl2) );
	memset( old_acc_no_lvl3,0,sizeof(old_acc_no_lvl3) );
		
	strcpy( old_acc_hrt , "*****" );
		
	pub_base_sysinit();

	get_zd_data("0910",tel );

	/**��ɲ�ѯ����**/
	pub_base_strpack( tel );
	if( strlen(tel) )
	{
		/* ����Ա���Ƿ���� */
		ret = Com_tel_Sel(g_pub_tx.reply,&v_com_tel,"tel='%s'",tel);
		if(ret==100)
		{
			sprintf(acErrMsg,"�˹�Ա�Ų�����![%s]",tel);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O120" );			
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"��ȡ��Ա��Ϣ���쳣����!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O221" );			
			goto ErrExit;
		}
		
		sprintf( tmpstr," tel='%s' and",tel );
		strcat( wherelist,tmpstr );
	}
	
	/* �������� */
	sprintf( tmpstr," tx_tx_br_no='%s' and",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	/* �������� */
	sprintf( tmpstr," tx_date=%ld and",g_pub_tx.tx_date );
	strcat( wherelist,tmpstr );
		
	/**��ɲ�ѯ**/
	strcat(wherelist," sts='0' ORDER BY acc_hrt ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	

	ret = sql_execute( "delete from mo_dc_inf where br_no='%s' and tel='%s' " , g_pub_tx.tx_br_no , g_pub_tx.tel );
	vtcp_log("ret=[%d]",ret);
        if(ret==-1) ret=0;
	ERR_DEAL	
	
	/* ͳ�����ݹ��� */
	ret=Dc_log_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		sprintf(acErrMsg,">>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		WRITEMSG
		ret=Dc_log_Fet_Sel( &v_dc_log, g_pub_tx.reply);
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"+++++++++++++++++++++ttlnum====[%d]",ttlnum);
		        WRITEMSG
			if( ttlnum )
			{
				/* �����Ŀ������Ϣ */
				ret =pub_base_item_inf( old_acc_hrt, &v_com_item );
				ERR_DEAL
				
				v_mo_dc_inf.dr_cash_amt=dr_cash_amt;				/* �跽�ֽ��� */				
				v_mo_dc_inf.dr_cash_cnt=dr_cash_cnt;				/* �跽�ֽ���� */
				v_mo_dc_inf.dr_ch_amt=dr_ch_amt;				/* �跽ת�˽�� */
				v_mo_dc_inf.dr_ch_cnt=dr_ch_cnt;				/* �跽ת�˱��� */
				v_mo_dc_inf.dc_cash_amt=dc_cash_amt;				/* �����ֽ��� */
				v_mo_dc_inf.dc_cash_cnt=dc_cash_cnt;				/* �����ֽ���� */
				v_mo_dc_inf.dc_ch_amt=dc_ch_amt;				/* ����ת�˽�� */
				v_mo_dc_inf.dc_ch_cnt=dc_ch_cnt;				/* ����ת�˱��� */
				if( v_com_item.acc_no[0]>'5' )					/* ����/�����־ */
				{
					strcpy( v_mo_dc_inf.sts , "1" );
				}else{
					strcpy( v_mo_dc_inf.sts , "0" );
				}
				strcpy( v_mo_dc_inf.tel , g_pub_tx.tel );			/* ��Ա��� */
				strcpy( v_mo_dc_inf.br_no , g_pub_tx.tx_br_no );		/* ������� */	
				sprintf(acErrMsg,"��Ŀ��[%s]����[%s]!",v_com_item.acc_no,v_com_item.acc_lvl);
				WRITEMSG				
				/* ������Ŀ */
				if( v_com_item.acc_lvl[0]=='3' )
				{
					strcpy( v_mo_dc_inf.acc_no_lvl3 , v_com_item.acc_no );		/* ������Ŀ�� */
					strcpy( v_mo_dc_inf.acc_name_lvl3 , v_com_item.acc_name );	/* ������Ŀ���� */
					
					/* ��ÿ�Ŀ������Ŀ��Ϣ */                 
					ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl2 );
					ERR_DEAL	
					strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item_lvl2.acc_no );	/* ������Ŀ�� */
					strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item_lvl2.acc_name );	/* ������Ŀ���� */
					
					/* ��ÿ�Ŀһ����Ŀ��Ϣ */
					ret =pub_base_item_inf( v_com_item_lvl2.up_acc_hrt, &v_com_item_lvl1 );
					ERR_DEAL
					sprintf(acErrMsg,"һ����Ŀ[%s]�����ϼ�[%s]!",v_com_item_lvl1.acc_no,v_com_item_lvl2.up_acc_hrt);
					WRITEMSG
											
					strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* һ����Ŀ�� */
					strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* һ����Ŀ���� */												
				
				}else if( v_com_item.acc_lvl[0]=='2' ){		/* ������Ŀ */
					strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item.acc_no );		/* ������Ŀ�� */
					strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item.acc_name );	/* ������Ŀ���� */					

					/* ��ÿ�Ŀһ����Ŀ��Ϣ */
					ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl1 );
					ERR_DEAL	
					strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* һ����Ŀ�� */
					strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* һ����Ŀ���� */												
				}else if( v_com_item.acc_lvl[0]=='1' ){		/* һ����Ŀ */
					strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item.acc_no );		/* һ����Ŀ�� */
					strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item.acc_name );	/* һ����Ŀ���� */
				}else{
					sprintf(acErrMsg,"�޴˼����Ŀ��!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"S056" );			
					goto ErrExit;		
				}								
								
				/* �Ǽ�ͳ�Ʊ� */

				
				ret = Mo_dc_inf_Ins( v_mo_dc_inf , g_pub_tx.reply );
				if( ret )
				{
					sprintf(acErrMsg,"�Ǽ����˿�Ŀͳ�Ʊ��쳣����,�������Ļ�����ϵ!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"G106" );			
					goto ErrExit;					
				}
				
				strcpy( old_acc_hrt , v_dc_log.acc_hrt);	/* ��Ŀ������ */ 	
				
				/* �������� */
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;
				memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
				memset( &v_com_item_lvl1 , 0x00 , sizeof(struct com_item_c));
				memset( &v_com_item_lvl2 , 0x00 , sizeof(struct com_item_c));
				memset( &v_com_item_lvl3 , 0x00 , sizeof(struct com_item_c));
				memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));				
			}
			sprintf(acErrMsg,"-----------------------------");
			WRITEMSG
			break;
			
		}else if(ret){
			goto ErrExit;
		}
		
		sprintf(acErrMsg,">>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		WRITEMSG
		/* ����Ŀ������Ϊ�� */		
		if( strlen(v_dc_log.acc_hrt)==0 )
		{
			strcpy( v_dc_log.acc_hrt , "��" );
		}
		
		/* ��һ����¼ */
		if( strcmp( old_acc_hrt , "*****")==0 )
		{
			TRACE
			strcpy( old_acc_hrt, v_dc_log.acc_hrt );
		}		
		
		
		if( strcmp( v_dc_log.acc_hrt , old_acc_hrt ) )	/* ��Ŀ�����ֱ仯ʱ */
		{
		
	sprintf(acErrMsg,"��Ŀ�����ֱ仯ʱ");
	WRITEMSG
			/* �����Ŀ������Ϣ */
			ret =pub_base_item_inf( old_acc_hrt, &v_com_item );
			ERR_DEAL
						
			v_mo_dc_inf.dr_cash_amt=dr_cash_amt;				/* �跽�ֽ��� */				
			v_mo_dc_inf.dr_cash_cnt=dr_cash_cnt;				/* �跽�ֽ���� */
			v_mo_dc_inf.dr_ch_amt=dr_ch_amt;				/* �跽ת�˽�� */
			v_mo_dc_inf.dr_ch_cnt=dr_ch_cnt;				/* �跽ת�˱��� */
			v_mo_dc_inf.dc_cash_amt=dc_cash_amt;				/* �����ֽ��� */
			v_mo_dc_inf.dc_cash_cnt=dc_cash_cnt;				/* �����ֽ���� */
			v_mo_dc_inf.dc_ch_amt=dc_ch_amt;				/* ����ת�˽�� */
			v_mo_dc_inf.dc_ch_cnt=dc_ch_cnt;				/* ����ת�˱��� */
			if( v_com_item.acc_no[0]>'5' )					/* ����/�����־ */
			{
				strcpy( v_mo_dc_inf.sts , "1" );
			}else{
				strcpy( v_mo_dc_inf.sts , "0" );
			}
			strcpy( v_mo_dc_inf.tel , g_pub_tx.tel );			/* ��Ա��� */
			strcpy( v_mo_dc_inf.br_no , g_pub_tx.tx_br_no );		/* ������� */	
			
			/* ������Ŀ */
			if( v_com_item.acc_lvl[0]=='3' )
			{
				strcpy( v_mo_dc_inf.acc_no_lvl3 , v_com_item.acc_no );		/* ������Ŀ�� */
				strcpy( v_mo_dc_inf.acc_name_lvl3 , v_com_item.acc_name );	/* ������Ŀ���� */
				
				/* ��ÿ�Ŀ������Ŀ��Ϣ */
				ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl2 );
				ERR_DEAL	
				strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item_lvl2.acc_no );	/* ������Ŀ�� */
				strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item_lvl2.acc_name );	/* ������Ŀ���� */
				
				/* ��ÿ�Ŀһ����Ŀ��Ϣ */
				ret =pub_base_item_inf( v_com_item_lvl2.up_acc_hrt, &v_com_item_lvl1 );
				ERR_DEAL	
				strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* һ����Ŀ�� */
				strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* һ����Ŀ���� */												
			
			}else if( v_com_item.acc_lvl[0]=='2' ){		/* ������Ŀ */
				strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item.acc_no );		/* ������Ŀ�� */
				strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item.acc_name );	/* ������Ŀ���� */					

				/* ��ÿ�Ŀһ����Ŀ��Ϣ */
				ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl1 );
				ERR_DEAL	
				strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* һ����Ŀ�� */
				strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* һ����Ŀ���� */												
			}else if( v_com_item.acc_lvl[0]=='1' ){		/* һ����Ŀ */
				strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item.acc_no );		/* һ����Ŀ�� */
				strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item.acc_name );	/* һ����Ŀ���� */
			}else{
				sprintf(acErrMsg,"�޴˼����Ŀ��!lvl[%s]acc_hrt[%s]",v_com_item.acc_lvl,old_acc_hrt);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"S056" );			
				goto ErrExit;		
			}								
							
			/* �Ǽ�ͳ�Ʊ� */

			ret = Mo_dc_inf_Ins( v_mo_dc_inf , g_pub_tx.reply );
			if( ret )
			{
				sprintf(acErrMsg,"�Ǽ����˿�Ŀͳ�Ʊ��쳣����,�������Ļ�����ϵ!");
				WRITEMSG
				strcpy( g_pub_tx.reply ,"G106" );			
				goto ErrExit;					
			}
			
			strcpy( old_acc_hrt , v_dc_log.acc_hrt);	/* ��Ŀ������ */ 	
			
			/* �������� */
			dr_cash_amt=0.00;
			dr_cash_cnt=0;
			dr_ch_amt=0.00;
			dr_ch_cnt=0;
			dc_cash_amt=0.00;
			dc_cash_cnt=0;
			dc_ch_amt=0.00;
			dc_ch_cnt=0;
			memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
			memset( &v_com_item_lvl1 , 0x00 , sizeof(struct com_item_c));
			memset( &v_com_item_lvl2 , 0x00 , sizeof(struct com_item_c));
			memset( &v_com_item_lvl3 , 0x00 , sizeof(struct com_item_c));
			memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));						
		}
		
		
		/* ͳ����Ϣ */
		if( pub_base_CompDblVal( v_dc_log.amt , 0.00 ) )
		{
			if( v_dc_log.ct_ind[0]=='1' )		/* �ֽ� */
			{
				if( v_dc_log.dc_ind[0]=='1')	/* �跽 */
				{
					dr_cash_amt+=v_dc_log.amt;
					dr_cash_cnt++;
				}
				if( v_dc_log.dc_ind[0]=='2')	/* ���� */
				{
					dc_cash_amt+=v_dc_log.amt;
					dc_cash_cnt++;
				}
			}
			if( v_dc_log.ct_ind[0]=='2' )		/* ת�� */
			{
				if( v_dc_log.dc_ind[0]=='1')	/* �跽 */
				{
					dr_ch_amt+=v_dc_log.amt;
					dr_ch_cnt++;
				}
				if( v_dc_log.dc_ind[0]=='2')	/* ���� */
				{
					dc_ch_amt+=v_dc_log.amt;
					dc_ch_cnt++;
				}
			}			
		}	
		
		ttlnum++;
	}

	sprintf(acErrMsg,"===-=--=-=--=-=-=----=-=-=-");
	WRITEMSG
		sprintf(acErrMsg,"hello");
WRITEMSG
	ret=Dc_log_Clo_Sel();
	sprintf(acErrMsg,"hello");
WRITEMSG
	ERR_DEAL
	sprintf(acErrMsg,"hello");
WRITEMSG
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}else{
/***JYW		fclose(fp); **/
		set_zd_data( DC_FILE_SND,"1" );
	}
		
	
	/** д��ʾ�ļ� **/
	
	ret=Mo_dc_inf_Dec_Sel( g_pub_tx.reply,"br_no='%s' and tel='%s' order by acc_no_lvl1,acc_no_lvl2,acc_no_lvl3",g_pub_tx.tx_br_no,g_pub_tx.tel );
	ERR_DEAL

		num=0;
	sprintf(acErrMsg,"===-=--=-=--=-=-=----=-=-=-");
	WRITEMSG	
	while(1)
	{
		ret=Mo_dc_inf_Fet_Sel( &v_mo_dc_inf, g_pub_tx.reply );
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"<><><><><><><><><><><>");
			WRITEMSG
			if( num )
			{
				/* �ֱ�д����/�����Ŀͳ����Ϣ */
				/* ������Ϣ */
				ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
				
				ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
																
				/**��ʾ�б���**/
				fprintf( fp,"@3v1\n");
				fprintf( fp,"`\n" );
				fprintf( fp,"`     �� Ŀ ��: ����    �ڡ�\n");
				fprintf( fp,"`     ��Ŀ����: �����ձ��ڿ�Ŀ������Ϣ��\n" );
				fprintf( fp,"`     ��Ŀ����: �����ڿ�Ŀ��\n" );	
				fprintf( fp,"`\n" );
				fprintf( fp,"`    �������ө����������������������������ө�����������������������������\n" );
				fprintf( fp,"`    ��    ��          ��       ��       ��           ��      ��       ��\n" );
				fprintf( fp,"`    ��    �������������������ө����������㩥���������������ө�����������\n" );										
				fprintf( fp,"`    ��    ��   ��      ��   ��  ��  ��  ��   ��      ��   ��  ��  ��  ��\n" );
				fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
				fprintf( fp,"`    ���ֽ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
				fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
				fprintf( fp,"`    ��ת�˩�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
				fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
				fprintf( fp,"`    ���ϼƩ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
				fprintf( fp,"`    �������۩����������������۩����������۩����������������۩�����������\n" );		
				fprintf( fp,"`\n" );			
				fprintf( fp,"`\n" );
				
				/* �������� */				
				
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;
				
				/* ������Ϣ */
				ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
				
				ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
																
				/**��ʾ�б���**/
				fprintf( fp,"@3v1\n");
				fprintf( fp,"`\n" );
				fprintf( fp,"`     �� Ŀ ��: ����    �⡿\n");
				fprintf( fp,"`     ��Ŀ����: �����ձ����Ŀ������Ϣ��\n" );
				fprintf( fp,"`     ��Ŀ����: �������Ŀ��\n" );	          
				fprintf( fp,"`\n" );
				fprintf( fp,"`    �������ө����������������������������ө�����������������������������\n" );
				fprintf( fp,"`    ��    ��          ��       ��       ��           ��      ��       ��\n" );
				fprintf( fp,"`    ��    �������������������ө����������㩥���������������ө�����������\n" );										
				fprintf( fp,"`    ��    ��   ��      ��   ��  ��  ��  ��   ��      ��   ��  ��  ��  ��\n" );
				fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
				fprintf( fp,"`    ���ֽ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
				fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
				fprintf( fp,"`    ��ת�˩�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
				fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
				fprintf( fp,"`    ���ϼƩ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
				fprintf( fp,"`    �������۩����������������۩����������۩����������������۩�����������\n" );		
				fprintf( fp,"`\n" );			
				fprintf( fp,"`\n" );
				
				/* �������� */				
				
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;							
			
			}
			
			break;
			
		}else if(ret){
			goto ErrExit;
		}						
		if( !num )
		{
			pub_base_AllDwnFilName( filename );
			vtcp_log("filename=[%s]",filename);
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
		}
		/* ͳ��һ����Ŀ��Ϣ */
		if( strcmp(  v_mo_dc_inf.acc_no_lvl1 , old_acc_no_lvl1 ) && strlen(v_mo_dc_inf.acc_no_lvl1)!=0 )
		{
			ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);			
			ERR_DEAL
			ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);		
			ERR_DEAL
			
			ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);			
			ERR_DEAL
			ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);		
			ERR_DEAL
															
			/**��ʾ�б���**/
			fprintf( fp,"@3v1\n");
			fprintf( fp,"`\n" );		
			fprintf( fp,"`     �� Ŀ ��: ��%7s ��\n" , v_mo_dc_inf.acc_no_lvl1 );
			fprintf( fp,"`     ��Ŀ����: ��%30s��\n" , v_mo_dc_inf.acc_name_lvl1);
			fprintf( fp,"`     ��Ŀ����: ��һ����Ŀ��\n" );				
			fprintf( fp,"`\n" );
			fprintf( fp,"`    �������ө����������������������������ө�����������������������������\n" );
			fprintf( fp,"`    ��    ��          ��       ��       ��           ��      ��       ��\n" );
			fprintf( fp,"`    ��    �������������������ө����������㩥���������������ө�����������\n" );										
			fprintf( fp,"`    ��    ��   ��      ��   ��  ��  ��  ��   ��      ��   ��  ��  ��  ��\n" );
			fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
			fprintf( fp,"`    ���ֽ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
			fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
			fprintf( fp,"`    ��ת�˩�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
			fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
			fprintf( fp,"`    ���ϼƩ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
			fprintf( fp,"`    �������۩����������������۩����������۩����������������۩�����������\n" );		
			fprintf( fp,"`\n" );			
			fprintf( fp,"`\n" );
			
			/* �������� */		
			dr_cash_amt=0.00;
			dr_cash_cnt=0;
			dr_ch_amt=0.00;
			dr_ch_cnt=0;
			dc_cash_amt=0.00;
			dc_cash_cnt=0;
			dc_ch_amt=0.00;
			dc_ch_cnt=0;
			
			strcpy( old_acc_no_lvl1 , v_mo_dc_inf.acc_no_lvl1 );
			memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
		}else{
			continue;
		}
		
		/* ��ѯ������Ŀ��Ϣ */
		ret=Mo_dc_inf_Dec_Sel1( g_pub_tx.reply,"br_no='%s' and tel='%s' and acc_no_lvl1='%s' order by acc_no_lvl1,acc_no_lvl2,acc_no_lvl3",g_pub_tx.tx_br_no,g_pub_tx.tel,old_acc_no_lvl1 );
		ERR_DEAL				
		while(1)
		{
			ret=Mo_dc_inf_Fet_Sel1( &v_mo_dc_inf, g_pub_tx.reply );
			if( ret==100 ) 
			{
				break;			
			}else if(ret){
				goto ErrExit;
			}
			/* ��ӡ������Ŀ��Ϣ */
			if( strcmp(  v_mo_dc_inf.acc_no_lvl2 , old_acc_no_lvl2 ) && strlen(v_mo_dc_inf.acc_no_lvl2)!=0 )
			{
				ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);			
				ERR_DEAL
				ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);		
				ERR_DEAL
				
				ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);			
				ERR_DEAL
				ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);		
				ERR_DEAL
																
				/**��ʾ�б���**/
				fprintf( fp,"@3v1\n");
				fprintf( fp,"`\n" );
				fprintf( fp,"`     �� Ŀ ��: ��%7s ��\n" , v_mo_dc_inf.acc_no_lvl2 );
				fprintf( fp,"`     ��Ŀ����: ��%30s��\n" , v_mo_dc_inf.acc_name_lvl2);
				fprintf( fp,"`     ��Ŀ����: ��������Ŀ��\n" );	
				fprintf( fp,"`\n" );
				fprintf( fp,"`    �������ө����������������������������ө�����������������������������\n" );
				fprintf( fp,"`    ��    ��          ��       ��       ��           ��      ��       ��\n" );
				fprintf( fp,"`    ��    �������������������ө����������㩥���������������ө�����������\n" );										
				fprintf( fp,"`    ��    ��   ��      ��   ��  ��  ��  ��   ��      ��   ��  ��  ��  ��\n" );
				fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
				fprintf( fp,"`    ���ֽ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
				fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
				fprintf( fp,"`    ��ת�˩�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
				fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
				fprintf( fp,"`    ���ϼƩ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
				fprintf( fp,"`    �������۩����������������۩����������۩����������������۩�����������\n" );		
				fprintf( fp,"`\n" );			
				fprintf( fp,"`\n" );
				
				/* �������� */		
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;
				
				strcpy( old_acc_no_lvl2 , v_mo_dc_inf.acc_no_lvl2 );
				memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
			}else{
				continue;
			}
			
			/* ��ѯ������Ŀ��Ϣ */
			ret=Mo_dc_inf_Dec_Sel2( g_pub_tx.reply,"br_no='%s' and tel='%s' and acc_no_lvl1='%s' and acc_no_lvl2='%s' order by acc_no_lvl1,acc_no_lvl2,acc_no_lvl3",g_pub_tx.tx_br_no,g_pub_tx.tel,old_acc_no_lvl1,old_acc_no_lvl2 );
			ERR_DEAL				
			while(1)
			{
				ret=Mo_dc_inf_Fet_Sel2( &v_mo_dc_inf, g_pub_tx.reply );
				if( ret==100 ) 
				{
					break;			
				}else if(ret){
					goto ErrExit;
				}
				/* ��ӡ������Ŀ��Ϣ */
				if( strcmp(  v_mo_dc_inf.acc_no_lvl3 , old_acc_no_lvl3 ) && strlen(v_mo_dc_inf.acc_no_lvl3)!=0)
				{
					ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);			
					ERR_DEAL
					ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);		
					ERR_DEAL
					
					ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);			
					ERR_DEAL
					ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);		
					ERR_DEAL
																	
					/**��ʾ�б���**/
					fprintf( fp,"@3v1\n");
					fprintf( fp,"`\n" );
					fprintf( fp,"`     �� Ŀ ��: ��%7s ��\n" , v_mo_dc_inf.acc_no_lvl3 );
					fprintf( fp,"`     ��Ŀ����: ��%30s��\n" , v_mo_dc_inf.acc_name_lvl3);
					fprintf( fp,"`     ��Ŀ����: ��������Ŀ��\n" );	
					fprintf( fp,"`\n" );
					fprintf( fp,"`    �������ө����������������������������ө�����������������������������\n" );
					fprintf( fp,"`    ��    ��          ��       ��       ��           ��      ��       ��\n" );
					fprintf( fp,"`    ��    �������������������ө����������㩥���������������ө�����������\n" );										
					fprintf( fp,"`    ��    ��   ��      ��   ��  ��  ��  ��   ��      ��   ��  ��  ��  ��\n" );
					fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
					fprintf( fp,"`    ���ֽ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
					fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
					fprintf( fp,"`    ��ת�˩�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
					fprintf( fp,"`    �������㩥���������������㩥���������㩥���������������㩥����������\n" );
					fprintf( fp,"`    ���ϼƩ�%16.2lf��%10d��%16.2lf��%10d��\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
					fprintf( fp,"`    �������۩����������������۩����������۩����������������۩�����������\n" );		
					fprintf( fp,"`\n" );			
					fprintf( fp,"`\n" );
					
					/* �������� */		
					dr_cash_amt=0.00;
					dr_cash_cnt=0;
					dr_ch_amt=0.00;
					dr_ch_cnt=0;
					dc_cash_amt=0.00;
					dc_cash_cnt=0;
					dc_ch_amt=0.00;
					dc_ch_cnt=0;
					
					strcpy( old_acc_no_lvl3 , v_mo_dc_inf.acc_no_lvl3 );
					memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
				}else{
					continue;
				}			
			num++;
			}
			ret=Mo_dc_inf_Clo_Sel2( );
			ERR_DEAL
		num++;
		}					
		ret=Mo_dc_inf_Clo_Sel1( );
		ERR_DEAL
	num++;
	}
	ret=Mo_dc_inf_Clo_Sel( );
	ERR_DEAL
		
	fclose(fp);
	set_zd_data( DC_FILE_SND,"1" );
	

GoodExit:
	
	ret = sql_execute( "delete from  mo_dc_inf where br_no='%s' and tel='%s' " , g_pub_tx.tx_br_no , g_pub_tx.tel );
	ERR_DEAL
	
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
