/***************************************************************************
�ļ�����: sp6351.c                                                      
��������: ���б�ƱС��֧����Ϣ��ѯ 
��    �ߣ�
������ڣ� 
�޸ļ�¼�� 
��    ��: 
�� �� ��:
�޸�����:                                            
��    ע:                                          
***************************************************************************/
#include "stdio.h"
#include "string.h"

#include "stdlib.h"
#include "public.h"
#include "lv_pkgreg_c.h"
#include "lv_param_c.h"
#include "bplosthis_c.h"
#include "lv_define.h"

sp6351()
{
    struct lv_pkgreg_c lv_pkgreg;
    struct lv_param_c  lv_param;
    int    ret=0;
    int    ipkgcnt=0;
    int    rdCnt=0;
    
    long   lSdate=0;         /**��ʼ����**/
    long   lEdate=0;         /**��������**/
    
    char   Parname[31];     
    char   clwflag[6];       /**������ʶ**/
    char   cLvstat[30];      /**����״̬**/
    char   cHzstat[32];      /**��ִ״̬**/
    char   cKinbr[BRNO_LEN+1];
    char   filename[101];  
    char   cTmpstr[201];
    char   cPkg_ind[2];
    char   cLw_ind[2];
    char   wherelist[501];   /**��ѯ����**/
    FILE   *fp;
      
    /****��ʼ������****/
    memset(&lv_param  , 0 , sizeof(struct lv_param_c));
    memset(&lv_pkgreg , 0 , sizeof(struct lv_pkgreg_c));
    memset(filename   , 0 , sizeof(filename));
    memset(cTmpstr    , 0 , sizeof(cTmpstr));
    memset(cKinbr     , 0 , sizeof(cKinbr));
    memset(clwflag    , 0 , sizeof(clwflag));
    memset(cLvstat    , 0 , sizeof(cLvstat));
    memset(cHzstat    , 0 , sizeof(cHzstat));
    memset(wherelist  , 0 , sizeof(wherelist));
    memset(Parname  , 0 , sizeof(Parname));
    memset(cPkg_ind  , 0 , sizeof(cPkg_ind));
    memset(cLw_ind   , 0 , sizeof(cLw_ind));
       
    /***��ʼ��ȫ�ֱ���****/
    pub_base_sysinit();
   /****����ǰ̨����ֵ****/
    vtcp_log("[%s][%d]sp6351��ʼ��\n",__FILE__,__LINE__);
    get_zd_long("0440",&lSdate);
    vtcp_log("[%s],[%d]��ʼ����:[%d]\n",__FILE__,__LINE__,lSdate);
    get_zd_long( "0450",&lEdate);
    vtcp_log("[%s],[%d]��������:[%d]\n",__FILE__,__LINE__,lEdate);
    get_zd_data("0910",cKinbr);
    vtcp_log("[%s][%d]���׻���Ϊ[%s]\n",__FILE__,__LINE__,cKinbr);
    get_zd_data("0720",cPkg_ind);
    vtcp_log("[%s][%d]ҵ������[%s]\n",__FILE__,__LINE__,cPkg_ind);
    get_zd_data("0710",cLw_ind);
    vtcp_log("[%s][%d]������־[%s]\n",__FILE__,__LINE__,cLw_ind);
    if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5)!=0 )
    {
    	  if(strlen(cKinbr)==0)
    	  {
            vtcp_log("[%s][%d][%s]���������Ĳ�ѯ���׻�������Ϊ��!\n",__FILE__,__LINE__,g_pub_tx.tx_br_no);
            strcpy(g_pub_tx.reply,"P075");
            goto ErrExit;
        }
        else if(memcmp(g_pub_tx.tx_br_no,cKinbr,5)!=0)
        {
            vtcp_log("[%s][%d][%s]��������,���׻���[%.5s]!\n",__FILE__,__LINE__,LV_CZ_QSBRNO,g_pub_tx.tx_br_no);
            strcpy(g_pub_tx.reply,"P075");
            goto ErrExit;
        }
    }
    
    /***�к�***/
    memset(cTmpstr    , 0 , sizeof(cTmpstr));
    if(cKinbr[0]!=0x00)
    {
        sprintf( cTmpstr,"br_no='%s' and ",cKinbr);
        strcat(wherelist,cTmpstr);
    }
    /**������־**/
    memset(cTmpstr    , 0 , sizeof(cTmpstr));
    if(cLw_ind[0]!='0')
    {
    	  sprintf( cTmpstr,"lw_ind='%s' and  ",cLw_ind);
        strcat(wherelist,cTmpstr);
    }
    /**ҵ������**/
    memset(cTmpstr    , 0 , sizeof(cTmpstr));
    if(cPkg_ind[0]!='0')
    {
    	  if(cPkg_ind[0]=='1')
    	  {
    	      sprintf( cTmpstr,"pkgno='004' and ");
    	  }
    	  else
    	  {
    	  	  sprintf( cTmpstr,"pkgno='010' and ");
    	  }
        strcat(wherelist,cTmpstr);
    }
    else
    {
    	  sprintf( cTmpstr,"pkgno in ('004','010') and  ");
        strcat(wherelist,cTmpstr);
    }
    memset(cTmpstr    , 0 , sizeof(cTmpstr));

    sprintf(cTmpstr," wt_date>=%ld and wt_date<=%ld  and txnum='30103' and ",lSdate,lEdate);

    strcat(wherelist,cTmpstr);
    sprintf(wherelist+strlen(wherelist), " 1=1 order by wt_date,orderno,packid");
    vtcp_log("[%s][%d]wherelist[%s]\n",__FILE__,__LINE__,wherelist);
    ipkgcnt=sql_count("lv_pkgreg",wherelist);
    
    if(ipkgcnt<=0)
    {
        vtcp_log("[%s][%d]---------δ�ҵ����������ļ�¼!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"S049");
        goto ErrExit;
    }
    
    ret=Lv_pkgreg_Dec_Sel(g_pub_tx.reply,wherelist);
    vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
    if(ret)
    {
        vtcp_log("[%s],[%d] �����α�ʧ��!",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    while(1)
    {
    	memset(&lv_param  , 0 , sizeof(struct lv_param_c));
    	memset(cHzstat    , 0 , sizeof(cHzstat));
    	memset(Parname  , 0 , sizeof(Parname));
    	memset(clwflag    , 0 , sizeof(clwflag));
    	memset(cLvstat    , 0 , sizeof(cLvstat));
        strcpy(cHzstat,"TPLY");
        vtcp_log("[%s],[%d]*****************\n",__FILE__,__LINE__);
        ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
        vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
        if(100 == ret) break;
        if(ret){
            vtcp_log("[%s],[%d] ȡֵʧ��!",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"D103");
            goto ErrExit;
        }
        vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,rdCnt);
        if(!rdCnt)
        {
            pub_base_AllDwnFilName( filename );
            fp = fopen( filename,"w" );
            if( fp==NULL ){
                sprintf(acErrMsg," open file [%s] error ",filename );
                WRITEMSG
                strcpy( g_pub_tx.reply,"S047" );
                goto ErrExit;
            }
            fprintf( fp, "~@pkg���|�������к�|�������|�������к�|��������|ί������|�������|���׽��|�տ����ʺ�|�տ�������|Ӧ������|��ִ״̬|���׻���|����״̬|�����|������ʶ|\n");
        }
        switch(lv_pkgreg.lv_sts[0])
        {
            case '0':
                  strcpy(cLvstat,"��Ч״̬");
                  break;
            case '1':
                  strcpy(cLvstat,"����¼��");
                  break;
            case '2':
                  strcpy(cLvstat,"���˸��ˣ�����");
                  break;
            case '3':
                  strcpy(cLvstat,"���˸��ˣ�����");
                  break;
            case '4':
                  strcpy(cLvstat,"���˷���");
                  break;
            case '5':
                  strcpy(cLvstat,"���˾ܾ�");
                  break;
            case '6':
                  strcpy(cLvstat,"��������");
                  break;
            case '7':
                  strcpy(cLvstat,"�����Ŷ�");
                  break;
            case '8':
                  strcpy(cLvstat,"���˳����ȴ�");
                  break;
            case '9':
                  strcpy(cLvstat,"���˳���");
                  break;
            case 'A':
                  strcpy(cLvstat,"�����˻ء�������ֹ���ȴ�");
                  break;
            case 'B':
                  strcpy(cLvstat,"�����˻�");
                  break;
            case 'C':
                  strcpy(cLvstat,"�����ѳ���");
                  break;
            case 'D':
                  strcpy(cLvstat,"������ֹ��");
                  break;
            case 'G':
                  strcpy(cLvstat,"�����쳣����");
                  break;
            case 'H':
                  strcpy(cLvstat,"��ʱ����,������");
                  break;
            case 'I':
                  strcpy(cLvstat,"��ʽ���գ���������");
                  break;
            case 'J':
                  strcpy(cLvstat,"ȫ��Ѻ�� -��֧��");
                  break;
            case 'K':
                  strcpy(cLvstat,"�ط�Ѻ�� -��֧��");
                  break;
            case 'L':
                  strcpy(cLvstat,"ȫ��Ѻ�͵ط�Ѻ����");
                  break;
            case 'M':
                  strcpy(cLvstat,"��Ϣ�������������Ĺ���");
                  break;
            case 'N':
                  strcpy(cLvstat,"��Ϣ������֧�й���");
                  break;      
            case 'O':
                  strcpy(cLvstat,"�����˻صȴ�");
                  break;
            case 'P':
                  strcpy(cLvstat,"�����˻�");
                  break;
            case 'Q':
                  strcpy(cLvstat,"����ͬ��Ӧ����δ����");
                  break;
            case 'R':
                  strcpy(cLvstat,"����ͬ��Ӧ�����ѷ���");
                  break;
            case 'S':
                  strcpy(cLvstat,"������ͬ��Ӧ����δ����");
                  break;
            case 'T':
                  strcpy(cLvstat,"������ͬ��Ӧ�����ѷ���");
                  break;
            case 'U':
                  strcpy(cLvstat,"�����ѳ���");
                  break; 
            case 'V':
                  strcpy(cLvstat,"������ֹ��");
                  break;
            case 'W':
                  strcpy(cLvstat,"�����Ѹ���");
                  break;
            case 'Z':
                  strcpy(cLvstat,"�����쳣����");
                  break;
            default:
                  vtcp_log("�ѵ���ȥ��");
                  break;
        }
       
        if(lv_pkgreg.lw_ind[0]=='1')
        {
            strcpy(clwflag,"����"); 
        }    
        if(lv_pkgreg.lw_ind[0]=='2')
        {
            strcpy(clwflag,"����"); 
        }    
        vtcp_log("[%s][%d]lv_pkgreg.rcpstat=[%s]\n",__FILE__,__LINE__,lv_pkgreg.rcpstat);
        
        strcat(cHzstat,lv_pkgreg.rcpstat);
        ret=Lv_param_Sel(g_pub_tx.reply,&lv_param,"par_code='%s'",cHzstat);
        
        if (ret==100)
        {
            
            vtcp_log("[%s],[%d] �޻�ִ״̬!",__FILE__,__LINE__);
            strcpy(Parname,"�޻�ִ״̬");
            
        }
        else if (ret)
        {
            vtcp_log("[%s],[%d] ��ѯLv_paramʧ��!",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"D101");
            goto ErrExit;
        }
        else if (ret==0)
        {
        	strcpy(Parname,lv_param.par_name);
        	
        }
        fprintf( fp, "%-3s|%-13s|%-8s|%-13s|%-8ld|%-8ld|%-8ld|%.2f|%-20s|%-32s|%-8ld|%-30s|%-7s|%-30s|%-8s|%-6s|\n",
        lv_pkgreg.pkgno,
        lv_pkgreg.or_br_no,
        lv_pkgreg.orderno,
        lv_pkgreg.ac_br_no,  
        lv_pkgreg.in_date,
        lv_pkgreg.wt_date,
        lv_pkgreg.pack_date,
        lv_pkgreg.tx_amt,
        lv_pkgreg.cash_ac_no,
        lv_pkgreg.cash_name,
        lv_pkgreg.resp_date,
        Parname,
        lv_pkgreg.br_no,
        cLvstat,
        lv_pkgreg.packid,
        clwflag);           
      
        rdCnt ++;
        vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,rdCnt);
        
    }
    vtcp_log("[%s][%d]��ѯ��¼���!\n",__FILE__,__LINE__);
    if( !rdCnt )
    {
        strcpy( g_pub_tx.reply,"S049" );
        goto ErrExit;
    }
    else
    {
        fclose(fp);
        set_zd_data( DC_FILE_SND,"1" );
    }
    
    Lv_pkgreg_Clo_Sel();
                                                           
    vtcp_log("[%s][%d]========�������",__FILE__,__LINE__);
    
OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"errcode: reply is[%s]\n",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
