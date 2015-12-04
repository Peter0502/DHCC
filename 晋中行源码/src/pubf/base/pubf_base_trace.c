#include "find_debug.h" 
/*************************************************
* �� �� ��:     pub_base_trace
* ����������    1������ģ��pub_base_PubQlshȡ�õ�ǰ���õ�������ˮ��
                2������ģ��pub_base_PubQlsh_Transȡ�õ�ǰ���õ�������ˮ��
                3������ģ��pub_base_getwdlshȡ�õ�ǰ���õ�������ˮ��
* ��    ��:
* ������ڣ�    2003��12��16��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include "stdio.h"
#define EXTERN
#include "public.h"

/**********************************************************************
 * �� �� ����   pub_base_PubQlsh
 * �������ܣ�   ȡ�õ�ǰ���õ�������ˮ��
 * ����/ʱ�䣺
 *
 * ������
 *     ����: ��
 *     ���: ��
 *     ����: <=0ʧ��
             >0ȡ�õ�������ˮ��
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_base_init_trace_no()
{
 	long tlsrno;
	long max_trace_no;
	int ret=0;

		TRACE;

  	ret=sql_max_long ( "trace_log","trace_no",&max_trace_no,"1=1" );
	if( ret )
	{
		TRACE;
		goto ErrExit;
	}
		TRACE;

 	tlsrno = get_tlsrno();
 	if ( tlsrno<0 )
 	{
 		sprintf(acErrMsg,"ȡ������ˮ�Ŵ� ! zjlsh=[%d]",tlsrno);
 		WRITEMSG
		/*goto ErrExit; */
 	}
 		sprintf(acErrMsg,"������ˮ�� ![%d] tab[%d]",tlsrno,max_trace_no);
 		WRITEMSG
		TRACE;

	/*if( max_trace_no>=tlsrno )*/
	{
		set_tlsrno( max_trace_no+1 );
		printf( " SET TRACE NO to [%d]\n",max_trace_no+1 );
		sprintf( acErrMsg," SET TRACE NO to [%d]",max_trace_no+1 );
		WRITEMSG
	}

	TRACE;
	return 0;
ErrExit:
	strcpy ( g_pub_tx.reply,"52T1" );
	TRACE;
 	return -1;
}
int pub_base_PubQlsh(long *zjlsh,char *xym)
{
 long tlsrno;
 char tmp[21];

 MEMSET_DEBUG(tmp,0x00,sizeof(tmp));

 tlsrno = get_tlsrno();

 if ( tlsrno<0 )
 {
 sprintf(acErrMsg,"ȡ������ˮ�Ŵ� ! zjlsh=[%d]",tlsrno);
 WRITEMSG
 strcpy ( xym , "52T1" );
 return -1;
 }

        *zjlsh=tlsrno;

        return (0);
}

/**********************************************************************
 * �� �� ����   pub_base_PubQlsh_Trans
 * �������ܣ�   ����ҵ��ȡ�õ�ǰ���õ�������ˮ��
 * ����/ʱ�䣺
 *
 * ������
 *     ����: ��
 *     ���: zhlsh
 *     ����: <=0ʧ��
	     >0ȡ�õ�������ˮ��
 *
 * �޸���ʷ��
 *
********************************************************************/

int pub_base_PubQlsh_Trans( long *zjlsh)
{
        long tlsrno;

        tlsrno = get_tlsrno();

        *zjlsh=tlsrno;

        return (0);
}
