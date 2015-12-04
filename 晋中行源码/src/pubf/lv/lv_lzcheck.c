/******************************************************************
ģ������: ���˽��״���Ĺ�������
��������: ���˽��״���Ĺ�������
�������:
�������:
ʹ��˵��:
�� д ��: dxy 
*******************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lvpack.h"
#include "exitlvpack.h"
#include "lv_pub.h"



static	char	cAcbrno[13];	/** �����к� **/
static	char	cQsactno[13];	/** ���������к� **/
static	char	cSendco[5];		/** ���з������Ĵ��� **/
static	char	cCmtno[4];		/** CMT���� **/
static	char	cTxday[9];		/** ί������ **/
static	char	cCashno[13];	/** �����к� **/
static	char	cCashqsactno[13];	/** ���������к� **/


lv_lzcheck()
{
	int ret=0,cmtno=0;

    TLVPACKAGE lv;
    TLVPACKAGE lv2;
	lv_create(&lv);		/** ��ʼ���ṹ�� **/

	memset(cAcbrno,  '\0', sizeof(cAcbrno));
	memset(cQsactno, '\0', sizeof(cQsactno));
	memset(cSendco,  '\0', sizeof(cSendco));
	memset(cCmtno,   '\0', sizeof(cCmtno));
	memset(cTxday,   '\0', sizeof(cTxday));
	memset(cCashno,  '\0', sizeof(cCashno));
	memset(cCashqsactno,  '\0', sizeof(cCashqsactno));

	/** �ȴﵽ����������Ϣ **/
    if(find_netbrno(lvca.brno, cAcbrno)==NULL)
    {
        vtcp_log("[%s][%d]�õ������ߺŴ���!",__FILE__,__LINE__);
        _lv_errmsg("�õ������ߺŴ���!");
        return -1;
    }
	vtcp_log("[%s][%d]==========����12λ�к�cAcbrno=[%s]\n",__FILE__,__LINE__,cAcbrno);

    if(find_qsactno(cAcbrno,cQsactno,0)==NULL)
    {
        vtcp_log("[%s][%d]�õ����������д���!",__FILE__,__LINE__);
        _lv_errmsg("�õ����������д���!");
        return -1;
    }
	vtcp_log("[%s][%d]==========���������к�cQsactno=[%s]\n",__FILE__,__LINE__,cQsactno);

    if(find_sendco(cAcbrno,cSendco,0)==NULL)
    {
        vtcp_log("[%s][%d]�õ��������Ĵ���!",__FILE__,__LINE__);
        _lv_errmsg("�õ��������Ĵ���!");
        return -1;
    }
	vtcp_log("[%s][%d]==========���з������Ĵ���cSendco=[%s]\n",__FILE__,__LINE__,cSendco);


	/** ���ձ��� **/

	/** ��鱨�ĵĺϷ��� **/
	ret = lv_analyze_pack(&lv);	
    if(ret<0){
        
        _lv_errmsg("����С��ķǷ�!");
        vtcp_log("%s,%d,����С��ķǷ�!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
        return(-1);
    }

	if(lv.opeType != '1')
	{
        
        _lv_errmsg("���ʲ���С���!");
        vtcp_log("%s,%d,���ʲ���С���!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
        return(-1);
    }

	memcpy(cCmtno, lv.CMTCode, sizeof(cCmtno) -1);
	vtcp_log("[%s][%d]=============���ʱ�����=[%s]\n",__FILE__,__LINE__,cCmtno);
	
	/** ���������� **/
	if(lv.isMulti == '1')
	{
		lv_create(&lv2);		/** ��ʼ���ṹ�� **/

		ret = lv_fetchpkg(&lv,&lv2);		/** ������������ **/
    	if(ret<0){
        	
        	_lv_errmsg("�������Ĵ���!");
        	vtcp_log("%s,%d,�������Ĵ���!\n",__FILE__,__LINE__);
        	
			lv_destroy(&lv);
			lv_destroy(&lv2);
        	return(-1);
    	}
    	if(ret==1){
        	
        	_lv_errmsg("�����޼�¼!");
        	vtcp_log("%s,%d,�����޼�¼!\n",__FILE__,__LINE__);
        	
			lv_destroy(&lv);
			lv_destroy(&lv2);
        	return(-1);
		}

		cmtno = apatoi(lv.CMTCode,3);
		
		/** �õ������к� **/
		if(memcmp(lv.CMTNo,"006", 3) == 0 )
		{
			lv_get_tag(&lv2,"CC1",cCashno);	/** ��{3:}�еõ��������к� **/
		}
		else if(memcmp(lv.CMTNo,"003", 3) == 0 )
		{
			lv_get_tag(&lv2,"CCM",cCashno);	/** ��{3:}�еõ��������к� **/
			
		}
		else if(memcmp(lv.CMTNo,"004", 3) == 0 )
		{
			lv_get_tag(&lv2,"CCN",cCashno);	/** ��{3:}�еõ��������к� **/
		}
		else
		{
			lv_get_tag(&lv2,"58A",cCashno);	/** ��{3:}�еõ��������к� **/
		}	
		
		/** �õ����������к� **/	
		if(memcmp(lv.CMTCode,"013", 3) == 0 || memcmp(lv.CMTCode,"012", 3) == 0)
		{
			lv_pkg_getHeaderTag(&lv,"30E",cCashqsactno);	/** ��{P:}�еõ��������������к� **/
		}
		else
		{
			lv_pkg_getHeaderTag(&lv,"012",cCashqsactno);	/** ��{P:}�еõ��������������к� **/
		}

		/** �õ�ί������ **/
		lv_get_tag(&lv2,"30A",cTxday);			/** ��{3:}�еõ�ί������ **/
	}
	else	/** �õ����������Ľ����кż����� **/
	{
		cmtno = apatoi(lv.CMTNo,3);

		/** ί������ **/
		if(memcmp(lv.CMTNo,"301", 3) == 0 )
		{
			lv_get_tag(&lv2,"CJ9",cTxday);	
		}
		else if(memcmp(lv.CMTNo,"302", 3) == 0 )
		{
			lv_get_tag(&lv2,"CJ1",cTxday);	
		}
		else
		{
			lv_get_tag(&lv2,"30A",cTxday);	
		}

		/** �����к� **/ 
		if(memcmp(lv.CMTNo,"301", 3) == 0 )
		{
			lv_get_tag(&lv2,"01D",cCashno);	
		}
		else if(memcmp(lv.CMTNo,"302", 3) == 0 )
		{
			lv_get_tag(&lv2,"01C",cCashno);	
		}
		else if(memcmp(lv.CMTNo,"328", 3) == 0 )
		{
			lv_get_tag(&lv2,"CC1",cCashno);	
		}
		else if(memcmp(lv.CMTNo,"303", 3) == 0 && memcmp(lv.CMTNo,"319", 3) == 0 && 
				memcmp(lv.CMTNo,"320", 3) == 0 && memcmp(lv.CMTNo,"327", 3) == 0 )
		{
			lv_get_tag(&lv2,"58A",cCashno);	
		}

		/** �����к� **/
		if(memcmp(lv.CMTNo,"322", 3) == 0 || memcmp(lv.CMTNo,"324", 3) == 0 || memcmp(lv.CMTNo,"325", 3) == 0)
		{
			lv_get_tag(&lv2,"011",cCashqsactno);	
		}
		else if(memcmp(lv.CMTNo,"328", 3) == 0 )
		{
			lv_get_tag(&lv2,"CCO",cCashqsactno);	
		}
		else if(memcmp(lv.CMTNo,"920", 3) == 0 )
		{
			lv_get_tag(&lv2,"C37",cCashqsactno);	
		}
		else if(memcmp(lv.CMTNo,"254", 3) == 0 )
		{
			lv_get_tag(&lv2,"CC3",cCashqsactno);	
		}
		else if(memcmp(lv.CMTNo,"920", 3) == 0 )
		{
			lv_get_tag(&lv2,"C37",cCashqsactno);	
		}
		else 	/** 301,302,303,319,320,327,316 **/
		{
			lv_get_tag(&lv2,"012",cCashqsactno);	
		}
		
		if(memcmp(lv.CMTNo,"920", 3) == 0 || memcmp(lv.CMTNo,"316", 3) == 0	|| memcmp(lv.CMTNo,"254", 3) == 0
		   ||memcmp(lv.CMTNo,"324", 3) == 0 || memcmp(lv.CMTNo,"325", 3) == 0)
		{
			/** �õ������к� **/
			if(find_qsactno(cCashqsactno,cCashno)!=0)
			{
				vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FILE__,__LINE__,lvca.rtcode);
				_lv_errmsg("�������ݿ����!");
				
				
				return(-1);
			}
		}
	}


	/** ���ж��Ƿ��Ǹ��������ı��� **/
	if(memcmp(cCashqsactno,cQsactno,12)!=0)
	{
        
        _lv_errmsg("���������кŲ��Ǳ���!");
        vtcp_log("%s,%d,���������кŲ��Ǳ���!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
		lv_destroy(&lv2);
        return(-1);
	}

	if(memcmp(cCashno,cAcbrno,12)!=0)
	{
        
        _lv_errmsg("�����кŲ��Ǳ���!");
        vtcp_log("%s,%d,�����кŲ��Ǳ���!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
		lv_destroy(&lv2);
        return(-1);
	}

	/** ���ж�ί�������Ƿ�Ϸ� **/
	if(memcmp(cTxday,lvca.tbsdy,8)!=0)
	{
        
        _lv_errmsg("�������ڷǷ�!");
        vtcp_log("%s,%d,�������ڷǷ�!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
		lv_destroy(&lv2);
        return(-1);
	}

	vtcp_log("[%s][%d]============���ʱ��ı����=cmtno=[%d]\n",__FILE__,__LINE__,cmtno);

	switch(cmtno)
	{
		case 1:
				lv_deal_ptdjlz(&lv);
				break;
		case 2:
				lv_deal_ptjjlz(&lv);
				break;
		case 3:
				lv_deal_ssdjlz(&lv);
				break;
		case 4:
				lv_deal_ssjjlz(&lv);
				break;
		case 5:
				lv_deal_dqdjlz(&lv);
				break;
		case 6:
				lv_deal_dqjjlz(&lv);
				break;
		case 7:
				lv_deal_djthlz(&lv);
				break;
		case 8:
				lv_deal_ptjjhzlz(&lv);
				break;
		case 9:
				lv_deal_ssdjhzlz(&lv);
				break;
		case 10:
				lv_deal_ssjjhzlz(&lv);
				break;
		case 11:
				lv_deal_dqjjhzlz(&lv);
				break;
		case 12:
				lv_deal_tyxxlz(&lv);
				break;
		case 13:
				lv_deal_ssxxlz(&lv);
				break;
		case 301:						/** ��ѯ���� **/
				lv_deal_cmt301(&lv);
				break;
		case 302:						/** �鸴���� **/
				lv_deal_cmt302(&lv);
				break;
		case 303:						/** ���ɸ�ʽ���� **/
				lv_deal_cmt303(&lv);
				break;
		case 319:						/** �˻����뱨�� **/
				lv_deal_cmt319(&lv);
				break;
		case 320:						/** �˻�����Ӧ���� **/
				lv_deal_cmt320(&lv);
				break;
		case 322:						/** �˻س���Ӧ���� **/
				lv_deal_cmt322(&lv);
				break;
		case 324:						/** ʵʱҵ�����Ӧ���� **/
				lv_deal_cmt324(&lv);
				break;
		case 325:						/** ʵʱҵ�����֪ͨ���� **/
				lv_deal_cmt325(&lv);
				break;
		case 327:						/** ���ҵ��ֹ�����뱨�� **/
				lv_deal_cmt327(&lv);
				break;
		case 328:						/** ���ҵ��ֹ��Ӧ���� **/
				lv_deal_cmt328(&lv);
				break;
		case 254:						/** С�����������֪ͨ���� **/
				lv_deal_cmt254(&lv);
				break;
		default:
        		
        		_lv_errmsg("�������ڷǷ�!");
        		vtcp_log("%s,%d,�������ڷǷ�!\n",__FILE__,__LINE__);
        		
				lv_destroy(&lv);
				lv_destroy(&lv2);
        		return(-1);
				
	}


	lv_destroy(&lv);
	lv_destroy(&lv2);

}
