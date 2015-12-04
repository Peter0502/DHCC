/***************************************************************************/
/* 文件名称: lv_HuaTeng_comm.c                                             */
/* 功能描述: 调用华腾卡的服务"scl34660",将卡帐号传入,获得相关信息          */
/* 创建日期: 2006.02.21                                                    */
/* 作者:     卿苏德                                                        */
/* 说明:                                                                   */
/***************************************************************************/
#include	<stdio.h>
#include "lv_define.h"
#include "errlog.h"
#include "atmi.h"
static struct To_HuaTeng to_huateng;
static struct From_HuaTeng from_huateng;

static struct Dhcc_Interface dhcc_interface;

int iHuaTeng_Interface_Deal(struct To_HuaTeng* pht_interface, struct Dhcc_Interface* pdhcc_interface)
{
	char *sendbuf,*recvbuf;
	long rcvlen;
	int iRc=0;
	struct HuaTeng_Interface* 				pInterface;	
	static struct HuaTeng_Interface ht_interface;
	struct From_HuaTeng*			pf_huateng;
  
  /*2006-3-4 23:19 卿苏德 修改为 传来的结构体 保证每个成员数组都以'\0'结尾 故采用strlen()来确定长度*/
	memset(&ht_interface, ' ', sizeof(ht_interface));
	memcpy(ht_interface.kinbr, pht_interface->c_kinbr, strlen(pht_interface->c_kinbr));
	memcpy(ht_interface.tlrno, pht_interface->c_tlrno, strlen(pht_interface->c_tlrno));
	memcpy(ht_interface.ejfno, pht_interface->c_ejfno, strlen(pht_interface->c_ejfno));
	memcpy(ht_interface.txno, pht_interface->c_txno, strlen(pht_interface->c_txno));
	memcpy(ht_interface.hcode, pht_interface->c_hcode, strlen(pht_interface->c_hcode));
	memcpy(ht_interface.cardno, pht_interface->c_cardno, strlen(pht_interface->c_cardno));
	memcpy(ht_interface.actseq, pht_interface->c_actseq, strlen(pht_interface->c_actseq));
	memcpy(ht_interface.pwd, pht_interface->c_pwd, strlen(pht_interface->c_pwd));
	memcpy(ht_interface.txamt, pht_interface->c_txamt, strlen(pht_interface->c_txamt));
	memcpy(ht_interface.dscpt, pht_interface->c_dscpt, strlen(pht_interface->c_dscpt));
	memcpy(ht_interface.rev, pht_interface->c_rev, strlen(pht_interface->c_rev));
	
	printf("[%s][%d]The KINBR is [%s]\n", __FILE__, __LINE__, pht_interface->c_kinbr);
	printf("[%s][%d]The TLRNO is [%s]\n", __FILE__, __LINE__, pht_interface->c_tlrno);	
	printf("[%s][%d]The EJFNO is [%s]\n", __FILE__, __LINE__, pht_interface->c_ejfno);	
	printf("[%s][%d]The TXNO  is [%s]\n", __FILE__, __LINE__, pht_interface->c_txno);	
	printf("[%s][%d]The HCODE is [%s]\n", __FILE__, __LINE__, pht_interface->c_hcode);	
	printf("[%s][%d]The CARDNO is [%s]\n", __FILE__, __LINE__, pht_interface->c_cardno);	
	printf("[%s][%d]The ACTSEQ is [%s]\n", __FILE__, __LINE__, pht_interface->c_actseq);
	/*printf("[%s][%d]The PWD  is [%s]\n", __FILE__, __LINE__, pht_interface->c_pwd);	*/
	printf("[%s][%d]The TXAMT is [%s]\n", __FILE__, __LINE__, pht_interface->c_txamt);	
	printf("[%s][%d]The DSCPT is [%s]\n", __FILE__, __LINE__, pht_interface->c_dscpt);
	printf("[%s][%d]The REV is [%s]\n", __FILE__, __LINE__, pht_interface->c_rev);	
	
	printf("=================[%s][%d]========================\n",__FILE__,__LINE__);

	sendbuf=(char *)tpalloc("CARRAY",NULL,HT_SENDBUF_LEN+1);
	if(sendbuf==NULL){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Can not alloc memory,errno=%d",__FILE__,__LINE__,tperrno);
		printf("[%s][%d] 分配内存错误 \n",__FILE__,__LINE__);
		return -1;
	}
	recvbuf=(char *)tpalloc("CARRAY",NULL,HT_RECVBUF_LEN+1);
	if(recvbuf==NULL){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Can not alloc memory,errno=%d",__FILE__,__LINE__,tperrno);
		printf("[%s][%d] 分配内存错误 \n",__FILE__,__LINE__);
		tpfree(sendbuf);
		return -1;
	}
	
	printf("=================[%s][%d]========================\n",__FILE__,__LINE__);
	pInterface = (struct HuaTeng_Interface*)sendbuf;
	/*给sendbuf赋值 可以用pInterface = pht_interface 为保险起见使用下面语句*/
	memset(pInterface, 0, HT_SENDBUF_LEN);
	memcpy(pInterface->kinbr, ht_interface.kinbr, 7);
	memcpy(pInterface->tlrno, ht_interface.tlrno, 6);
	memcpy(pInterface->ejfno, ht_interface.ejfno, 6);
	memcpy(pInterface->txno, ht_interface.txno, 4);
	memcpy(pInterface->hcode, ht_interface.hcode, 1);
	memcpy(pInterface->cardno, ht_interface.cardno, 19);
	memcpy(pInterface->actseq, ht_interface.actseq, 4);
	memcpy(pInterface->pwd, ht_interface.pwd, 8);
	memcpy(pInterface->txamt, ht_interface.txamt, 16);
	memcpy(pInterface->dscpt, ht_interface.dscpt, 16);
	memcpy(pInterface->rev, ht_interface.rev, 64);
	printf("=[%s]==\n",(char *)pInterface);
		
	/**
	errLog(LOG_ERR,0,RPT_TO_TTY,sendbuf,HT_SENDBUF_LEN+39+20,"%s,%d,Send transaction:",__FILE__,__LINE__);
	**/
	
	printf("=================[%s][%d]========================\n",__FILE__,__LINE__);
	
	iRc=tpcall("scl34660", sendbuf, HT_SENDBUF_LEN, &recvbuf, &rcvlen, (long)0);
	printf("After tpcall,return:%d\n",iRc);
	if(iRc<0){
		pf_huateng = (struct From_HuaTeng*)recvbuf;
		memcpy(pdhcc_interface->orspcode, pf_huateng->orspcode, 4);
		memcpy(pdhcc_interface->actno, pf_huateng->actno, 18);
		memcpy(pdhcc_interface->opnbr, pf_huateng->opnbr, 7);
		memcpy(pdhcc_interface->name, pf_huateng->name, 30);
		memcpy(pdhcc_interface->bal, pf_huateng->bal, 12);
		memcpy(pdhcc_interface->status, pf_huateng->status, 4);
		memcpy(pdhcc_interface->cdkind, pf_huateng->cdkind, 8);
		memcpy(pdhcc_interface->orev, pf_huateng->orev, 64);

		zip_space(dhcc_interface.orspcode);	
		zip_space(dhcc_interface.actno);	
		zip_space(dhcc_interface.opnbr);	
		zip_space(dhcc_interface.name);	
		zip_space(dhcc_interface.bal);	
		zip_space(dhcc_interface.status);	
		zip_space(dhcc_interface.cdkind);	
		zip_space(dhcc_interface.orev);
		errLog(LOG_ERR,0,RPT_TO_TTY,recvbuf,HT_RECVBUF_LEN+1,"%s,%d,tpcall error,errno=%d",__FILE__,__LINE__,tperrno);
		tpfree(sendbuf);
		tpfree(recvbuf);
		printf("[%s][%d]处理华腾卡业务失败\n",__FILE__,__LINE__);
		return -1;
	}else{	
		errLog(LOG_ERR,0,RPT_TO_TTY,recvbuf,rcvlen,"%s,%d,Receive response:",__FILE__,__LINE__);
	}	
	printf("=================[%s][%d]========================\n",__FILE__,__LINE__);
	
	pf_huateng = (struct From_HuaTeng*)recvbuf;
	
	printf("[%s][%d]orspcode[%.4s]\n", __FILE__, __LINE__, pf_huateng->orspcode);
	printf("[%s][%d]actno[%.18s]\n", __FILE__, __LINE__, pf_huateng->actno);
	printf("[%s][%d]opnbr[%.7s]\n", __FILE__, __LINE__, pf_huateng->opnbr);
	printf("[%s][%d]name[%.30s]\n", __FILE__, __LINE__, pf_huateng->name);
	printf("[%s][%d]bal[%.12s]\n", __FILE__, __LINE__, pf_huateng->bal);
	printf("[%s][%d]status[%.4s]\n", __FILE__, __LINE__, pf_huateng->status);
	printf("[%s][%d]cdkind[%.8s]\n", __FILE__, __LINE__, pf_huateng->cdkind);
	printf("[%s][%d]orev[%.64s]\n", __FILE__, __LINE__, pf_huateng->orev);
	
	memcpy(pdhcc_interface->orspcode, pf_huateng->orspcode, 4);
	memcpy(pdhcc_interface->actno, pf_huateng->actno, 18);
	memcpy(pdhcc_interface->opnbr, pf_huateng->opnbr, 7);
	memcpy(pdhcc_interface->name, pf_huateng->name, 30);
	memcpy(pdhcc_interface->bal, pf_huateng->bal, 12);
	memcpy(pdhcc_interface->status, pf_huateng->status, 4);
	memcpy(pdhcc_interface->cdkind, pf_huateng->cdkind, 8);
	memcpy(pdhcc_interface->orev, pf_huateng->orev, 64);

	zip_space(dhcc_interface.orspcode);	
	zip_space(dhcc_interface.actno);	
	zip_space(dhcc_interface.opnbr);	
	zip_space(dhcc_interface.name);	
	zip_space(dhcc_interface.bal);	
	zip_space(dhcc_interface.status);	
	zip_space(dhcc_interface.cdkind);	
	zip_space(dhcc_interface.orev);
	
	tpfree(sendbuf);
	tpfree(recvbuf);

	printf("=================[%s][%d]========================\n",__FILE__,__LINE__);
	return(0);	
}
