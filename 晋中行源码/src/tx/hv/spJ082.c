/***************************************************************
* �� �� ��:     spJ082.c
* ����������    �����ϸ��Ϣ��ѯ
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"

spJ082()
{
	int ret = 0;

	FILE 	*fp;
	char 	cZf_type[2];
        char    cBuff[2001];
        char    cFilename[1000];
        char    cWherelist[200];

	HV_NOTPAY_AREA 		hv_notpayin;
        HV_FD123_AREA  		fd123;
        struct hv_zf_c  	hv_zf;
        struct hv_fzf_c 	hv_fzf;
        
	
	memset(cBuff,		0x00,	sizeof(cBuff));
	memset(cFilename,	0x00,	sizeof(cFilename));
	memset(cWherelist,	0x00,	sizeof(cWherelist));
	memset(cZf_type,	0x00,	sizeof(cZf_type));
	memset(&hv_notpayin,	0x00,	sizeof(hv_notpayin));
	memset(&fd123,		0x00,	sizeof(fd123));
	memset(&hv_zf,		0x00,	sizeof(hv_zf));
	memset(&hv_fzf,		0x00,	sizeof(hv_fzf));




        vtcp_log("[%s][%d]���ڿ�ʼ�����ϸ��ѯ",__FILE__,__LINE__);


	/********************
	 * 1.��ʼ��ȫ�ֱ��� *
	 ********************/

        pub_base_sysinit();



	 /********************
	  * 2.��ȡ8583������ *
	  ********************/

        get_zd_data("0680",cZf_type);
         
        ret=iHvTistoNotpay(&hv_notpayin);
        if( ret )
        {
            WRITEMSG
            goto ErrExit;
        }


             
       /**��ɲ�ѯ����**/
       if(cZf_type[0] == '0') /*֧�����ѯ����*/
       {
            MEMCPY_DEBUG(hv_zf.orderno,fd123.orderno,sizeof(hv_zf.orderno)-1);
            vtcp_log("[%s][%d]cZf_type=[%c]",__FILE__,__LINE__,cZf_type);
            sprintf(cWherelist,"br_no='%s' and wt_date='%s' and orderno='%s' and cmtno='%s'",cBr_no,cTx_date, hv_zf.orderno,cZftx_type);
   
       }
       else                   /* ��֧�����ѯ���� */
       {
           MEMCPY_DEBUG(hv_fzf.orderno,fd123.orderno,sizeof(hv_fzf.orderno)-1);
           vtcp_log("here2��[%s][%d]",__FILE__,__LINE__);
           vtcp_log("[%s][%d]cOtype[0]=[%c]",__FILE__,__LINE__,cOtype[0]);
           vtcp_log("+++++hv_fzf.orderno==[%s]!+++++",hv_fzf.orderno);  
           sprintf(cWherelist,"otype='%c' and br_no='%s' and wt_date='%s' and orderno='%s' and cmtno='%s'", cOtype[0],cBr_no,cTx_date,hv_fzf.orderno,cFzftx_type);

       }

       /***֧���������͵�ת��***/
       iNum2TxType(hv_zf.cmtno,cZftx_type);

       /***��֧���������͵�ת��***/
       iNum2TxType(hv_fzf.cmtno,cFzftx_type);
       
       vtcp_log("======�����ϸ��ѯ��ʼ[[%s][%d]",__FILE__,__LINE__);

       /***֧��������Ĳ�ѯ***/
       if(cZf_type[0] == '0')
       {
           char   tmpbuf[15] ;
           memset( tmpbuf, 0x00, sizeof(tmpbuf) );

           ret = Hv_zf_Sel(g_pub_tx.reply, &hv_zf, cWherelist);
           if ( ret==100 )
           {
               sprintf(acErrMsg,"�޴˴��֧������");
               WRITEMSG
               goto    ErrExit;
           }
           else if(ret)
           {
               sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
               WRITEMSG
               goto ErrExit;

           }

           vtcp_log("=======֧���࿪ʼ��ֵ[%s][%d]",__FILE__,__LINE__);

           /**cmtno��***/
           MEMCPY_DEBUG(fd123.cmtno,hv_zf.cmtno,sizeof(fd123.cmtno));
           vtcp_log("[%s][%d] hv_zf.cmtno=[%s]",__FILE__,__LINE__,hv_zf.cmtno);
          
           /**ԭcmt��**/
           MEMCPY_DEBUG(fd123.o_cmtno,hv_zf.o_cmtno,sizeof(fd123.o_cmtno));
           vtcp_log("[%s][%d] hv_zf.o_cmtno=[%s]",__FILE__,__LINE__,hv_zf.o_cmtno);

           /**�����������к�**/
           MEMCPY_DEBUG(fd123.pay_qs_no,hv_zf.pay_qs_no,sizeof(fd123.pay_qs_no));
           vtcp_log("[%s][%d] hv_zf.pay_qs_no=[%s]",__FILE__,__LINE__,hv_zf.pay_qs_no);


           /**�����к� ԭ�����к� */
           MEMCPY_DEBUG(fd123.or_br_no,hv_zf.or_br_no,sizeof(fd123.or_br_no));
           vtcp_log("[%s][%d] hv_zf.or_br_no=[%s]",__FILE__,__LINE__,hv_zf.or_br_no);

           /**�������Ĵ���**/
           MEMCPY_DEBUG(fd123.sendco,hv_zf.sendco,sizeof(fd123.sendco));
           vtcp_log("[%s][%d] hv_zf.sendco=[%s]",__FILE__,__LINE__,hv_zf.sendco);

           /**�ձ����Ĵ���**/
           MEMCPY_DEBUG(fd123.receco,hv_zf.receco,sizeof(fd123.receco));
           vtcp_log("[%s][%d] hv_zf.receco=[%s]",__FILE__,__LINE__,hv_zf.receco);

           /**�����˿�����**/
           MEMCPY_DEBUG(fd123.pay_opn_br_no   ,hv_zf.pay_opn_br_no   ,sizeof(fd123.pay_opn_br_no   ));
           vtcp_log("[%s][%d] hv_zf.pay_opn_br_no=[%s]",__FILE__,__LINE__,hv_zf.pay_opn_br_no);

           /**�������ʺ�(ԭ�տ����ʺ�)**/
           MEMCPY_DEBUG(fd123.pay_ac_no       ,hv_zf.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));
           vtcp_log("[%s][%d] hv_zf.pay_ac_no=[%s]",__FILE__,__LINE__,hv_zf.pay_ac_no);

           /**����������(ԭ�տ�������)**/
           MEMCPY_DEBUG(fd123.pay_name        ,hv_zf.pay_name        ,sizeof(fd123.pay_name        ));
           vtcp_log("[%s][%d] hv_zf.pay_name=[%s]",__FILE__,__LINE__,hv_zf.pay_name);

           /**�����˵�ַ(ԭ�տ��˵�ַ)**/
           MEMCPY_DEBUG(fd123.pay_addr        ,hv_zf.pay_addr        ,sizeof(fd123.pay_addr        ));
           vtcp_log("[%s][%d] hv_zf.pay_addr=[%s]",__FILE__,__LINE__,hv_zf.pay_addr);

           /**���������к�**/
           MEMCPY_DEBUG(fd123.cash_qs_no      ,hv_zf.cash_qs_no      ,sizeof(fd123.cash_qs_no      ));
           vtcp_log("[%s][%d] hv_zf.cash_qs_no=[%s]",__FILE__,__LINE__,hv_zf.cash_qs_no);

           /**�����к�(ԭ�����к�)**/
           MEMCPY_DEBUG(fd123.ac_br_no        ,hv_zf.ac_br_no        ,sizeof(fd123.ac_br_no        ));
           vtcp_log("[%s][%d] hv_zf.ac_br_no=[%s]",__FILE__,__LINE__,hv_zf.ac_br_no);

           /**�տ��˿����к�**/
           MEMCPY_DEBUG(fd123.cash_opn_br_no  ,hv_zf.cash_opn_br_no  ,sizeof(fd123.cash_opn_br_no  ));
           vtcp_log("[%s][%d] hv_zf.cash_opn_br_no=[%s]",__FILE__,__LINE__,hv_zf.cash_opn_br_no);

           /**�տ����ʺ�,ԭ�������ʺ�**/
           MEMCPY_DEBUG(fd123.cash_ac_no      ,hv_zf.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));
           vtcp_log("[%s][%d] hv_zf.cash_ac_no=[%s]",__FILE__,__LINE__,hv_zf.cash_ac_no);

           /**�տ�������,ԭ����������**/
           MEMCPY_DEBUG(fd123.cash_name       ,hv_zf.cash_name       ,sizeof(fd123.cash_name       ));
           vtcp_log("[%s][%d]hv_zf.cash_name=[%s]",__FILE__,__LINE__,hv_zf.cash_name);

           /**�տ��˵�ַ**/
           MEMCPY_DEBUG(fd123.cash_addr       ,hv_zf.cash_addr       ,sizeof(fd123.cash_addr       ));
           vtcp_log("[%s][%d]hv_zf.cash_addr=[%s]",__FILE__,__LINE__,hv_zf.cash_addr);

           /**ҵ������**/
           MEMCPY_DEBUG(fd123.yw_type         ,hv_zf.yw_type         ,sizeof(fd123.yw_type         ));
           vtcp_log("[%s][%d]hv_zf.yw_type=[%s]",__FILE__,__LINE__,hv_zf.yw_type);

           /*������� ����֧���������*/
           MEMCPY_DEBUG(fd123.orderno         ,hv_zf.orderno         ,sizeof(fd123.orderno         ));
           vtcp_log("[%s][%d]hv_zf.orderno=[%s]",__FILE__,__LINE__,hv_zf.orderno);

           /**����**/
           MEMCPY_DEBUG(fd123.brf             ,hv_zf.hv_brf          ,sizeof(fd123.brf             ));
           vtcp_log("[%s][%d]hv_brf=[%s]",__FILE__,__LINE__,hv_zf.hv_brf);

           /**ԭ֧���������**/
           MEMCPY_DEBUG(fd123.o_orderno       ,hv_zf.o_orderno       ,sizeof(fd123.o_orderno       ));
           vtcp_log("[%s][%d]hv_zf.o_orderno=[%s]",__FILE__,__LINE__,hv_zf.o_orderno);

           /**���ȼ�**/
           fd123.operlevel[0]=hv_zf.operlevel[0];
           vtcp_log("[%s][%d]hv_zf.operlevel[0]=[%c]",__FILE__,__LINE__,hv_zf.operlevel[0]);

           /**����״̬**/
