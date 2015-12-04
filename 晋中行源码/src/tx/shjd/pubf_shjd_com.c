#include "com_tel_c.h"
#include "shjd_tlrpoot_c.h"
#include "public.h"
int check_shjd_tel_poot(char tel[])
{
	struct com_tel_c wd_com_tel;
	memset(&wd_com_tel,'\0',sizeof(wd_com_tel));
	g_reply_int=Com_tel_Sel(g_pub_tx.reply,&wd_com_tel,"tel='%s'",tel);
	if(g_reply_int){
		vtcp_log("[%s][%d] �����Ա���[%d]",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"�����Ա���[%d]",g_reply_int);
		WRITEMSG
		return -1;;
	}
	switch(wd_com_tel.csts[0]){
		case '0':
			if(check_shjd_tlrpoot(tel)){
				vtcp_log("����Աcom_tel��״̬ǩ��,��tlrpoot��û�м�¼");
				sprintf(acErrMsg,"����Աcom_tel��״̬ǩ��,��tlrpoot��û�м�¼[%s]",tel);
				WRITEMSG
				return -1;
			}
			break;
		case '1':
			sprintf(acErrMsg,"����Ա״̬��ʱǩ��");
			break;
		case '2':
			sprintf(acErrMsg,"����Ա״̬ǩ��");
			break;
		case '3':
			sprintf(acErrMsg,"����Ա״̬����");
			break;
		case '4':
			sprintf(acErrMsg,"����Ա��ɾ��");
			break;
		default:
			vtcp_log("����Ĺ�Ա״̬[%s]",wd_com_tel.csts);
			sprintf(acErrMsg,"����Ĺ�Ա״̬[%s]",wd_com_tel.csts);
			return -1;
	}
	if(wd_com_tel.csts[0]!='0')
		return 1;
	vtcp_log("[%s][%d] csts[%s]\n",__FILE__,__LINE__,wd_com_tel.csts);
	return 0;
}
int check_shjd_tlrpoot(char tel[])
{
	char tx_day[9];
	struct shjd_tlrpoot_c wd_shjd_tlrpoot;
	memset(&wd_shjd_tlrpoot,'\0',sizeof(wd_shjd_tlrpoot));
	memset(tx_day,'\0',sizeof(tx_day));
	sprintf(tx_day,"%08d",g_pub_tx.tx_date);
	g_reply_int = Shjd_tlrpoot_Sel(g_pub_tx.reply, &wd_shjd_tlrpoot," tlrno='%s' and txday='%s' ",tel,tx_day);
        if(g_reply_int==100){
		return -1;
        }
	return 0;
}
