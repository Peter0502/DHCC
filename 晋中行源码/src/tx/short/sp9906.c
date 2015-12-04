/*************************************************
* �� �� ��:  sp9906.c
* ���������� С��Ļ�ɷѺ����ѯ
*
* ��    ��:  jake
* ������ڣ� 2003��4��30�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_peritemmap_c.h"

int sp9906()
{
	struct ag_peritemmap_c sPerItemMap;	/*���˽ɷѺ�����Ʊ�*/
	int ret = 0;
	char cItem_no[13];
	char cUnit_no[5];
  long lMaxitem_no=0;
	memset(cItem_no,0,sizeof(cItem_no));
	memset(cUnit_no,0,sizeof(cUnit_no));
	pub_base_sysinit();
	get_zd_data("0330",cItem_no); 
	get_zd_data("0920",cUnit_no);

	sprintf( acErrMsg, "-----------------���Ϊ:[%s][%s]",cItem_no, cUnit_no );
	WRITEMSG

	memset( &sPerItemMap, 0x00, sizeof(sPerItemMap) );
	ret=Ag_peritemmap_Sel( g_pub_tx.reply , &sPerItemMap ,\
			"item_no = '%s' and unit_no = '%s' and stat ='0'", \
			cItem_no,cUnit_no );
	if( ret != 0 && ret != 100 )
	{
		sprintf( acErrMsg, "ִ��Ag_peritemmap����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
  
		sprintf( acErrMsg, "-----------------���Ϊ:[%s][%s][%s]",sPerItemMap.name ,sPerItemMap.ac_no ,sPerItemMap.id_no );
		WRITEMSG

	set_zd_data( "0260",sPerItemMap.name );
	set_zd_data( "0370",sPerItemMap.ac_no );
	set_zd_data( "0620",sPerItemMap.id_no );
	set_zd_data( "0200",sPerItemMap.id_type);
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