/**
           fd123.proc_sts[0]=hv_zf.hv_sts[0];
**/
           vtcp_log("[%s][%d]hv_zf.hv_sts[0]=[%c]",__FILE__,__LINE__,hv_zf.hv_sts[0]);
           get_statname(hv_zf.hv_sts[0],tmpbuf);
           vtcp_log("[%s][%d]tmp=[%s]",__FILE__,__LINE__,tmpbuf);
 
           pub_base_full_space((char*)&fd123,sizeof(fd123));
           set_zd_data("1230",(char*)&fd123);
           vtcp_log("fd123=[%s]",(char*)&fd123);
           set_zd_double("0390",hv_zf.tx_amt);         /* ���׽�� */
           vtcp_log("[%s][%d]hv_zf.tx_amt=[%f]",__FILE__,__LINE__,hv_zf.tx_amt);
              
 

           set_zd_data("0300",tmpbuf);          /* ״̬����*/
           set_zd_data("0580",hv_zf.note_no);          /* ƾ֤�� Ʊ�ݺ���*/
           set_zd_data("1228",hv_zf.cur_no);           /* ���ҷ��� */
           set_zd_data("0890",hv_zf.note_type);        /* ƾ֤���� */
           set_zd_data("1050",hv_zf.hv_brf);           /* ���� 60 */
           vtcp_log("%s,%d hv_zf.o_wt_date=[%d]",__FILE__,__LINE__,hv_zf.o_wt_date);
           set_zd_long("0450",hv_zf.o_wt_date);        /* ԭί������ */
           if(!memcmp(hv_zf.cur_no,"RMB",3))
                       set_zd_data("0210","01");       /* ���ҷ��� */

           /**�õ�������д���ļ�����ȥ**/
           if(memcmp(hv_zf.cmtno,HV_HD,sizeof(hv_zf.cmtno)-1) &&
              memcmp(hv_zf.cmtno,HV_GKZJHH,sizeof(hv_zf.cmtno)-1))
           {
                   FILE *fpw;
                   vtcp_log("%s,%d �õ��ĸ�����=[%s]",__FILE__,__LINE__,cBuff);
                   iHvGetAddit(cTx_date,cTx_date,hv_zf.addid,"000",cBuff,sizeof(cBuff)-1);
                   vtcp_log("%s,%d �õ��ĸ�����=[%s]",__FILE__,__LINE__,cBuff);
                   ret=pub_base_AllDwnFilName(cFilename);
                   if(ret)
                   {
                           vtcp_log("%s,%d �����´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
                   }
                   fpw=fopen(cFilename,"w");
                   if(fpw==NULL)
                   {
                           vtcp_log("%s,%d ���´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
                   }

                   if(!memcmp(hv_zf.cmtno,HV_CJ,sizeof(hv_zf.cmtno)-1))
                   {
                           HV_TYCJ_ADD hv_cj;
                           memset(&hv_cj, 0 , sizeof(hv_cj));
                           MEMCPY_DEBUG((char *)&hv_cj,cBuff, sizeof(hv_cj));
                           vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FILE__,__LINE__,(char *)&hv_cj);
                           memset(cBuff, 0 , sizeof(cBuff));
                           MEMCPY_DEBUG(cBuff,(char *)&hv_cj,sizeof(hv_cj));
                           fprintf(fpw,"%s",cBuff);
                           set_zd_data( DC_FILE_SND,"2" );
                   }
                   if(!memcmp(hv_zf.cmtno,HV_WT,sizeof(hv_zf.cmtno)-1))
                   {
                           HV_WTSK_ADD hv_wt;
                           memset(&hv_wt, 0 , sizeof(hv_wt));
                           MEMCPY_DEBUG((char *)&hv_wt,cBuff, sizeof(hv_wt));
                           vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FILE__,__LINE__,(char *)&hv_wt);
                           memset(cBuff, 0 , sizeof(cBuff));
                           MEMCPY_DEBUG(cBuff,(char *)&hv_wt,sizeof(hv_wt));
                           fprintf(fpw,"%s",cBuff);
                           set_zd_data( DC_FILE_SND,"2" );
                    }
                    if(!memcmp(hv_zf.cmtno,HV_TSCF,sizeof(hv_zf.cmtno)-1))
                    {
                           HV_TSCF_ADD hv_tscf;
                           memset(&hv_tscf, 0 , sizeof(hv_tscf));
                           MEMCPY_DEBUG((char *)&hv_tscf,cBuff, sizeof(hv_tscf));
                           vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FILE__,__LINE__,(char *)&hv_tscf);
                           memset(cBuff, 0 , sizeof(cBuff));
                           MEMCPY_DEBUG(cBuff,(char *)&hv_tscf,sizeof(hv_tscf));
                           fprintf(fpw,"%s",cBuff);
                           set_zd_data( DC_FILE_SND,"2" );
                    } 
                    vtcp_log("cmtno=[%s][%s][%d]",hv_zf.cmtno,__FILE__,__LINE__);
                    if(!memcmp(hv_zf.cmtno,HV_HPYC,sizeof(hv_zf.cmtno)-1))
                    {
                           HV_121_ADD hv_hpyc;
                           memset(&hv_hpyc, 0 , sizeof(hv_hpyc));
                           MEMCPY_DEBUG((char *)&hv_hpyc,cBuff, sizeof(hv_hpyc));
                           vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FILE__,__LINE__,(char *)&hv_hpyc);
                           memset(cBuff, 0 , sizeof(cBuff));
                           MEMCPY_DEBUG(cBuff,(char *)&hv_hpyc,sizeof(hv_hpyc));
                           fprintf(fpw,"%s",cBuff);
                           set_zd_data( DC_FILE_SND,"2" );
                    }
                    fclose(fpw);
           }

       }
       else  /**��֧�����ײ�ѯ**/
       {
          ret =Hv_fzf_Sel(g_pub_tx.reply, &hv_fzf, cWherelist);
          if ( ret==100 )
          {
              sprintf(acErrMsg,"�޴˴��֧������");
              WRITEMSG
              goto    ErrExit;
          }
          else if(ret)
          {
              sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
              WRITEMSG
              goto ErrExit;

          }

          vtcp_log("=======֧���࿪ʼ��ֵ[%s][%d]",__FILE__,__LINE__);

          /**cmtno����**/
          MEMCPY_DEBUG(fd123.cmtno      ,hv_fzf.cmtno           ,sizeof(fd123.cmtno));
          vtcp_log("[%s][%d]==hv_fzf.cmtno=[%s]",__FILE__,__LINE__,hv_fzf.cmtno);

          /**ԭo_cmtno��**/
          MEMCPY_DEBUG(fd123.o_cmtno    ,hv_fzf.o_cmtno ,sizeof(fd123.cmtno));
          vtcp_log("[%s][%d]==hv_fzf.cmtno=[%s]",__FILE__,__LINE__,hv_fzf.cmtno);

          /**�����к� ԭ�����к� **/
          MEMCPY_DEBUG(fd123.or_br_no   ,hv_fzf.or_br_no,sizeof(fd123.or_br_no));
          vtcp_log("[%s][%d]==hv_fzf.or_br_no=[%s]",__FILE__,__LINE__,hv_fzf.or_br_no);

          /**�����������к�**/
          MEMCPY_DEBUG(fd123.pay_qs_no  ,hv_fzf.pay_qs_no,sizeof(fd123.pay_qs_no));
          vtcp_log("[%s][%d]==hv_fzf.pay_qs_no=[%s]",__FILE__,__LINE__,hv_fzf.pay_qs_no);

          /**�������Ĵ���**/
          MEMCPY_DEBUG(fd123.sendco             ,hv_fzf.sendco  ,sizeof(fd123.sendco));
          vtcp_log("[%s][%d]==hv_fzf.sendco=[%s]",__FILE__,__LINE__,hv_fzf.sendco);

          /**�ձ����Ĵ���**/
          MEMCPY_DEBUG(fd123.sendco,hv_fzf.receco  ,sizeof(fd123.receco));
          vtcp_log("[%s][%d]==hv_fzf.receco=[%s]",__FILE__,__LINE__,hv_fzf.receco);

          /**���������к�**/
          MEMCPY_DEBUG(fd123.cash_qs_no ,hv_fzf.cash_qs_no,sizeof(fd123.cash_qs_no));
          vtcp_log("[%s][%d]==hv_fzf.cash_qs_no=[%s]",__FILE__,__LINE__,hv_fzf.cash_qs_no);

          /**�����к�(ԭ�����к�)**/
          MEMCPY_DEBUG(fd123.ac_br_no   ,hv_fzf.ac_br_no,sizeof(fd123.ac_br_no));
          vtcp_log("[%s][%d]==hv_fzf.ac_br_no=[%s]",__FILE__,__LINE__,hv_fzf.ac_br_no);

          /**������� ����֧���������**/
          MEMCPY_DEBUG(fd123.orderno    ,hv_fzf.orderno ,sizeof(fd123.orderno));
          vtcp_log("[%s][%d]==hv_fzf.orderno=[%s]",__FILE__,__LINE__,hv_fzf.orderno);

          /**����**/
          MEMCPY_DEBUG(fd123.brf                ,hv_fzf.hv_brf  ,sizeof(fd123.brf));
          vtcp_log("[%s][%d]==hv_fzf.hv_brf=[%s]",__FILE__,__LINE__,hv_fzf.hv_brf);

          /**ԭ֧���������**/
          MEMCPY_DEBUG(fd123.o_orderno  ,hv_fzf.o_orderno,sizeof(fd123.o_orderno));
          vtcp_log("[%s][%d]==hv_fzf.o_orderno=[%s]",__FILE__,__LINE__,hv_fzf.o_orderno);

          /**��֧������״̬**/
          fd123.proc_sts[0] = hv_fzf.hv_fzf_sts[0];
          vtcp_log("[%s][%d]==hv_fzf.hv_fzf_sts[0]=[%c]",__FILE__,__LINE__,hv_fzf.hv_fzf_sts[0]);

          /**������ʾ**/
          fd123.lw_ind[0] = hv_fzf.lw_ind[0];
          vtcp_log("[%s][%d]==hv_fzf.lw_ind[0]=[%c]",__FILE__,__LINE__,hv_fzf.lw_ind[0]);

          /**Ӧ��״̬**/
          fd123.resp[0]   = hv_fzf.res_sts[0];
          vtcp_log("[%s][%d]==hv_fzf.res_sts[0]=[%c]",__FILE__,__LINE__,hv_fzf.res_sts[0]);

          /**ԭ���׽��**/
          dot_str2dbl(cTxamt ,sizeof(cTxamt),&hv_fzf.o_tx_amt);
          hv_fzf.o_tx_amt = hv_fzf.o_tx_amt/100;
          vtcp_log(" === ԭ���׽�� === hv_fzf.o_tx_amt=[%lf] ",hv_fzf.o_tx_amt);
          set_zd_double("0390", hv_fzf.o_tx_amt);   

          /* ԭί������ */
          set_zd_long("0450",hv_fzf.o_wt_date);   

          /* ԭ�������� */
          set_zd_long("0470",hv_fzf.o_tx_date);
          
          /*ԭ����*/
          set_zd_data("1228",hv_fzf.o_cur_ind);   

          /*ԭ��������*/
          set_zd_data("0710",hv_fzf.o_tx_type);

          /*ԭ(֧������)�������к�*/
          set_zd_data("0310",hv_fzf.o_or_br_no);

          /*ԭ(֧������)�������к�*/
          set_zd_data("0320",hv_fzf.o_ac_br_no);

          /**Ӧ������**/
          if(memcmp(cFzftx_type,"314",sizeof(cFzftx_type)-1)==0 || memcmp(cFzftx_type,"302",sizeof(cFzftx_type)-1)==0)
               MEMCPY_DEBUG(fd123.notpay_content,hv_fzf.res_content,sizeof(fd123.notpay_content));
          else    /**��������**/
               MEMCPY_DEBUG(fd123.notpay_content,hv_fzf.req_content,sizeof(fd123.notpay_content));


          pub_base_full_space((char*)&fd123,sizeof(fd123));
          set_zd_data("1230",(char*)&fd123);
          vtcp_log("fd123=[%s]",(char*)&fd123);

       }


