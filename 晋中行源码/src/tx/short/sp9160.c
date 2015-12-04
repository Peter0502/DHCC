/*************************************************
* �� �� ��: sp9160.c
* ��������������IC����Ӧ��Ʒ
* ��    ��: YW
* ������ڣ�2011��12��12��
* �޸ļ�¼��   
* ��    ��:
* �� �� ��:
* �޸�����:
*insert into tx_sub_def 	values('9160','����IC����Ӧ��Ʒ','sp9160','0','0');
*insert into tx_def values('9160','����IC����Ӧ��Ʒ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
*insert into tx_flow_def values('9160','0','9160','#$')
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "ic_subac_parm_c.h"
#include "mo_ic_ac_rel_c.h"

sp9160()  
{ 		
	/** ���ݳ�ʼ�� **/
	struct ic_subac_parm_c    s_ic_subac_parm;
	struct mo_ic_ac_rel_c     s_mo_ic_ac_rel;
	struct mdm_ac_rel_c       s_mdm_ac_rel;
	int ret=0;
	char wherelist[40];
	char filename[100];
	char cType[2];
	char cAc_no[20];
	FILE *fp;
	int i=0;

	memset (&s_ic_subac_parm, 0x00, sizeof(struct ic_subac_parm_c));
	memset (&s_mo_ic_ac_rel,  0x00, sizeof(struct mo_ic_ac_rel_c));
	memset (&s_mdm_ac_rel,    0x00, sizeof(struct mdm_ac_rel_c));
		
	memset(cType,0,sizeof(cType));
	memset(cAc_no,0,sizeof(cAc_no));
	/** ȡֵ����ֵ **/
	get_zd_data( "0300" , cAc_no);  /* ��    �� */
	get_zd_data( "0670" , cType );	/* ��ѯ���� 0:ȫ������ 1:���Կͻ�δͨ���˻����� 2�����Կͻ���ͨ���˻�����*/
	vtcp_log("cAc_no[%s],cType[%s]",cAc_no,cType );
	pub_base_AllDwnFilName( filename );
	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	ret=Ic_subac_parm_Dec_Sel( g_pub_tx.reply," sts='1' ");
	if( ret ) goto ErrExit;
	while(1)
	{
		memset (&s_ic_subac_parm, 0x00, sizeof(struct ic_subac_parm_c));
		ret = Ic_subac_parm_Fet_Sel(&s_ic_subac_parm,g_pub_tx.reply);
		if( ret==100 ) break;	
		else if( ret ) goto ErrExit;
		
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no='%s' ",cAc_no);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų���![%s]",cAc_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P183");
			goto ErrExit;
		}
		
		if(cType[0]=='1'){
			ret=sql_count("mo_ic_ac_rel","main_ac_no='%s' and sts='1' and sub_seqn=%d",s_mdm_ac_rel.ac_no,s_ic_subac_parm.sub_seqn);
			if(!ret){
				continue;
			}
		}if(cType[0]=='2'){
			ret=sql_count("mo_ic_ac_rel","main_ac_no='%s' and sts='1' and sub_seqn=%d",s_mdm_ac_rel.ac_no,s_ic_subac_parm.sub_seqn);
			if(!ret){
				continue;
			}
		}
		fprintf( fp,"0680|%-20s|%04d|\n", s_ic_subac_parm.sub_name, s_ic_subac_parm.sub_seqn );
		
		i++;
	}
	Ic_subac_parm_Clo_Sel();
	vtcp_log("i==[%d]",i );
	vtcp_log("0680|%-20s|%04d|\n", s_ic_subac_parm.sub_name, s_ic_subac_parm.sub_seqn);
	if(i)
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
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
