/*************************************************
*2101的证件快捷查询
* 文 件 名: spEB07.c
* 功能描述：根据证件类型和证件号码查询个人客户基本信息
* 作    者: 李洋，孙朋君
* 完成日期：2010年10月23日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_per_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_email_inf_c.h"
#include "mob_acct_type_c.h"

int spEB07()
{
        /** 数据初始化 **/
        struct cif_id_code_rel_c cif_id_code_rel;
        struct cif_basic_inf_c cif_basic_inf;
        struct cif_per_inf_c cif_per_inf;
        struct cif_addr_inf_c cif_addr_inf;
        struct cif_email_inf_c cif_email_inf;
				struct mob_acct_type_c sMob_acct_type;	/*短信银行客户账号与服务品种对应表*/
				struct dd_mst_c sDd_mst;	
        char   cOutBuf[21];
        char   temp[200]={0};
        int    ret = 0;
        char  Name[60+1];
				char ac_id_str[20]={0};
				char Ac_id_str[2000]={0};
				
				memset(Name,0x00, sizeof(Name));
        memset(&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
        memset(&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
        memset(&cif_per_inf, 0x00, sizeof(struct cif_per_inf_c));
        memset(&cif_addr_inf, 0x00, sizeof(struct cif_addr_inf_c));
        memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
				memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
				memset(&sDd_mst,0x00,sizeof(sDd_mst));
        memset(cOutBuf,0x00,sizeof(cOutBuf));

        /** 取值、赋值 **/
        get_zd_data("0250", Name);   /* 客户姓名 */
        get_zd_data("0680", cif_id_code_rel.id_type);   /* 证件类型 */
        get_zd_data("0630", cif_id_code_rel.id_no);     /* 证件号码 */
        vtcp_log("[%s].........[%s]", cif_id_code_rel.id_type, cif_id_code_rel.id_no);

        /* 根据证件类型和证件号码取客户号 */
       ret = Cif_id_code_rel_Sel(g_pub_tx.reply, &cif_id_code_rel,
                   "id_type = '%s' and id_no = '%s'", cif_id_code_rel.id_type, \
                                  cif_id_code_rel.id_no);
        if (ret == 100) 
        {
                if (cif_id_code_rel.id_type[0] == '1')
                {
                	
                }
                sprintf(acErrMsg, "该证件信息没有登记,请到客户中心登记[%s]", \
                        g_pub_tx.reply);
                WRITEMSG
                        strcpy(g_pub_tx.reply, "D104");
                goto ErrExit;
        } 
        else if (ret) 
        {
                sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
                WRITEMSG
                        strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
        }
        /* 输出变量 */
        set_zd_long("0280",cif_id_code_rel.cif_no );    /*客户号*/
        /* 根据客户号查询客户名称 */
        ret = Cif_basic_inf_Sel(g_pub_tx.reply, &cif_basic_inf,
                                " cif_no=%ld ", cif_id_code_rel.cif_no);
        if (ret == 100) 
        {
                sprintf(acErrMsg, "该证件对应的客户号不存在[%s]", g_pub_tx.reply);
                WRITEMSG
                        strcpy(g_pub_tx.reply, "D104");
                goto ErrExit;
        } 
        else if (ret) 
        {
                sprintf(acErrMsg, "取客户基本信息异常![%s]", g_pub_tx.reply);
                WRITEMSG
                        strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
        }
        /* 输出变量 */
       
        if(strcmp(Name,cif_basic_inf.name)!= 0)
	      {
	       	sprintf(acErrMsg,"[%s],[%d]客户姓名不符!输入name=[%s],表中cif_basic_inf.name=[%s]",__FILE__,__LINE__,Name,cif_basic_inf.name);
		  		WRITEMSG
		  		strcpy(g_pub_tx.reply,"D249");
		 			goto ErrExit;
	      }
	      
	       sprintf(acErrMsg,"[%s],[%d]Name=[%s],账户客户名=[%s],",__FILE__,__LINE__,Name,cif_basic_inf.name);
       WRITEMSG
        pub_base_strpack(cif_basic_inf.name);	
        set_zd_data("0250", cif_basic_inf.name);
        
        /* 根据客户号查询客户性别*/
        ret = Cif_per_inf_Sel(g_pub_tx.reply, &cif_per_inf,
                                " cif_no = %ld ", cif_id_code_rel.cif_no);
        if (ret != 100 && ret != 0)
        {
        	sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
            	WRITEMSG
                   	strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
        }
        set_zd_data("0660", cif_per_inf.sex);
        
        /* 根据客户号查询客户地址和邮政编码*/
        ret = Cif_addr_inf_Sel(g_pub_tx.reply, &cif_addr_inf,
                                " cif_no = %ld and addr_type = '2'", cif_id_code_rel.cif_no);
        if (ret != 100 && ret != 0)
        {
        	sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
            	WRITEMSG
                   	strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
        }                        
        set_zd_data("0260", cif_addr_inf.addr);
        set_zd_data("0640", cif_addr_inf.post_code);
        
        /* 根据客户号查询客户联系电话、手机号码及邮箱*/
        memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
        ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf,
                                " cif_no = %ld and addr_type = '1'", cif_id_code_rel.cif_no);
        if (ret != 100 && ret != 0)
        {
        	sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
            	WRITEMSG
                   	strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
        }
        set_zd_data("0620", cif_email_inf.email);
       
        memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
        ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf,
                                " cif_no = %ld and addr_type = '5'", cif_id_code_rel.cif_no);
        if (ret != 100 && ret != 0)
        {
        	sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
            	WRITEMSG
                   	strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
        }
        
        /*将手机号传给前台*/        
        set_zd_data("0610",cif_email_inf.email);
        
				/*查询dd_mst表，客户号对应的账号
				ret = Dd_mst_Dec_Sel(g_pub_tx.reply," cif_no='%ld' ",cif_id_code_rel.cif_no);
				if( ret )
			  {
			  	vtcp_log("[%s][%d]查询dd_mst错误![%d]\n",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"W015");
			    goto ErrExit;
				}
				while(1)
				{
					ret=Dd_mst_Fet_Sel(&sDd_mst,g_pub_tx.reply);
					if(ret==100)break;
			   		else if( !ret )
			   		{
							sprintf(ac_id_str,"%d,",sDd_mst.ac_id);
			 				strcat(Ac_id_str,ac_id_str);									
			   		}
			   		else if( ret!=100 )
			   		{
			        	vtcp_log("[%s],[%d] 查询dd_mst异常!",__FILE__,__LINE__);
								strcpy(g_pub_tx.reply,"W015");
			       		goto ErrExit;
			   		}
				}
				Dd_mst_Clo_Sel();
				/substr(Ac_id_str,0,len(Ac_id_str)-1);/
				Ac_id_str[strlen(Ac_id_str)-1]='\0';
				/memcpy (temp,Ac_id_str,strlen(Ac_id_str)-1);/
				
				vtcp_log("[%s],[%d] Ac_id_str=[%s]!",__FILE__,__LINE__,Ac_id_str);
				
				/查询短信银行客户账号与服务品种对应表/
				ret = Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_id in (%s) and mob_sts <> '3' ",Ac_id_str);
				
				if(ret == 100)
				{
					vtcp_log("[%s],[%d] 此客户未做过短信签约或已解约!",__FILE__,__LINE__);
					/goto ErrExit;/
				}
				else if( ret )
				{
					vtcp_log("[%s][%d]查询表Mob_acct_type_Sel错误!",__FILE__,__LINE__);
			    strcpy(g_pub_tx.reply,"D104");
			    return -1; 
				}
				else
				{
				sprintf(acErrMsg,"检查短信银行客户账号与服务品种对应表 PASS!");
				WRITEMSG
				
				/将手机号传给前台/        
        		set_zd_data("0610",sMob_acct_type.mobile);
				
				}	
			*/
        /**set_zd_data("0610","18735431293");**/  /** 测试用设定手机号返回 20141120 lzy**/
				
        memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
				ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf,
                                " cif_no = %ld and addr_type = '6'", cif_id_code_rel.cif_no);
        if (ret != 100 && ret != 0) 
        {
					sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply, "D103");
					goto ErrExit;
        }
        set_zd_data("0810", cif_email_inf.email);

        /*
        sprintf(g_pub_tx.ac_no, "%ld", cif_id_code_rel.cif_no);
        set_zd_data("0300", g_pub_tx.ac_no);
        sprintf(g_pub_tx.ac_no, "%ld", cif_id_code_rel.cif_no);
        */

OkExit:
        strcpy(g_pub_tx.reply, "0000");
        sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
        WRITEMSG
                set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
        WRITEMSG
                set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 1;
}