OkExit:
       strcpy(g_pub_tx.reply,"0000");
       sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
       WRITEMSG
       set_zd_data(DC_REPLY,g_pub_tx.reply);
       return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
       sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
       WRITEMSG
       set_zd_data(DC_REPLY,g_pub_tx.reply);
       return 1;

}

/**��������**/
int iNum2TxType(char *ch,char *cTmpList)
{
          if(!memcmp(ch,"100",3))
          {           
                strcpy(cTmpList,"�����");
          } 
          else if(!memcmp(ch,"101",3))
          {
               strcpy(cTmpList,"ί���տ�");
          }
          else if(!memcmp(ch,"105",3))
          {
               strcpy(cTmpList,"ͬҵ���");
          }
          else if(!memcmp(ch,"102",3))
          {
               strcpy(cTmpList,"���ճи�");
          }
          else if(!memcmp(ch,"109",3))
          {
               strcpy(cTmpList,"��������");
          }
          else if(!memcmp(ch,"108",3))
          {
               strcpy(cTmpList,"֧���˻�");
          }
          else if(!memcmp(ch,"313",3))
          {
                strcpy(cTmpList,"�����˻�");
          }
          else if(!memcmp(ch,"314",3))
          {
                strcpy(cTmpList,"�˻�Ӧ��");
          }
          else if(!memcmp(ch,"301",3))
          {
                strcpy(cTmpList,"����ѯ");
          }
          else if(!memcmp(ch,"302",3))
          {
               strcpy(cTmpList,"���鸴");
          }
          else if(!memcmp(ch,"303",3))
          {
               strcpy(cTmpList,"���ɸ�ʽ");
          }
          else if(!memcmp(ch,"311",3))
          {
                strcpy(cTmpList,"���볷��");
          }
          else if(!memcmp(ch,"407",3))
          {
                strcpy(cTmpList,"��Ѻ����");
          }
          else if(!memcmp(ch,"408",3))
          {
                strcpy(cTmpList,"��Ѻ���ʿۿ�");
          }
          else if(!memcmp(ch,"121",3))
          {
                strcpy(cTmpList,"�ʽ��ƴ�");
          }
          else if(!memcmp(ch,"721",3))
          {
               strcpy(cTmpList,"�����ʽ�����");
          }
          else if(!memcmp(ch,"122",3))
          {
               strcpy(cTmpList,"�����ʽ�����");
          }
          else if(!memcmp(ch,"123",3))
          {
               strcpy(cTmpList,"�����ʽ��˻�");
          }
          else if(!memcmp(ch,"725",3))
          {
                strcpy(cTmpList,"ȫ��Ҹ�֪ͨ");
          }
          else if(!memcmp(ch,"724",3))
          {
                strcpy(cTmpList,"��Ʊ�����˻�");
          }
          else if(!memcmp(ch,"124",3))
          {
               strcpy(cTmpList,"��Ʊδ���˻�");
          }
          else
               strcpy(cTmpList,"δ֪��������");

          return (0);
}

