/*************************************************
* �� �� ��:  sp9982.c
* ���������� ֧�������Ѷ _ Inclusive for transaction 2206
*			 
*			1.�¹������������˻����Ӵ�����֧���������á�
*
*			2.�������������˻��� ��Ʊ������֮ǰ10������������֧�����룬
*  
*			  ����������֧Ʊ���ڣ���ֻ�ڿ����а���֧ȡҵ�� ����̨���ƣ�
*
*			3.ǰ̨��ʾ��Ա����֧Ʊ10���ڵ���Ч���ڣ�����ƾ֤һ�졣
*			  ( ����Ҫ֧��������˻�������ͨ�� )
*
* ��    ��:  jack
* ������ڣ� 2003��08��19��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "paymach_mana_inf_c.h"

int sp9982()
{
	int	ret;
	int tag=0;
	long tmp_date;
	char ac_no[19];
	char pwd_yn[2];
	struct	paymach_mana_inf_c	g_paymach_mana_inf;		/* ֧������ */
	struct	mdm_ac_rel_c		s_mdm_ac_rel;		

	memset(ac_no,0,sizeof(ac_no));
	memset(&g_paymach_mana_inf,0x00,sizeof(struct paymach_mana_inf_c));
	memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit( );

	get_zd_data( "1021", ac_no );				/* ��    �� */
	get_zd_data( "1023", g_pub_tx.beg_note_no );		/* ƾ֤���� */
	get_zd_long( "102E", &tmp_date );					/* ��Ʊ���� */
        vtcp_log("spCHNO��ʼ��[%ld][%s][%d]",tmp_date,__FILE__,__LINE__);

	pub_base_old_acno( ac_no );

	pub_base_strpack(ac_no);

	strcpy(g_pub_tx.ac_no,ac_no);

	/*** ����Ʊ���� ***/
	if( tmp_date && pub_base_checkdate( tmp_date ) )
	{
   		sprintf( acErrMsg, "���ڸ�ʽ���벻��ȷ������������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D243");
		goto ErrExit;
	}

	/*  ȡ����������  */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &s_mdm_ac_rel, "ac_no='%s'", 
							g_pub_tx.ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼![%s]",g_pub_tx.ac_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"�����ʺų���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** ί���տ���л�������֧������ ***/
	if( !strncmp(g_pub_tx.beg_note_no,"133",3) || \
		!strncmp(g_pub_tx.beg_note_no,"138",3) )
	{
		if( strcmp(g_pub_tx.tx_br_no,s_mdm_ac_rel.opn_br_no) )
		{
			sprintf(acErrMsg,"��Ʊ��ֻ���ڿ����а���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D248");
			goto ErrExit;
		}
		
	 	strcpy( pwd_yn,"N" );
	}
	else
	{
		/* �˻��Ƿ��������� */
		ret = Paymach_mana_inf_Sel( g_pub_tx.reply, &g_paymach_mana_inf, \
									"ac_no='%s' and flag='0'", g_pub_tx.ac_no );
		if( ret == 100 )
			strcpy( pwd_yn,"N" );
		else if( ret )
   		{
   			sprintf( acErrMsg, "���Ҵ�![%d]", ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		else 
		{
			/*** �ʻ����������������Ʊ���� ***/
			ret = pub_base_checkdate( tmp_date );
			if( ret )
			{
   				sprintf( acErrMsg, "���ڸ�ʽ���벻��ȷ������������!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"D243");
				goto ErrExit;
			}

			if( tmp_date > g_pub_tx.tx_date )
			{
   				sprintf( acErrMsg, "��Ʊ���ڲ��ô���ϵͳ���ڣ�����������!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"D244");
				goto ErrExit;
			}

			/*** ��Ʊ����С�ڴ������� ***/
			if( g_paymach_mana_inf.pr_date < tmp_date )
				strcpy( pwd_yn,"Y" );
			else if( g_paymach_mana_inf.pr_date >= tmp_date )
				strcpy( pwd_yn,"N" );
		}
	}

	pub_base_strpack( g_pub_tx.tx_br_no );
	pub_base_strpack( s_mdm_ac_rel.opn_br_no );

	if( pwd_yn[0]=='N' && strcmp(g_pub_tx.tx_br_no,s_mdm_ac_rel.opn_br_no) )
   	{
   		sprintf( acErrMsg, "�û���֧�����룬���ڿ����а���ҵ��![%s][%s][%s]",pwd_yn,g_pub_tx.tx_br_no,s_mdm_ac_rel.opn_br_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D240");
		goto ErrExit;
	}

	set_zd_data( "102C", pwd_yn );					/* ֧������ */

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
