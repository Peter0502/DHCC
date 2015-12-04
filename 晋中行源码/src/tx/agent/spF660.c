/*************************************************
* �� �� ��: spF660.c
* �������������������������
*
* ��    ��: jack
* ������ڣ�2004��06��29��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "com_branch_c.h"
#include "com_rate_c.h"
#include "ag_peritemmap_c.h"
#include "hv_brno_orno_c.h"

int spF660()
{
	int ret = 0;

	char cAcno[20];
	char cRcno[17];
	char cRctp[5];
	char cUnit_no[5];
	char cur_name[11]; /**��������**/
	char mbfebkno[13]; /**����к�**/

	char term_type[3],deac_type[11],term[3];

	char filename[64];
	FILE *fp = NULL;

	char cDraw_type[15];
	char cSttl_type[11];
	char br_name[31];

	struct mdm_ac_rel_c sMdm_ac_rel;
	
	struct dd_mst_c sDd_mst;
	struct dd_mst_hst_c sDd_mst_hst;
	struct dd_parm_c sDd_parm;
	struct ag_peritemmap_c ag_peritemmap;
	struct td_mst_c sTd_mst;
	struct td_mst_hst_c sTd_mst_hst;
	struct td_parm_c sTd_parm;
	struct hv_brno_orno_c   g_hv_brno_orno;

	
	struct com_rate_c sCom_rate;
	
	struct com_branch_c sCom_branch;

	/*** ��ʼ��ȫ�ֱ���***/
	pub_base_sysinit();
    
	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cRcno, 0x0, sizeof(cRcno) );
	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	memset( cRctp, 0x0, sizeof(cRctp) );
	memset( cur_name, 0x0, sizeof(cur_name) );
	memset( br_name, 0x0, sizeof(br_name) );
	memset(&ag_peritemmap,0x00,sizeof(struct ag_peritemmap_c));

	memset(&g_hv_brno_orno,0x00,sizeof(g_hv_brno_orno));
	memset(mbfebkno,0x00,sizeof(mbfebkno));

	get_zd_data( "0370", cAcno );
	sprintf( acErrMsg,"----->�˻�[%s]", cAcno );
	WRITEMSG
	pub_base_old_acno( cAcno );
	sprintf( acErrMsg,"----->�˻�[%s]", cAcno );
	WRITEMSG
	get_zd_data( "0580", cRcno );
	sprintf( acErrMsg,"----->pingzheng[%s]", cRcno );
	WRITEMSG
	get_zd_data( "1161", cRctp );
	sprintf( acErrMsg,"----->leixing[%s]", cRctp );
	WRITEMSG
	get_zd_data( "0920", cUnit_no );

	memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel));
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel, \
		"ac_no = '%s' and mdm_code = '%s' and note_no = '%s'", \
		cAcno, cRctp, cRcno );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "L015" );
		sprintf( acErrMsg,"----->��ȡ�������˻���ϵ��Ϣ�쳣[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	pub_base_strpack( sMdm_ac_rel.note_no );
	pub_base_strpack( sMdm_ac_rel.mdm_code );

	if ( strcmp( cRctp, "0010" ) == 0 )
	{
		strcpy( cRctp, "010" );	/*����*/
	}
	else if ( strcmp( cRctp, "0011" ) == 0 )
	{
		strcpy( cRctp, "011" );	/*��������*/
	}
	else if ( strcmp( cRctp, "0012" ) == 0 )
	{
		strcpy( cRctp, "012" );	/*�ؿ��浥*/
	}
	else if ( strcmp( cRctp, "0013" ) == 0 )
	{
		strcpy( cRctp, "013" );	/*�������ڴ浥*/
	}
	else if ( strcmp( cRctp, "0015" ) == 0 )
	{
		strcpy( cRctp, "015" );	/*��������浥*/
	}
	else if ( strcmp( cRctp, "0016" ) == 0 )
	{
		strcpy( cRctp, "016" );	/*����һ��ͨ*/
	}
	
	memset( cDraw_type, 0x0, sizeof(cDraw_type) );
	if(sMdm_ac_rel.draw_pwd_yn[0]=='Y' && \
			sMdm_ac_rel.draw_id_yn[0]=='N')
	{
		strcpy(cDraw_type,"ƾ��֧ȡ");
	}
	else if(sMdm_ac_rel.draw_pwd_yn[0]=='N' && \
			sMdm_ac_rel.draw_id_yn[0]=='Y')
	{
		strcpy(cDraw_type,"֤��֧ȡ");
	}
	else if( sMdm_ac_rel.draw_pwd_yn[0]=='Y' && \
			sMdm_ac_rel.draw_id_yn[0]=='Y')
	{
		strcpy(cDraw_type,"�����֤��֧ȡ");
	}
	else 
	{
		strcpy(cDraw_type,"");
	}	
	
		
	memset( &sCom_branch, 0x0, sizeof(sCom_branch) );
	ret = Com_branch_Sel( g_pub_tx.reply, &sCom_branch, \
		"br_no = '%s'", sMdm_ac_rel.opn_br_no );
	
	memset( filename, 0x0, sizeof(filename) );
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		strcpy( g_pub_tx.reply, "P159" );
		sprintf( acErrMsg,"��д������Ϣ�ļ���!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	if ( strcmp( cRctp, "010" ) == 0 )	/*���ڴ���*/
	{
		memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
			"ac_id = %ld", sMdm_ac_rel.ac_id );
		if ( ret )      
		{               
			strcpy( g_pub_tx.reply, "B114" );
			sprintf( acErrMsg,"----->��ȡ�������ļ���Ϣ�쳣[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}               

		memset( &sDd_mst_hst, 0x0, sizeof(sDd_mst_hst) );
		ret = Dd_mst_hst_Sel( g_pub_tx.reply, &sDd_mst_hst, \
			"ac_id = %ld and hst_cnt = 1", sMdm_ac_rel.ac_id );
		if ( ret )      
		{               
			strcpy( g_pub_tx.reply, "B114" );
			sprintf( acErrMsg, "---->��ȡ������ϸ�п�����Ϣ�쳣[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}               
        	                
		memset( &sDd_parm, 0x0, sizeof(sDd_parm) );
		ret = Dd_parm_Sel( g_pub_tx.reply, &sDd_parm, \
			"prdt_no = '%s'", sDd_mst.prdt_no );
		if ( ret )      
		{               
			strcpy( g_pub_tx.reply, "A020" );
			sprintf( acErrMsg, "---->��ȡ������ϸ�п�����Ϣ�쳣[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}		        	                
        	                
		pub_base_strpack( sDd_mst.name );
		pub_base_strpack( sDd_parm.title );		

		memset( cSttl_type, 0x0, sizeof(cSttl_type) );
		if ( sDd_mst.sttl_type[0] == 'D' )
		{               
			strcpy( cSttl_type, "���˽��㻧" );
		}               
		if ( sDd_mst.sttl_type[0] == 'S' )
		{               
			strcpy( cSttl_type, "���˴��" );
		}
		/***���ر��ֺ�����  add by wanglei 20061023***/	
		pub_base_dic_show_str(cur_name,"cur_name",sDd_parm.cur_no);
		pub_base_getbrname(sDd_mst.opn_br_no,br_name);/****�õ�������������****/
		/****���ӷ��ش���к� add by wanglei 20071120****/
		ret = Hv_brno_orno_Sel(g_pub_tx.reply,&g_hv_brno_orno,"br_no = '%s' ",g_pub_tx.tx_br_no);                                                                   
		if(ret )
		{
			sprintf(acErrMsg,"����[%s]��Ӧ��֧���кŲ�����!\n",g_pub_tx.tx_br_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P099");
			goto ErrExit;
		}
		strcpy(mbfebkno,  g_hv_brno_orno.or_br_no);
		vtcp_log("[%s][%d] ��������Ϊ[%s],����к�Ϊ [%s]\n",__FILE__,__LINE__,g_pub_tx.tx_br_no,mbfebkno);
		/****ended 20071120****/

                fprintf( fp, "CZFY%s|%s|%ld|%s|%ld|%s|%s|%s|%lf|%s|%s|%lf|%s|%lf|%s|%s|\n", \
			cAcno, sDd_mst.name, sDd_mst.cif_no, cSttl_type, \
			sDd_mst.opn_date, cDraw_type, sDd_parm.title, \
			"����", sDd_mst_hst.tx_amt, sDd_mst_hst.tel, sCom_branch.br_tele, sDd_mst_hst.bal, cur_name, sDd_mst.rate,br_name,mbfebkno);
	}
	else	/*��������ͬ�Ķ��ڣ�����*/
	{
		memset( &sTd_mst, 0x0, sizeof(sTd_mst) );
		ret = Td_mst_Sel( g_pub_tx.reply, &sTd_mst, \
			"ac_id = %ld", sMdm_ac_rel.ac_id );
		if ( ret )
		{
			strcpy( g_pub_tx.reply, "B114" );
			sprintf( acErrMsg,"----->��ȡ�������ļ���Ϣ�쳣[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}               

		memset( &sTd_mst_hst, 0x0, sizeof(sTd_mst_hst) );
		ret = Td_mst_hst_Sel( g_pub_tx.reply, &sTd_mst_hst, \
			"ac_id = %ld and hst_cnt = 1", sMdm_ac_rel.ac_id );
		if ( ret )      
		{               
			strcpy( g_pub_tx.reply, "B114" );
			sprintf( acErrMsg, "---->��ȡ������ϸ�п�����Ϣ�쳣[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}               

		memset( &sTd_parm, 0x0, sizeof(sTd_parm) );
		ret = Td_parm_Sel( g_pub_tx.reply, &sTd_parm, \
			"prdt_no = '%s'", sTd_mst.prdt_no );
		if ( ret )      
		{
			strcpy( g_pub_tx.reply, "A020" );
			sprintf( acErrMsg, "---->��ȡ������ϸ�п�����Ϣ�쳣[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		memset( &sCom_rate, 0x0, sizeof(sCom_rate) );
		ret = Com_rate_Sel( g_pub_tx.reply, &sCom_rate, \
			"rate_code = '%s'", sTd_parm.rate_no );

		pub_base_strpack( sTd_mst.name );
		pub_base_strpack( sTd_parm.title );	
		
		if ( strcmp( cRctp, "011" ) == 0 )	/*��������*/
		{
			fprintf( fp, "CZFY%s|%s|%ld|%ld|%s|%s|%s|%.2lf|%s|%s|\n", \
				cAcno, sTd_mst.name, sTd_mst.cif_no, \
				sTd_mst.opn_date, cDraw_type, sTd_parm.title, \
				"����", sTd_mst_hst.tx_amt, sTd_mst_hst.tel, sCom_branch.br_tele );
		}
		else if ( !strcmp( cRctp, "012" ) || \
			!strcmp( cRctp, "013" )  )	/*�������ڴ浥*/
		{
			memset( &g_td_mst, 0x0, sizeof(g_td_mst) );
			ret = Td_mst_Sel( g_pub_tx.reply, &g_td_mst, \
				"ac_id = %ld", sMdm_ac_rel.ac_id );
			if ( ret )
			{
				strcpy( g_pub_tx.reply, "B114" );
				sprintf( acErrMsg,"----->��ȡ�������ļ���Ϣ�쳣[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
                	
			memset( &g_td_parm, 0x0, sizeof(g_td_parm) );
			ret = Td_parm_Sel( g_pub_tx.reply, &g_td_parm, \
				"prdt_no = '%s'", g_td_mst.prdt_no );
			if ( ret )      
			{               
				strcpy( g_pub_tx.reply, "A020" );
				sprintf( acErrMsg, "---->��ȡ������ϸ�п�����Ϣ�쳣[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			strcpy( g_pub_tx.cur_no,"01");
			g_pub_tx.tx_amt1 = sTd_mst_hst.tx_amt;
			g_pub_tx.tx_date = g_td_mst.mtr_date;
			g_pub_intst.edu_ind[0]='Y';
			memset(term_type,'\0',sizeof(term_type)); memset(term,'\0',sizeof(term));
	if(memcmp(g_td_parm.prdt_no,"241",3)!=0){
			if(g_td_parm.term_type[0]=='Y')
				strcpy(term_type,"��");
			else if(g_td_parm.term_type[0]=='M')
			   	strcpy(term_type,"��");
			else if(g_td_parm.term_type[0]=='D')
				strcpy(term_type,"��");
			sprintf(term,"%02d",g_td_parm.term);
	}
		pub_base_dic_show_str(deac_type,"deac_type",g_pub_tx.ac_type);

			memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
			ret = pub_base_CalIntstTd( &g_pub_intst.dealval,
        		        &g_pub_intst.factval,&g_pub_intst.val, \
						&g_pub_intst.keepval,
        		        g_pub_tx.reply);
        	if( ret )
        	{
        	    sprintf(acErrMsg,"ȡ������Ϣֵ����");
        	    WRITEMSG
        	    strcpy( g_pub_tx.reply,"O194" );
        	    goto ErrExit;
        	}
			/*�������˺ţ��������գ����ڣ����ʣ������գ�������Ϣ��֧ȡ��ʽ��������Ա,����˵��*/
			fprintf( fp,"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
				cAcno,sTd_mst.name,sTd_mst_hst.tx_amt,\
				g_td_parm.title,g_td_mst.ic_date,g_td_mst.opn_br_no,\
				g_td_mst.cif_no,g_td_mst.mtr_date,term,term_type, \
				g_td_mst.rate,sTd_mst_hst.tx_amt," ", \
				cDraw_type," ",cur_name,g_pub_tx.tx_code,"ת��", \
				cAcno,g_pub_intst.keepval+g_pub_intst.dealval, \
				"ת��",g_pub_tx.ac_seqn,g_td_mst.ic_date, \
				" ",g_td_mst.prdt_no,0,g_pub_tx.brf, \
				" "," ",sTd_mst_hst.tel," ", \
				" ",sCom_branch.br_name,sCom_branch.br_no,sMdm_ac_rel.note_no," ",cAcno);
		}
		else if ( strcmp( cRctp, "015" ) == 0 )	/*���������浥*/
		{
			/*�������˺ţ��������գ�֧ȡ��ʽ��������Ա*/
			fprintf( fp, "CZFY%s|%s|%.2lf|%ld|%s|%s|\n", \
				sTd_mst.name, cAcno, sTd_mst_hst.tx_amt, sTd_mst.ic_date, \
				cDraw_type, sTd_mst_hst.tel );
		}
		else if ( strcmp( cRctp, "016" ) == 0 )	/*����һ��ͨ*/
		{
		 /*�˺ţ������������գ�֧ȡ��ʽ���������գ����200�����ڣ���Ϣ�գ���ֹ�գ������ʣ�������Ա��*/
			fprintf( fp, "CZFY%s|%s|%ld|%s|%d|%ld|%ld|%lf|%lf|%s|%s|%s|\n", \
				cAcno, sTd_mst.name, sTd_mst.opn_date, cDraw_type, \
				sTd_parm.term, sTd_mst.ic_date, sTd_mst.mtr_date, sTd_mst_hst.tx_amt, \
				sCom_rate.rate_val, sTd_mst_hst.tel, "200", "����" );
		}
	}
	ret=Ag_peritemmap_Sel( g_pub_tx.reply,&ag_peritemmap,"unit_no='%s' and ac_no='%s'",cUnit_no,cAcno);
	if( ret )
  {
        	    sprintf(acErrMsg,"ȡ����λ���˶��ձ����");
        	    WRITEMSG
        	    strcpy( g_pub_tx.reply,"T062" );
        	    goto ErrExit;
  }
  ag_peritemmap.remark[8]='1';
  ret=Ag_peritemmap_Upd_Upd(ag_peritemmap,g_pub_tx.reply);
  if( ret )
  {
          	  goto ErrExit;	
  }
	fclose( fp );

	set_zd_data( DC_FILE_SND, "1" );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg,"��������������۳ɹ�!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���������������ʧ��[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
