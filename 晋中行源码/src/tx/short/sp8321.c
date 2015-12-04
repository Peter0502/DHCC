/*************************************************
* �� �� ��:  sp8321.c
* ���������� ��̫�����ɳ���¼��ѯ
*
* ��    ��:  wp
* ������ڣ� 
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include "public.h"
#include "mdm_attr_c.h"
#include "dd_mst_hst_c.h"
#include "mdm_unprt_hst_c.h"

sp8321()
{
	int ret=0;
	char ac_type[2];
	int unprt_cnt;
	long b_date;
	long e_date;
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[512];
	struct mdm_attr_c	g_mdm_attr;
	struct dd_mst_hst_c	g_dd_mst_hst;
	struct mdm_unprt_hst_c	g_mdm_unprt_hst;

	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_dd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
	memset(&g_mdm_unprt_hst,0x00,sizeof(struct mdm_unprt_hst_c));
	memset( wherelist,0,sizeof(wherelist) );

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0310",g_pub_tx.ac_no);			/*�ʺ�*/
	get_zd_long("0440",&b_date);
	get_zd_long("0450",&e_date );
	g_pub_tx.ac_seqn=DD_BOOK_AC_SEQN;			/*�ʺ����*/

	pub_base_old_acno( g_pub_tx.ac_no );        /**�Ծ��ʺŵĴ���**/
	
		ret=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and ac_seqn=%d ",g_pub_tx.ac_no,g_mdm_ac_rel.ac_seqn);
		if( ret )
		{
			sprintf(acErrMsg,"ȡg_mdm_ac_rel�ļ�����![]");
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
		
		ret=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"ȡδ��ӡ��ϸ�ʴ���![]");
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
		g_mdm_ac_rel.prt_line=0;
		ret=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"ȡδ��ӡ��ϸ�ʴ���![]");
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
		Mdm_ac_rel_Clo_Upd();


	memset(&g_dd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
  ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						   g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�������Ա��в����ڸü�¼[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy (g_pub_tx.reply,"N055");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	sprintf( tmpstr," ac_id=%ld  and",g_mdm_ac_rel.ac_id );
	strcat( wherelist,tmpstr );
		
	if(b_date)
	{
		sprintf( tmpstr," tx_date>=%ld and",b_date );
		strcat( wherelist,tmpstr );
	}
	if(e_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",e_date );
		strcat( wherelist,tmpstr );
	}
	strcat(wherelist," 1=1 ORDER BY tx_date,trace_no");

	vtcp_log( "WHERE [%s]",wherelist );
	ret = Dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist);
	MYSQLERR
	while(1)
  {
	  memset(&g_mdm_unprt_hst,0x00,sizeof(struct mdm_unprt_hst_c));
	  ret = Dd_mst_hst_Fet_Sel(&g_dd_mst_hst,g_pub_tx.reply);
	  if(ret==100)
	  {
	    break;
	  }
	  MYERR
	  
	  g_mdm_unprt_hst.trace_no =  g_dd_mst_hst.trace_no;
	  g_mdm_unprt_hst.trace_cnt =  g_dd_mst_hst.trace_cnt;
	  strcpy(g_mdm_unprt_hst.tx_br_no , g_dd_mst_hst.tx_br_no);
	  g_mdm_unprt_hst.ac_id =  g_dd_mst_hst.ac_id;
	  g_mdm_unprt_hst.ac_seqn =  g_dd_mst_hst.ac_seqn;
	  strcpy(g_mdm_unprt_hst.add_ind , g_dd_mst_hst.add_ind);
	  g_mdm_unprt_hst.tx_amt =  g_dd_mst_hst.tx_amt;
	  g_mdm_unprt_hst.bal =  g_dd_mst_hst.bal;
	  strcpy(g_mdm_unprt_hst.tx_code , g_dd_mst_hst.tx_code);
	  g_mdm_unprt_hst.tx_date =  g_dd_mst_hst.tx_date;
	  g_mdm_unprt_hst.hst_cnt = 0;
	  strncpy(g_mdm_unprt_hst.brf , g_dd_mst_hst.brf,10);
	  
	  vtcp_log("trace_no=[%ld],trace_cnt=[%d],tx_br_no=[%s],ac_id=[%ld],ac_seqn=[%d],add_ind=[%s],tx_amt=[%f],bal=[%f],tx_code=[%s],tx_dste=[%ld],hst_cnt=[%d],brf=[%s]",\
	  g_mdm_unprt_hst.trace_no,g_mdm_unprt_hst.trace_cnt,g_mdm_unprt_hst.tx_br_no,g_mdm_unprt_hst.ac_id, \
	  g_mdm_unprt_hst.ac_seqn,g_mdm_unprt_hst.add_ind,g_mdm_unprt_hst.tx_amt,g_mdm_unprt_hst.bal, \
	  g_mdm_unprt_hst.tx_code,g_mdm_unprt_hst.tx_date,g_mdm_unprt_hst.hst_cnt,g_mdm_unprt_hst.brf );
	  
	  
	  ret = Mdm_unprt_hst_Ins( g_mdm_unprt_hst, g_pub_tx.reply );
	  if( ret )
      {
         sprintf(acErrMsg,"�Ǽ�mdm_unprt_hst�����![%d]",ret);
         WRITEMSG
         goto ErrExit;
      }
	 }
	 Dd_mst_hst_Clo_Sel();

	/*------add by xxx 20050801---for stat-begin------*/
	if( g_mdm_ac_rel.note_sts[0] == '*' )
	{
		sprintf( acErrMsg, "����������!" );
		WRITEMSG
		strcpy (g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0] == '1' )
	{
		sprintf( acErrMsg, "��ƾ֤�ѹ�ʧ!" );
		WRITEMSG
		strcpy (g_pub_tx.reply,"N081");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0] != '0' )
	{
		sprintf( acErrMsg, "��ƾ֤״̬������" );
		WRITEMSG
		strcpy (g_pub_tx.reply,"M013");
		goto ErrExit;
	}
	/*------add by xxx 20050801---for stat-end------*/


  	ret = Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
					   g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�������Ա��в����ڸü�¼[%s]",g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy (g_pub_tx.reply,"N055");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
		strcpy(g_mdm_attr.mdm_type,'1');
		strcpy(g_mdm_attr.ac_num_ind,'Y');
		strcpy(g_mdm_attr.no_ind,'N');
		strcpy(g_mdm_attr.prt_ind,'Y');
		g_mdm_attr.pg_num=10;
		g_mdm_attr.pg_line=10;

		ret = pub_base_patch_book(g_mdm_attr);
		if( ret<0 )
		{
			sprintf(acErrMsg,"���ò����ۺ���ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==0 )
		{
			sprintf(acErrMsg,"�û�û��δ����ϸ!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O190");
			goto ErrExit;
		}

		set_zd_data("0110","1");


    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log( ) )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}
	unprt_cnt=sql_count("mdm_unprt_hst","ac_id=%ld " ,g_pub_tx.ac_id);
	if (unprt_cnt>0)
		set_zd_data("0200","1");
    else
		set_zd_data("0200","0");
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����۳ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

