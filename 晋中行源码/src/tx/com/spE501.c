/*************************************************
* �� �� ��:  spE501.c
* ���������� ���㿪������
*
* ��    ��:  andy
* ������ڣ� 2004��3��9��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_branch_c.h"
#include "com_tel_c.h"
#include "com_sys_parm_c.h"

int spE501()
{
	int	ret;
	FILE	*fp;
	char	filename[51]; 
	char	mailname[51]; 
	char	tmpstr[100];
	
	struct	com_sys_parm_c	g_com_sys_parm;
	struct	com_branch_c	g_com_branch;
	struct	com_tel_c		g_com_tel;

	memset(&g_com_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));

	/* ��ʼ���ṹ */
	pub_base_sysinit();
	
	/* ��ѯϵͳ״̬ */
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&g_com_sys_parm,"1=1");
	if( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "��ѯ����ϵͳ����״̬����!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O072");
		goto ErrExit;
	}
	else if( ret==100 )
	{
		sprintf( acErrMsg, "����ϵͳ�޲���!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O247");
		goto ErrExit;
	}

	if( g_com_sys_parm.sys_sts!=0 )
	{
		sprintf( acErrMsg, "ϵͳ������״̬,�����Կ���!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O071");
		goto ErrExit;
	}

	/* �޸�����״̬ */
	ret = Com_branch_Dec_Upd(g_pub_tx.reply,"br_no='%s'",g_pub_tx.tx_br_no);
	if( ret )
	{
		sprintf( acErrMsg, "��ѯ��������������!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O070");
		goto ErrExit;
	}

	ret = Com_branch_Fet_Upd(&g_com_branch,g_pub_tx.reply);
	if( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "��ѯ��������������!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O070");
		goto ErrExit;
	}

	if( g_com_branch.wrk_sts[0]=='1' )
	{
		sprintf( acErrMsg, "������ǩ��!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O069");
		goto ErrExit;
	}
	
	if(g_com_branch.wrk_sts[0]=='*')
	{
		sprintf( acErrMsg, "���㴦�ڹر�״̬!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O068");
		goto ErrExit;
	}

	if(g_com_branch.wrk_sts[0]=='0')
	{
		sprintf( acErrMsg, "���㲻�ǿ�ǩ��״̬!!" );
		WRITEMSG
			/***********
		strcpy(g_pub_tx.reply,"O060");
		goto ErrExit;
			***********************/
	}

	/*��������״̬Ϊ��ǩ��*/
	strcpy(g_com_branch.wrk_sts,"1");

	ret = Com_branch_Upd_Upd(g_com_branch,g_pub_tx.reply);
	if( ret )
	{
		sprintf( acErrMsg, "�޸�����״̬����!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O067");
		goto ErrExit;
	}

	Com_branch_Clo_Upd( );

	/*�������й�Ա״̬Ϊ��ʽǩ��*	 ע���� ΪʲôҪ���ĳ���ʽǩ�ˣ�����������ʽǩ�˵�
	ret=Com_tel_Dec_Upd(g_pub_tx.reply,"br_no='%s' ",
			g_pub_tx.tx_br_no);
	if(ret)
	{
		sprintf( acErrMsg, "�����α����!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O070");
		goto ErrExit;
	}

	while(1)
	{
		ret=Com_tel_Fet_Upd(&g_com_tel,g_pub_tx.reply);
		if(ret&&ret!=100)
		{
			sprintf( acErrMsg, "��ѯ��Ա�����!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"O070");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}

		if( g_com_tel.lvl[0]=='6' ) continue;

		strcpy(g_com_tel.csts,"2");

		ret=Com_tel_Upd_Upd(g_com_tel,g_pub_tx.reply);
		if(ret)
		{
			sprintf( acErrMsg, "�޸����й�Ա״̬����!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"O067");
			goto ErrExit;
		}
	}
	Com_tel_Clo_Upd( );
	*/
	set_zd_long("0440",g_com_sys_parm.sys_date);
	
	/* ��ȡ���ձ�������ʾ�ļ� */

	sprintf(mailname, "%s/%ld/%s%ld" ,getenv("MAILDIR"), 
			g_pub_tx.tx_date , g_pub_tx.tx_br_no , g_pub_tx.tx_date ); 
	sprintf(acErrMsg,"�ļ�����[%s]",mailname);
	WRITEMSG
					      	
	fp = fopen( mailname,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",mailname );
		WRITEMSG
	}
	else
	{
		fclose(fp);
		pub_base_AllDwnFilName( filename );
		vtcp_log(filename);		
		
		sprintf(tmpstr,"cp %s %s 1>/dev/null 2>/dev/null",mailname, filename);
		system(tmpstr);
		vtcp_log(tmpstr);

		set_zd_data( DC_FILE_SND,"1" );	
	}


OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
