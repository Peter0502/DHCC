/*************************************************
* �� �� ��:    sp9814.c
* ����������   
*              ���ݴ����˺�ȡ���˻����
*
* ��    ��:    lance
* ������ڣ�   2003��01��14��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/	  
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
	
sp9814()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	int flag=0, redflag=0;
	char tx_code[5];
	char cif_type[2];
		
	double vbjlx,vbnlx,vbylx,vfllx;
	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	char dc_flg[2];
	char dc_inf[5];
	char dc_flg1[2];
	char dc_inf1[5];
	char dc_flg2[2];
	char dc_inf2[5];
		
	FILE *fp;			
	struct ln_mst_c		ln_mst_tmp;
	struct cif_basic_inf_c cif_basic_inf;
		
	/** ���ݳ�ʼ�� **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );
	memset( &cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c) );
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "1081" , ac_no );	/* �����ʺ� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("1081",ac_no);	/* ������ʺ� */	
	
	get_zd_data( "0650" , tx_code ); /* ���״��� */

	
	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		ERR_DEAL
	} else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
    	}
	sprintf(acErrMsg,"ȡ�����˻�IDΪ[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	/* ���ݴ����˻�ID��ѯ�������ļ����� */
	flag = sql_count("ln_mst", " ac_id=%ld " , g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg,"���ݴ����˻�ID��ѯ�������ļ�����[%d]",flag);
	WRITEMSG	
		
	if( flag==0 )
   	 {
		set_zd_int( "108G", redflag );	/* �Ƿ�������ű�־ 1-Ψһ��¼*/
 		sprintf(acErrMsg,"�������ļ������޴˼�¼[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L002");
 		goto ErrExit;
   	 }	
	 	
	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld " , g_mdm_ac_rel.ac_id );			
		if( ret==100 )
		{
			sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL

		} else if( ret !=0 )
			{
    				sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    				ERR_DEAL
    			}	
    			
    	sprintf(acErrMsg,"ȡ�������ļ�PASS");
	WRITEMSG 

	
	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG
	redflag=1;
	set_zd_long( "1082", ln_mst_tmp.ac_seqn );	/* �����˻���� */
	
	set_zd_int( "108G", redflag );	/* �Ƿ�������ű�־ 1-Ψһ��¼*/
	
	/* �ǿ��Ͽ���ר��ͨ�� */

	
	/* ���ø��ݴ�����ʾ�˺ź��˺����ȡ�������ļ�����*/
	ret = pub_ln_lndis_file(ac_no,ln_mst_tmp.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	/* ��ת��־�ж� */
	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	
	strcpy( dc_flg1 , "1" );
  strcpy( dc_inf1 , "�ֽ�" );
	strcpy( dc_flg , "2" );
  strcpy( dc_inf , "ת��" );
	strcpy( dc_flg2 , "8" );
  strcpy( dc_inf2 , "�ۻ�" );
	set_zd_data("0660",dc_flg);
	set_zd_data("0810",dc_inf);  
	
/**** ����ɳ����Ҫ���޸� �����ʱ�����۶�˽���Թ��ͻ���֧���ֽ��ת��		
	if( g_ln_parm.cif_type[0]=='2' )
	{		    
		pub_base_strpack(dc_inf);
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
	}else{
		pub_base_strpack(dc_inf1);
		pub_base_strpack(dc_inf);
	
		fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
		
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
	}	
***************************/
		fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
		fprintf( fp,"0660|%s|%s|\n",dc_inf2,dc_flg2 );
		
	set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);	
	
	if( strcmp( tx_code , "3202" )==0 )	
	{
			sprintf(acErrMsg,"����3202!");
			WRITEMSG	
						
			/* �����������Ƿ��ǰ��Ҵ��� */
			if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf( acErrMsg,"�˴���Ϊ���Ҵ���,�����ڴ˽���![%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L119" );
				ERR_DEAL
			}	
			sprintf(acErrMsg,"�����������Ƿ�����ͨ���� PASS!");
			WRITEMSG		
			TRACE				
			/* �������Ƿ��Ѿ��ſ� ��ϵͳ�������ⲻ����� modified by liuxuan 2006-11-29
			ret = pub_base_CompDblVal( g_ln_mst.ttl_prvd_amt, 0.00 );
			if( ret==0 )
			{
    				sprintf( acErrMsg,"�˴�����δ�ſ�!");
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L159" );		
				goto ErrExit;
			}		
			sprintf(acErrMsg,"�������Ƿ�ſ� PASS!");
			WRITEMSG
			*/
									
			vbjlx=vbnlx=vbylx=vfllx=0.00;
vtcp_log("--------bjlx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"0",&vbjlx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------bnlx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"1",&vbnlx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------bylx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"2",&vbylx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------fllx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"3",&vfllx,1 );
			if( ret ) { ERR_DEAL }	

vtcp_log("name is [%s]",g_ln_mst.name);
vtcp_log("bal is [%.2lf]",g_ln_mst.bal);
			set_zd_data("1083",g_ln_mst.name);
			set_zd_double("1084",g_ln_mst.bal);
			set_zd_double("1085",g_ln_mst.in_lo_intst);
			set_zd_double("1086",g_ln_mst.out_lo_intst);
			set_zd_double("1087",g_ln_mst.cmpd_lo_intst);
			set_zd_double("1088",vbjlx);
			set_zd_double("1089",vbnlx+vbylx+vfllx);	
			set_zd_data("108J",g_ln_parm.repay_ind);
						   	
			/* ȡ�ͻ����� */
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , "cif_no=%ld" , g_ln_mst.cif_no);
			if( ret )
			{
				goto ErrExit;
			}
			if( cif_basic_inf.type[0]=='1' || cif_basic_inf.type[0]=='5' )
			{
				strcpy( cif_type , "0" );	/* ��˽ */
			}else{
				strcpy( cif_type , "1" );	/* �Թ� */
			}		
			set_zd_data("108P" , cif_type);	   
  }
  
  
	if( strcmp( tx_code , "3304" )==0 )	
	{
			sprintf(acErrMsg,"����3304!");
			WRITEMSG	
														
			vbjlx=vbnlx=vbylx=vfllx=0.00;
vtcp_log("--------bjlx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"0",&vbjlx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------bnlx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"1",&vbnlx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------bylx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"2",&vbylx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------fllx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"3",&vfllx,1 );
			if( ret ) { ERR_DEAL }	

vtcp_log("name is [%s]",g_ln_mst.name);
vtcp_log("bal is [%.2lf]",g_ln_mst.bal);
			set_zd_data("1083",g_ln_mst.name);
			set_zd_double("1084",g_ln_mst.bal);
			set_zd_double("1085",g_ln_mst.in_lo_intst);
			set_zd_double("1086",g_ln_mst.out_lo_intst);
			set_zd_double("1087",g_ln_mst.cmpd_lo_intst);
			set_zd_double("1088",vbjlx);
			set_zd_double("1089",vbnlx+vbylx+vfllx);	
			set_zd_data("108J",g_ln_parm.repay_ind);			   	
			set_zd_double("108B",g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst);
			/* ȡ�ͻ����� */
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , "cif_no=%ld" , g_ln_mst.cif_no);
			if( ret )
			{
				goto ErrExit;
			}
			if( cif_basic_inf.type[0]=='1' || cif_basic_inf.type[0]=='5' )
			{
				strcpy( cif_type , "0" );	/* ��˽ */
			}else{
				strcpy( cif_type , "1" );	/* �Թ� */
			}		
			set_zd_data("108P" , cif_type);	
			   
  }  
  

   	goto OkExit;
  }
    	
    	
  	redflag=9;
	set_zd_int( "108G", redflag );	/* �Ƿ�������ű�־ 1-Ψһ��¼*/	 		
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