int get_statname( char stat, char *statname )
{
    switch(stat )
    {
         case '0':
            strcpy( statname, "���˵ǼǼ���" );
            break;
         case '1':
            strcpy( statname, "���˼��˵ȴ�" );
            break;
         case '2':
            strcpy( statname, "���˼�������" );
            break;
         case '3':
            strcpy( statname, "�����˻ؼ���" );
            break;
         case '4':
            strcpy( statname, "�����˻صȴ�" );
            break;
         case '5':
            strcpy( statname, "���˾ܾ�" );
            break;
         case '6':
            strcpy( statname, "���˳�������" );
            break;
         case '7':
            strcpy( statname, "���˳����ȴ�" );
            break;
         case '8':
            strcpy( statname, "���˾ܾ�" );
            break;
         case '9':
            strcpy( statname,"���˼�������" );
            break;
         case 'A':
            strcpy( statname,"���˵Ǽ�ȡ��" );
            break;
         case 'B':
            strcpy( statname,"�����˻�" );
            break;
         case 'C':
            strcpy( statname,"���ʵǼ�" );
            break;
         case 'D':
            strcpy( statname,"���ʸ���ȡ��" );
            break;
         case 'E':
            strcpy( statname,"���˼��˵Ǽ�" );
            break;
         case 'F':
            strcpy( statname,"���˺˵ط�Ѻ��" );
            break;
         case 'G':
            strcpy( statname,"���˺�ȫ��Ѻ��" );
            break;
         case 'H':
            strcpy( statname,"�������ݲ���" );
            break;
         case 'I':
            strcpy( statname,"���˻�������" );
            break;
         case 'J':
            strcpy( statname,"�����Ŷ���" );
            break;
         case 'K':
            strcpy( statname,"�˶��������" );
            break;
         default:
            strcpy( statname, "��Ч״̬" );
            break;
   }
} 
