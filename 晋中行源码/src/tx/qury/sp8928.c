/**************************************************************
* �� �� ��: sp8928.c
* ����������������һ��ͨ������ά��
* ���ݿ��ac_yht_rel_c
*
* ��    ��: jack
* ������ڣ�2004��3��28��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ac_yht_rel_c.h"
sp8928()
{
	int ret = 0;
	char cCzac_no    [20];   /***����  �ʺ�***/
	char cYhtac_no   [20];   /***һ��ͨ�ʺ�***/
	char cFlag[2];
	double lmt_amt;
	double zz_lmt;
	long tac_id=0;
	long tyht_ac_id=0;
	char tbwhere[128];
	struct ac_yht_rel_c ac_yht_rel;  /*����һ��ͨ������*/
	struct mdm_ac_rel_c vmdm_ac_rel;
	struct dd_mst_c vdd_mst;
	memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&vdd_mst,0x0,sizeof(struct dd_mst_c));
	memset(tbwhere,0x0,sizeof(tbwhere));
	memset(&ac_yht_rel,0x0,sizeof(ac_yht_rel));
  memset(cFlag,0x0,sizeof(cFlag));
	memset(cCzac_no,0x0,sizeof(cCzac_no));
	memset(cYhtac_no,0x0,sizeof(cYhtac_no));
	pub_base_sysinit();
  
	/*-------- ȡ�������  ---------*/

	get_zd_data("0310",cCzac_no);
	get_zd_data("0320",cYhtac_no);
	get_zd_data("0670",cFlag);
	get_zd_double("0400",&lmt_amt);
	get_zd_double("0410",&zz_lmt);
	pub_base_old_acno(cCzac_no);
	pub_base_old_acno(cYhtac_no);
	memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_no='%s'",cCzac_no);
	if(ret==100)
  {
        	sprintf(acErrMsg,"�����ʺŹ�ϵ�����ڸ��ʺ�!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P102" );
					goto ErrExit;
  }
  if(ret)
  {
        	sprintf(acErrMsg,"ȡ�����ʺŹ�ϵ�����!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"W011" );
					goto ErrExit;
  }
 	if(vmdm_ac_rel.note_sts[0]=='*')
  {
        	sprintf(acErrMsg,"����״̬������!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P172" );
					goto ErrExit;
  } 
  memset(&vdd_mst,0x0,sizeof(struct dd_mst_c));
 	ret=Dd_mst_Sel(g_pub_tx.reply,&vdd_mst,"ac_id=%ld",vmdm_ac_rel.ac_id);
  if(ret==100)
  {
  		       	sprintf(acErrMsg,"�������ļ������ڸ��ʻ�ID!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M108" );
							goto ErrExit;
  }
  if(ret)
  {
  						sprintf(acErrMsg,"�����������!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"W015" );
							goto ErrExit;
  }
  if(vdd_mst.ac_sts[0]=='*')
  {
  		      	sprintf(acErrMsg,"�û����ʺ�״̬Ϊ����!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M003" );
							goto ErrExit;       
	}
	tac_id=vdd_mst.ac_id;
  memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_no='%s'",cYhtac_no);
	if(ret==100)
  {
        	sprintf(acErrMsg,"�����ʺŹ�ϵ�����ڸ��ʺ�!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P102" );
					goto ErrExit;
  }
  if(ret)
  {
        	sprintf(acErrMsg,"ȡ�����ʺŹ�ϵ�����!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"W011" );
					goto ErrExit;
  }
 	if(vmdm_ac_rel.note_sts[0]=='*')
  {
        	sprintf(acErrMsg,"����״̬������!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P172" );
					goto ErrExit;
  } 
  memset(&vdd_mst,0x0,sizeof(struct dd_mst_c));
 	ret=Dd_mst_Sel(g_pub_tx.reply,&vdd_mst,"ac_id=%ld",vmdm_ac_rel.ac_id);
  if(ret==100)
  {
  		       	sprintf(acErrMsg,"�������ļ������ڸ��ʻ�ID!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M108" );
							goto ErrExit;
  }
  if(ret)
  {
  						sprintf(acErrMsg,"�����������!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"W015" );
							goto ErrExit;
  }
  if(vdd_mst.ac_sts[0]=='*')
  {
  		      	sprintf(acErrMsg,"�û����ʺ�״̬Ϊ����!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M003" );
							goto ErrExit;       
	}
	tyht_ac_id=vdd_mst.ac_id;
	if(cFlag[0]=='1')/**���ӹ���**/
	{
				ac_yht_rel.ac_id=tac_id;
				ac_yht_rel.yht_ac_id=tyht_ac_id;
				ac_yht_rel.lmt_amt=lmt_amt;
				ac_yht_rel.zz_lmt=zz_lmt;
				strcpy( ac_yht_rel.brf,"���ӹ���");
				ret=Ac_yht_rel_Ins(ac_yht_rel,g_pub_tx.reply);
				if( ret )
    		{
    		            sprintf(acErrMsg,"Insert!! [%d]",ret);
    		            WRITEMSG
    		            goto ErrExit;
    		}
	}	
	if(cFlag[0]=='2')/**�޸Ĺ���**/
	{
		    ret=Ac_yht_rel_Dec_Upd( g_pub_tx.reply,"ac_id=%ld and yht_ac_id=%ld",tac_id,tyht_ac_id);
		    if (ret != 0)
		    {
		    	sprintf(acErrMsg,"�޸���Ϣ��1[%ld]",tyht_ac_id);
		    	WRITEMSG
		    	strcpy(g_pub_tx.reply,"H008");
		    	goto ErrExit;
		    }
		    ret = Ac_yht_rel_Fet_Upd(&ac_yht_rel, g_pub_tx.reply );
		    if (ret != 0)
		    {
		    	sprintf(acErrMsg,"�޸���Ϣ��2[%ld]",tyht_ac_id);
		    	WRITEMSG
		    	strcpy(g_pub_tx.reply,"H008");
		    	goto ErrExit;
		    }
		    ac_yht_rel.zz_lmt=zz_lmt;
		    ac_yht_rel.lmt_amt=lmt_amt;
		    ret = Ac_yht_rel_Upd_Upd(ac_yht_rel, g_pub_tx.reply );
		    if (ret != 0)
		    {
		    	sprintf(acErrMsg,"�޸���Ϣ��[%ld]",tyht_ac_id);
		    	WRITEMSG
		    	strcpy(g_pub_tx.reply,"H008");
		    	goto ErrExit;
		    }
		    ret = Ac_yht_rel_Clo_Upd();	
	}
if(cFlag[0]=='3')/**ɾ������**/
{
			sprintf(tbwhere,"ac_id=%ld and yht_ac_id=%ld",tac_id,tyht_ac_id);
			ret = deldb("ac_yht_rel",tbwhere);
			if (ret != 0)
			{
				sprintf(acErrMsg,"ɾ����¼��[%s]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"H008");
				goto ErrExit;
			}
}

/***�˴�����ˮ-20070116***/
if( pub_ins_trace_log() )
{
				sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
				WRITEMSG
				goto ErrExit;
}
GoodExit:
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
