/*************************************************
* �� �� ��:  sp9845.c
* ���������� ��ί���տ�Э�����´�ί���嵥
*
* ��    ��:  jane
* ������ڣ� 2003��2��2�� 
*
* �޸ļ�¼:
* ��   �� :
* �� �� ��:
* �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_trust_list_c.h"

int sp9845()
{
	struct	mo_trust_list_c	sMoTrustList;	/* ί���嵥 */
    	char 	filename[100];
    	int 	ret;
	FILE 	*fp;
   
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", sMoTrustList.turst_pay_no );	/* ί���տ�Э���� */

	/* ���ί���տ�Э���Ƿ���� */
	ret = Mo_trust_list_Dec_Sel( g_pub_tx.reply, "turst_pay_no='%s'", sMoTrustList.turst_pay_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_trust_list_Dec_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
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

	while(1)
	{
		ret = Mo_trust_list_Fet_Sel( &sMoTrustList, g_pub_tx.reply );
		if( ret==100 )
		{
			sprintf( acErrMsg, "��ѯί���嵥���޼�¼!" );
			WRITEMSG
			break;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_trust_list_Fet_Sel��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%d|%s|%s|%s|%s|%.2f|\n", \
			sMoTrustList.turst_pay_no, sMoTrustList.payer_seqn, \
			sMoTrustList.payer_ac_no, sMoTrustList.payer_name, \
			sMoTrustList.payer_br_no, sMoTrustList.payer_br_name, \
			sMoTrustList.amt );
	}
	Mo_trust_list_Clo_Sel ();

	fclose(fp);

	set_zd_data(DC_FILE_SND,"1");
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
