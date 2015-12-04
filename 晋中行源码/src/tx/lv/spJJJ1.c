/*************************************************
* �� �� ��:  spJJJ1.c
* ���������� С���յ�920�ܾ�Ӧ��Ĵ���1,���ԭ������,��������ʺ�
*			 
* ��    ��:  LiuHuafeng
* ������ڣ� 2006-10-14 16:41
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"
/**add by YH_BP**/
#include "lv_addit_c.h"
#include "com_parm_c.h"
#include "cashier_c.h"
/**end by YH_BP**/
int spJJJ1()
{
	long iStep=0;
	char cAc_no [20];
	char cAcno110[20];
	char cKinbr[BRNO_LEN+1];
	int iRet=0;
	char cFinishFlag[2];
	char cCmtno[4];
	char cRejectWb[51];/* packid ,packdate,respcode ˳��*/
	struct lv_wbctl_c  sLv_wbctl;
	struct lv_pkgreg_c sLv_pkgreg;
	int iTimes=0;
	char cTmpstr[9];
	
	memset(cRejectWb,0 , sizeof(cRejectWb));

	memset(&sLv_wbctl , 0  ,sizeof(sLv_wbctl ));
	memset(&sLv_pkgreg, 0  ,sizeof(sLv_pkgreg));

	memset(cCmtno, 0 , sizeof(cCmtno));
	memset(cFinishFlag, 0 , sizeof(cFinishFlag));
	memset(cKinbr, 0 , sizeof(cKinbr));
	memset(cAcno110, 0 , sizeof(cAcno110));
	memset(cAc_no , 0 , sizeof(cAc_no ));
	get_zd_data("0300",cAc_no );
	get_zd_data("0500",cFinishFlag);
	get_zd_data(DC_TX_BR_NO,cKinbr);
	get_zd_data("0510",cCmtno);
	get_zd_data("0270",cRejectWb);
	
	get_zd_long("0530",&iStep);
	vtcp_log("%s,%d �õ� iStep---=[%d] ===",__FL__,iStep);	
		
	if(iStep < 0)
	{
		vtcp_log("%s,%d �����ˣ���Ӧ�õ�������ѽ!!! ",__FL__);
		strncpy(g_pub_tx.reply,"D102",4);
		goto ErrExit;
	}

	char cRespCode[9];
	memset(cRespCode,0,sizeof(cRespCode));
	/*get_zd_data("0290",cRespCode);	*/
	
	vtcp_log("%s,%d �õ� cAc_no---=[%s] ===",__FL__,cAc_no);	
	vtcp_log("%s,%d �õ� cFinishFlag---=[%s] ===",__FL__,cAc_no);	
	vtcp_log("%s,%d �õ� cKinbr---=[%s] ===",__FL__,cKinbr);
	vtcp_log("%s,%d �õ� cCmtno---=[%s] ===",__FL__,cCmtno);
	vtcp_log("%s,%d �õ� cRejectWb---=[%s] ===",__FL__,cRejectWb);					
	vtcp_log("%s,%d �õ� iStep---=[%d] ===",__FL__,iStep);	

	vtcp_log("%s,%d �õ� cRespCode---=[%s] ===",__FL__,cRespCode);

	memcpy(sLv_pkgreg.packid,cRejectWb,8);
	vtcp_log("%s,%d �õ� sLv_pkgreg.packid---=[%s] ===",__FL__,sLv_pkgreg.packid);		
	memset(cTmpstr,0 , sizeof(cTmpstr));			
	memcpy(cTmpstr,cRejectWb+8,8);	
	vtcp_log("%s,%d �õ� cTmpstr---=[%s] ===",__FL__,cTmpstr);	
	sLv_pkgreg.pack_date = apatoi(cTmpstr,8);
	vtcp_log("%s,%d �õ� sLv_pkgreg.pack_date---=[%d] ===",__FL__,sLv_pkgreg.pack_date);	

	memcpy(cRespCode,cRejectWb+16,8);	
	vtcp_log("%s,%d �õ� cRespCode---=[%s] ===",__FL__,cRespCode);	

	if(strlen(cRespCode) ==0 || cRespCode[3] =='I' )
	{
			vtcp_log("%s,%d Ӧ���벻�Ǿܾ�����Ӧ�õ�������ѽ!!! ",__FL__);
			strncpy(g_pub_tx.reply,"D102",4);
			goto ErrExit;
	}
	
	iTimes=0;

 	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply,"pack_date=%d and packid='%s' order by rowid",sLv_pkgreg.pack_date,sLv_pkgreg.packid);
 	if(g_reply_int)
 	{
 		vtcp_log("%s,%d �����α����",__FL__);
 		strncpy(g_pub_tx.reply,"D102",4);
 		goto ErrExit;
 	}
			
	while(1)
	{

    g_reply_int = Lv_pkgreg_Fet_Sel( &sLv_pkgreg , g_pub_tx.reply );		
		if(g_reply_int==DB_NOTFOUND)
		{
			
			if(iTimes == 0)
			{
				
				vtcp_log("[%s,%d] һ����¼Ҳû�ҵ�!!  ",__FL__);
				vtcp_log("[%s,%d] һ����¼Ҳû�ҵ�!!  ",__FL__);
				strcpy(acErrMsg,"û���ҵ�ԭʵʱ�������ʼ�¼");
				WRITEMSG
				/* rem by LiuHuafeng 2009-5-15 10:35:36 �����д�������ҵ�������п���һ�����Ҳ�����
				 * �Ҳ����Ͱ�������ϴ���ֱ������J099 
				 *return(-1);
				 **begin add by LiuHuafeng for ��������****/
				set_zd_long("0530",-1);
				set_zd_long("0500",1);/* ����finish flag =1 */
				strncpy(g_pub_tx.reply,"0010",4); /* ����Ҫ�������� ��ת��J099 */
				vtcp_log("[%s,%d] ===== 920 ���ʴ������ !!! ",__FL__);
				goto ErrExit;	
			}
			 
			 /* ����0530 Ϊ -1 ������һ�����״��������,����Ҫ�������� */
			 set_zd_long("0530",-1);
			 strncpy(g_pub_tx.reply,"0010",4); /* ����Ҫ�������� ��ת��J099 */
		   vtcp_log("[%s,%d] ===== 920 ���ʴ������ !!! ",__FL__);
			 goto ErrExit;
		}
		if(g_reply_int)
		{
			vtcp_log("%s,%d ��ȡ�α����",__FL__);
			strncpy(g_pub_tx.reply,"D102",4);
			goto ErrExit;
		}
		
		if(iTimes==iStep)
		{
			/* ��Ҫ����ļ�¼ */
			/* ��ƽ��׻��� */
			set_zd_data(DC_TX_BR_NO,sLv_pkgreg.br_no);
			memset(cAc_no, 0 ,sizeof(cAc_no));
			/*begin NEWYX add by LiuHuafeng 2007-4-2 17:17 
				����ǽ�ǻ�ִҵ��Ӧ���벻��00������Ҫ��飬ͬʱ������ؽ��Ϊ0 */
			if(
				  (!memcmp(sLv_pkgreg.pkgno,PKGNO_PTJJHZ,3) ||
				   !memcmp(sLv_pkgreg.pkgno,PKGNO_DQJJHZ,3) ||
				   !memcmp(sLv_pkgreg.pkgno,PKGNO_SSJJHZ,3) ||
				   !memcmp(sLv_pkgreg.pkgno,PKGNO_SSDJHZ,3))
				&&
				  memcmp(sLv_pkgreg.rcpstat,"00",sizeof(sLv_pkgreg.rcpstat)-1)
			)
			{
				set_zd_double("0400",0.0);
				strncpy(g_pub_tx.reply,"0010",4); /* ����Ҫ�������� ��ת��J099 */
				set_zd_long("0530",-1);
				vtcp_log("[%s,%d] ===== 920 ���ʴ������ !!! ",__FL__);
				goto ErrExit;
			}
			else if(!memcmp(sLv_pkgreg.pkgno,PKGNO_PTJJ,3) ||
				!memcmp(sLv_pkgreg.pkgno,PKGNO_DQJJ,3) ||
				!memcmp(sLv_pkgreg.pkgno,PKGNO_SSJJ,3)
			)
			{
				/* ���ҵ��Ҳ����Ҫ�ж��ʻ�״̬�ȣ�ͬʱ�������Ϊ0 */
				set_zd_double("0400",0.0);
				strncpy(g_pub_tx.reply,"0010",4); /* ����Ҫ�������� ��ת��J099 */
				set_zd_long("0530",-1);
				vtcp_log("[%s,%d] ===== 920 ���ʴ������ !!! ",__FL__);
				goto ErrExit;
			}
			/* begin add by LiuHuafeng 2009-5-13 17:56:36 for �������� */
			else if(strncmp(sLv_pkgreg.br_no,"9999",BRNO_LEN)==0)
			{
				set_zd_double("0400",0.0);
				strncpy(g_pub_tx.reply,"0010",4); /* ����Ҫ�������� ��ת��J099 */
				set_zd_long("0530",-1);
				vtcp_log("[%s,%d] ===== 920 ���ʴ������ ��������ҵ�񲻽��������� !!! ",__FL__);
				goto ErrExit;
			}
			/* end by LiuHuafeng 2009-5-13 17:56:49 */
      else
			{
			/* END NEWYX by LiuHuafeng 2007-4-2 17:30 */
				if(!memcmp(sLv_pkgreg.pkgno,"007",3))
				{
					if(sLv_pkgreg.lv_o_sts[0]==STAT_LZRECV2)
					{
						/* ���ݸ������ʺŽ��д��� */
						strncpy(cAc_no,sLv_pkgreg.pay_ac_no,sizeof(cAc_no)-1);
					}
					else
					{
						/* ���ݲ������еõ��Ĺ����ʻ����µ��ʺ� */
						iRet=pub_base_GetParm_Str(sLv_pkgreg.br_no,PARM_LVGZ,cAc_no);
						if(iRet)
						{
							vtcp_log("%s,%d û�����û���%s��С������ʺ�",__FL__,cKinbr);
							strncpy(g_pub_tx.reply,"P076",4);
							goto ErrExit;
						}
					}
				}
				else
				{
					  /**add by YH_BP **/
					  vtcp_log("[%s][%d]sLv_pkgreg.pkgno=[%s]\n",__FILE__,__LINE__,sLv_pkgreg.pkgno);
					  vtcp_log("[%s][%d]sLv_pkgreg.txnum=[%s]\n",__FILE__,__LINE__,sLv_pkgreg.txnum);
					  vtcp_log("[%s][%d]sLv_pkgreg.lw_ind=[%c]\n",__FILE__,__LINE__,sLv_pkgreg.lw_ind[0]);
					  if(memcmp(sLv_pkgreg.pkgno,"010",3)==0 && memcmp(sLv_pkgreg.txnum,"30103",5)==0 && sLv_pkgreg.lw_ind[0]=='1')
					  {
					  	  int iRc =0;
					  	  char   cBr_no[BRNO_LEN+1];
					  	  char   cPayacno[21];
					  	  struct lv_addit_c sLv_addit;
					  	  struct cashier_c  sCashier ;
					  	  struct com_parm_c sCom_parm;
					  	  TYJL1_ADD_AREA lv_tyjl1;
					  	  
					  	  /**add by NEWBP1 ֻ�����ʷ��͵ľܾ������ʰ�**/
					  	  if(sLv_pkgreg.lv_sts[0] !=STAT_WZSEND )
					  	  {
			          	   /* ���ҵ��Ҳ����Ҫ�ж��ʻ�״̬�ȣ�ͬʱ�������Ϊ0 */
			          	   set_zd_double("0400",0.0);
			          	   strncpy(g_pub_tx.reply,"0010",4); /* ����Ҫ�������� ��ת��J099 */
			          	   set_zd_long("0530",-1);
			          	   vtcp_log("[%s,%d] ===== 920 ���ʴ������ !!! ",__FL__);
			          	   goto ErrExit;
			          }
					  	  /***end by NEWBP1***/
					  	  memset(cPayacno      , 0 , sizeof(cPayacno));
					  	  memset(cBr_no        , 0 , sizeof(cBr_no));
					  	  memset(&lv_tyjl1     , 0 , sizeof(lv_tyjl1));
					  	  memset(&sLv_addit    , 0 , sizeof(sLv_addit));
					  	  memset(&sCashier     , 0 , sizeof(sCashier));
					  	  memset(&sCom_parm    , 0 , sizeof(sCom_parm));
					  	  
					  	  /**���Ҹý��׵ĸ������**/
					  	  vtcp_log("[%s][%d]sLv_pkgreg.in_date=[%d]\n",__FILE__,__LINE__,sLv_pkgreg.in_date);
					  	  vtcp_log("[%s][%d]sLv_pkgreg.addid  =[%s]\n",__FILE__,__LINE__,sLv_pkgreg.addid);
					  	  
					  	  iRc = Lv_addit_Sel(g_pub_tx.reply,&sLv_addit,"tagname='001' and tx_date='%ld' and addid='%s'",\
					  	                                    sLv_pkgreg.in_date,sLv_pkgreg.addid);
					  	  if(iRc)
                {
                    vtcp_log("[%s][%d],��ѯLv_addit��ʧ��[%d]",__FILE__,__LINE__, iRc);
                    sprintf(acErrMsg,"��ѯLv_addit��ʧ��[%s][%d]",__FILE__,__LINE__);
                    WRITEMSG
                    return(iRc);
                }
                /**�Ӹ�������õ�ǩ�����ںͱ�Ʊ����**/
                vtcp_log("[%s][%d]sLv_addit.tagdata=[%s]\n",__FILE__,__LINE__,sLv_addit.tagdata);
                memcpy(&lv_tyjl1,sLv_addit.tagdata,sizeof(lv_tyjl1));
                vtcp_log("[%s][%d]lv_tyjl1.cpday=[%.8s]\n",__FILE__,__LINE__,lv_tyjl1.cpday);
                vtcp_log("[%s][%d]lv_tyjl1.cpno =[%.20s]\n",__FILE__,__LINE__,lv_tyjl1.cpno);
					  	  /**��Ʊ��Ϣ���в鵽brno**/
					  	  iRc = Cashier_Dec_Upd(g_pub_tx.reply,"pono='%.20s' and signday='%.8s' ",\
                                    lv_tyjl1.cpno,lv_tyjl1.cpday);
                if (iRc)
                {
                    vtcp_log("%s,%d,���ұ�Ʊ������Ϣ�����[%d]\n",__FILE__,__LINE__,iRc);
                    sprintf( acErrMsg,"�����ִ�Ǽǲ�ʧ��");
                    WRITEMSG
                    return(iRc);
                }
                vtcp_log("[%s][%d]׼��FETCH ������[%d]\n",__FILE__,__LINE__,iRc);
                iRc = Cashier_Fet_Upd(&sCashier,g_pub_tx.reply);
                if (iRc!=0 && iRc!=100)
                {
                    vtcp_log("%s,%d,FET��Ʊ������Ϣ�����[%d]\n",__FILE__,__LINE__,iRc);
                    sprintf( acErrMsg,"FET��Ʊ������Ϣ�����");
                    WRITEMSG
                    return(iRc);
                }
                else if(iRc == 100)
                {
                   vtcp_log("%s,%d,û�������Ʊ��¼[%d]\n",__FILE__,__LINE__,iRc);
                   sprintf( acErrMsg,"û�������Ʊ��¼");
                   WRITEMSG
                   return(iRc);
                }
                printf("[%s][%d]Ʊ��״̬Ϊ[%s]\n",__FILE__,__LINE__,sCashier.stat);
                if(sCashier.stat[0]=='2' || sCashier.stat[0]=='3')
                {
                    sCashier.lactcls[0] = sCashier.stat[0];   /* ��Ʊ��ǰ״̬��־ 0-�Ǽ� 1-ǩ�� */
                    sCashier.stat[0] = '1';                   /* ��Ʊ��ǰ״̬��־ 0-�Ǽ� 1-ǩ�� */
                }
                memcpy(cBr_no,sCashier.brno,BRNO_LEN);
                vtcp_log("[%s][%d]��Ʊҵ�����cBr_no=[%s]\n",__FILE__,__LINE__,cBr_no);
                iRc = Cashier_Upd_Upd(sCashier,g_pub_tx.reply);
                if (iRc)
                {
                    vtcp_log("%s,%d,���±�Ʊ������Ϣ�����[%d]\n",__FILE__,__LINE__,iRc);
                    sprintf( acErrMsg,"���±�Ʊ������Ϣ�����");
                    WRITEMSG
                    return(iRc);
                }
                Cashier_Clo_Upd();
                vtcp_log("[%s][%d]���±�Ʊ��Ϣ���[%s]\n",__FILE__,__LINE__,cBr_no);
                iRc = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn=88",cBr_no);
                if (iRc)
                {
                    vtcp_log("%s,%d,���ұ�Ʊǩ��������[%d]\n",__FILE__,__LINE__,iRc);
                    sprintf( acErrMsg,"���ұ�Ʊǩ��������");
                    WRITEMSG
                    return(iRc);
                }
                strncpy(cPayacno,sCom_parm.val,sizeof(cPayacno)-1);
                zip_space(cPayacno);
                memcpy(cAc_no,cPayacno,sizeof(cAc_no)-1);
                vtcp_log("[%s][%d]cAc_no=[%s]\n",__FILE__,__LINE__,cAc_no);
					  	  /**ͨ��brno�õ�com_parm����88���ڲ��ʻ�,����cAc_no**/
				    }
				    else/**end by YH_BP**/
				    {
					      strncpy(cAc_no,sLv_pkgreg.pay_ac_no,sizeof(cAc_no)-1);
					  }
				}
				pub_base_old_acno(cAc_no);
				/* ��鸶���ʺ�״̬�Ƿ�����,�������Բ�����,�Ȳ����� */
      	
      	
      	/* �ֽ��Ŀ������ҵ��Ҫ�ܾ������˿�Ŀ��  Added by ChenMing  2006-11-5 15:26  */
      	if( (strlen(cAc_no)==5 && memcmp(cAc_no, "10101", 5 )==0) ||
      		  (strlen(cAc_no)==7 && memcmp(cAc_no,"1010100",7 )==0 ) )
      	{
      		  iRet = iHvHang_Check_Brno(0, sLv_pkgreg.br_no);  /* ���úû��� */
      	    if(iRet)
      	  	{
      	  		   vtcp_log("%s,%d iHvHang_Check_Brno()  ���� iRet=[%d]",__FILE__,__LINE__,iRet);
   			         sprintf(acErrMsg," iHvHang_Check_Brno()  ���� !! ");
   			         WRITEMSG       		
      	  		   return -1;
      	  	}
      	    
      	    memset(cAc_no, 0 , sizeof(cAc_no));
     				/* �ֽ��Ŀ������ҵ��Ҫ�ܾ������˿�Ŀ�� */
    				iRet=pub_base_GetParm_Str(cKinbr,PARM_LVGZ,cAc_no);
    				if(iRet)
    				{
    					vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĺ����ʺŴ���[%d]",__FILE__,__LINE__,iRet);
    					strncpy(acErrMsg,"�õ������������˺Ŵ���",50);
    					set_zd_data("0130",acErrMsg);
    					return iRet;
    				}  
    				set_zd_data("0980","5");         /* �ܾ� */       	
      	}
		  	vtcp_log("[%s] [%d] cAc_no====[%s]",__FL__,cAc_no);
		  
		  
				/* ������������⣬5��ͷ��ҲΪ1  ChenMing 2006-10-23 17:12*/						
				if(cAc_no[0]=='1'|| cAc_no[0]=='5') 
				{
		  	    vtcp_log("[%s] [%d] cAc_no====[%s]",__FL__,cAc_no);				
					  set_zd_data("0680","1"); 
				}
				/* ����Ӧ�ü�������ǿ� ҲΪ1 wudawei 20131008 */
				else if(memcmp(cAc_no,"621",3) == 0 && strlen(cAc_no) > 13 )
				{
						vtcp_log("[%s] [%d] cAc_no====[%s]",__FL__,cAc_no);				
					  set_zd_data("0680","1"); 
				}
				/* add end wudawei 20131008 */
				else
				{
		  	   vtcp_log("[%s] [%d] cAc_no====[%s]",__FL__,cAc_no);				
					 set_zd_data("0680","6");
				}
				
				/* ���������������0530���е�ֵ */
				/* ��������һ�����ܼ���,���÷�¼Ϊ 0400 Ϊ���׽�� ,0300λ�ͻ��ʺ� 0660 �����Ľ��
				   0320 Ϊ���ɿ�Ŀ,һ��һ��,����0330��¼Ϊ414, 0720 ������
				 */
				 /* For Test */
				char trance_type[2];
				memset(trance_type,0,2); 
	    	get_zd_data("0680",trance_type);			 
	    	vtcp_log("[%s,%d] trance_type=======[%s]",__FILE__,__LINE__,trance_type);	
					 
				set_zd_double("0400",sLv_pkgreg.tx_amt * -1);
				set_zd_long("0530",iStep+1);
				set_zd_data("0300",cAc_no);
				set_zd_data("0660","1");
				set_zd_data("0320",LVLZGD);/* ���ù��ɿ�Ŀ */
				set_zd_data("0330",LV_WAIT_QS);/* 414�ȴ������Ŀ */
				set_zd_data("0720","2");/* 414�ȴ������Ŀ��� */
			}
			break;
		}/*  ��ʼʱ iTiems==iStep==0  �������γ���ѭ��!!! 
		else if(iTimes < iStep)
		{
			iTimes++;
		}
		else
			break;    Rem by ChenMing  2006-10-22 20:13 */
		iTimes++;
		
		if( iTimes > iStep )
		{
			 strcpy(acErrMsg,"�����ˣ������ܵ���������!!! ");WRITEMSG
			 return(-1);					     	     
		}
	}
OkExit:
	memcpy(g_pub_tx.reply,"0000",4);
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
