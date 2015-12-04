/*************************************************
* �� �� ��:  sp9701.c
* ���������� �ɿ��Ż��Ի���___2102�н���ڿ���
*
* ��    ��:  rob 
* ������ڣ� 2003��2��21��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

int sp9701()
{
	int	ret,i=0,vcnt;
	char card_no[20],filename[300],tmp_name[51],cif_type[3],wherelist[300];
	FILE *fp;

	struct 	prdt_ac_id_c   	g_prdt_ac_id;			
	struct 	mdm_attr_c   	g_mdm_attr;		

	/*���*/
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	/*ǰ̨ȡֵ*/
	get_zd_data("1039",card_no);       				/*����*/

   	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",card_no);
   	if( ret==100 )
   	{
        sprintf( acErrMsg,"�޴˿��ţ�����!card_no=[%s]", card_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "M003" );
        goto ErrExit;
   	}
    else if( ret )
   	{
        sprintf( acErrMsg, "��ȡ�����˻����ձ��쳣![%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "W011" );
        goto ErrExit;
    }

	if( !strncmp(g_mdm_ac_rel.mdm_code,"0016",4) )
   	{
        sprintf( acErrMsg, "����һ��ͨ���ܰ����ҵ��!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "O211" );
        goto ErrExit;
    }

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
					 	g_mdm_ac_rel.mdm_code);
	if(!ret&&g_mdm_attr.ac_num_ind[0]=='Y')
   	{
        sprintf( acErrMsg, "�������ʻ����ܰ����ҵ��!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "O212" );
        goto ErrExit;
    }

	/*�����˻�ID�ҳ��˻�����*/
   	ret=Prdt_ac_id_Sel( g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
						g_mdm_ac_rel.ac_id);
   	if( ret==100 )
   	{
        sprintf( acErrMsg,"�޴˿��ţ�����!card_no=[%ld]", card_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "M003" );
        goto ErrExit;
   	}
    else if( ret )
   	{
        sprintf( acErrMsg, "��ȡ��Ʒ�˻����ձ��쳣![%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "W013" );
        goto ErrExit;
    }

	switch(g_prdt_ac_id.ac_id_type[0])
	{
		case '1':							/*1-����*/
   				ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld", \
								g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"���˻�ID������!card_no=[%ld]", 
							card_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "��ȡ�������ļ��쳣![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W015" );
        			goto ErrExit;
    			}
				strcpy(tmp_name,g_dd_mst.name);

   				ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
							"prdt_no='%s'",g_prdt_ac_id.prdt_no);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"�ò�Ʒ������!prdt_no=[%s]", 
							g_prdt_ac_id.prdt_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
				strcpy(cif_type,g_dd_parm.cif_type);
				break;
		case '2':							/*2-����*/
   				ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
							"ac_id=%ld",g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"���˻�ID������!card_no=[%ld]", 
							card_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "��ȡ�������ļ��쳣![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W017" );
        			goto ErrExit;
    			}
				strcpy(tmp_name,g_td_mst.name);

   				ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
							"prdt_no='%s'",g_prdt_ac_id.prdt_no);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"�ò�Ʒ������!prdt_no=[%s]", 
							g_prdt_ac_id.prdt_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
				strcpy(cif_type,g_td_parm.cif_type);
				break;
		default : 	
        		sprintf( acErrMsg, "���ʺ��쳣!![%d]", ret );
        		WRITEMSG
        		strcpy( g_pub_tx.reply, "O024" );
        		goto ErrExit;
				break;
	}

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

	sprintf( wherelist," cif_type in('0','1') and prdt_no in (select prdt_no from mdm_prdt_rel where mdm_code1='%s' or mdm_code2='%s' or mdm_code3='%s') ", g_mdm_ac_rel.mdm_code,g_mdm_ac_rel.mdm_code,g_mdm_ac_rel.mdm_code );
	
	/*** ���ɲ�Ʒ�б� ***/
	ret=Dd_parm_Dec_Sel( g_pub_tx.reply," %s order by prdt_no",wherelist );
	if( ret ) goto ErrExit;

	i=0;
	while(1)
	{
		ret = Dd_parm_Fet_Sel( &g_dd_parm , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( i==0 )
		{
			set_zd_data("1031",g_dd_parm.prdt_no);
			set_zd_data("1032",g_dd_parm.title);
		}

		pub_base_strpack(g_dd_parm.title);
		fprintf( fp,"1031|%s|%s|\n",g_dd_parm.title,g_dd_parm.prdt_no );

		i++;
		vcnt++;
	}
	Mdm_prdt_rel_Clo_Sel();
	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);

	if(cif_type[0]!='1')
  	{
   		sprintf( acErrMsg,"�����봢���˺�!cif_type=[%s]",cif_type );
      	WRITEMSG
       	strcpy( g_pub_tx.reply, "O164" );
       	goto ErrExit;
   	}

    /* ��ǰ̨��Ļ��ֵ */
	set_zd_data("1038",tmp_name);						/*����*/

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
