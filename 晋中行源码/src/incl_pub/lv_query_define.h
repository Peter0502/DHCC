#ifndef _QUERY_	
#define _QUERY_
#define TAB_NUM      20
#define TAB_NUM1     10
#define TAB_LENTH    20
#define TAB_FLAG     2
#define STAT_NUM1    15
#define STAT_NUM     50
#define STAT_NMLEN   30
#define STAT_NMLEN1  20
#define STAT_FLAG    2

/*****返回的重要的标准字段,这里的关键字段只为明细查询使用，历史查询输出包自定义********/ 


   /****************************************/  
   /*******所有支付交易查询的基本部分*******/ 
   /****************************************/     
   typedef struct {
   	         char filename[16];         /***返回交易前台名***/
   	         char txname[20];           /***交易名称****/
   	         char brno[7]; 			    /**交易机构**/
             char wtday[8];             /**交易日期,小额日期   **/
             char orderno[8];           /**支付交易序号**/
             char txtype[12];           /**业务类型,Txnum的描述**/
             char orbrno[12];           /**发起行行号**/
             char payno[12];            /**付款人开户行**/
             char acbrno[12];           /**接收行**/
             char cashno[12];           /**收款人开户行**/
             char txamt[15];            /**交易金额**/
             char payactno[32];         /**付款人帐号**/
             char payaddress[60];       /**付款人地址**/
             char payname[60];          /**付款人户名**/
             char cashactno[32];        /**收款人帐号**/
             char cashname[60];         /**收款人户名**/
             char cashaddress[60];      /**收款人地址**/
             char ywtype[12];           /**业务种类,电汇要把代码转换为描述**/
             char tlrno[3*TLRLENGTH];   /**操作员  **/
             char recvwssrno[18];       /**接收流水号**/ 
             char stat[STAT_NMLEN];     /***交易状态***/
             char ostat[STAT_NMLEN];    /***原交易状态***/
             char operlevel;            /***优先级***/
             char prtflag[12];          /***打印标志这里分得比较细，详见附录 ***/
             char content[60];          /**附言    **/
   }PAY_QUTOS_BASE;
   
   /****************************************/  
   /**所有非支付交易查询(普通类)的基本部分**/ 
   /****************************************/ 
   typedef struct {
              char filename[16];        /***返回交易前台名***/
   	          char txname[20];          /***交易名称****/
   	          char brno[7];      /***交易机构*/
              char orderno[8];          /***支付交易序号*/
              char qday[8];	            /***查询日期***/
			  char cbday[8];            /**查复日期****/
              char qbrno[12];  	        /***查询行行号***/
              char oorderno[8];	        /***原查询书号***/
              char cbbrno[12];	        /***查复行行号***/
              char owtday[8];	        /***原委托日期***/
              char oorbrno[12];	        /***原发起行行号***/
              char oacbrno[12];	        /***原接收行行号***/
              char otype;	            /***原交易种类***/
              char porderno[8];	        /***原支付交易序号***/
              char otxamt[16];	        /***原交易金额***/
              char tlrno[TLRLENGTH];    /***操作员  **/
              char wssrno[6];           /***流水号  **/      
              char stat[STAT_NMLEN];    /***交易状态***/
              char prtflag[3];          /***打印标志*0.未打印 1.支行打印 2.清算中心打印***/
              char qcontent[255];    	/***查询内容***/
              char cbcontent[255];	    /***查复内容***/
   }NOTPAY_TRAN_QUTOS_BASE;
   /****************************************/  
   /**所有非支付交易查询(止付类)的基本部分**/ 
   /****************************************/ 
   typedef struct {
             char filename[16];          /***返回交易前台名***/
   	         char txname[20];            /***交易名称****/
			 char cmtno[3];              /***报文编号***/
             char brno[7];	     /***发送机构号***/       
             char wtday[8];	             /***委托日期***/         
             char reqbrno[12];	         /***申请行***/     
             char orderno[8];	         /***申请号***/     
             char respbrno[12];	         /***应答行***/     
             char reqtype;	             /***申请类型***/                    
             char opkgno[3];             /***原包类型号***/	        
             char opackwtday[8];         /***原包委托日期***/	        
             char opackid[8];            /***原包序号***/	        
             char otxnum[5];             /***原业务类型***/	        
             char owtday[8];             /***原委托日期***/	        
             char oreqday[8];            /***原申请日期***/	        
             char respone;	             /***应答码***/                
             char oorderno[8];	         /***原支付交易序号***/        
             char prtflag[3];	         /***打印标志***/
             char reqqcontent[60];	     /***申请附言***/       
             char respcontent[60];	     /***应答附言***/       
   }NOTPAY_FRZE_QUTOS_BASE;
   /****************************************/  
   /**所有非支付交易查询(信息类)的基本部分**/ 
   /****************************************/
   typedef struct {  
   	         char filename[16];          /***返回交易前台名***/ 
   	         char txname[20];            /***交易名称****/ 
   	         char brno[7];	     /***交易机构***/            
             char wtday[8];	             /***委托日期***/      
             /*char orderno[6];*/
             char orderno[8];	         /***业务编号兼做支付交易序号***/      
             char txnum[5];	             /***业务类型编号***/              
             char orbrno[12];	         /***发起行行号***/      
             char acbrno[12];	         /***接收行行号***/      
             char datatype;	             /***附加数据类型***/              
             char content[255];	         /***附加数据	***/
             char qacttype;	             /***帐号类型	***/      
             char qactno[32];	         /***帐号	***/      
             char qname[60];	         /***户名	***/      
             char qpswdcd;	             /***密码类型	***/      
             char qpasswd[128];	         /***密码验证码***/
             char qtype;	             /***查询类型	***/      
             char respday[8];	         /***应答日期	***/      
             char opackday[8];           /***原批量包日期***/
             char opackid[8];	         /***原批量包id	 ***/      
             char qavbal[16];	         /***当前余额	 ***/      
             char addid[8];	             /***附加数据id	 ***/      
             char sendno[6];	         /***发送操作员号***/              
             char inwssrno[6];           /***录入流水号	 ***/
             char stat[STAT_NMLEN];	     /***交易状态	 ***/      
             char feeflag;	             /***手续费标志	 ***/      
             char feeamt[16];	         /***手续费金额	 ***/
             char prtflag[3];	         /***打印标志	 ***/
             char qcontent[60];	         /***查询附言	***/
   }NOTPAY_INFO_QUTOS_BASE; 
   /****************************************/  
   /**所有非支付交易查询(通知类)的基本部分**/ 
   /****************************************/ 
   typedef struct{ 
              char filename[16];              /***返回交易前台名***/ 
     	      char txname[20];                /***交易名称****/
    	      char brno[7];            /***交易机构***/	                      
              char wtday[8];                  /***委托日期***/
              char opayqsactno[12];           /***原包发起清算行***/             
              char osendco[4];                /***原发起节点***/	             
              char orececo[4];                /***原接收节点***/	             
              char dtlid[3];                  /***明细序号***/	             
              char opkgno[3];                 /***原包类型号***/	               
              char owtday[8];                 /***原包委托日期***/	             
              char opackid[8];                /***原包序号***/	             
              char zcnode;                    /***轧差节点类型***/	             
              char zcday[8];                  /***轧差日期***/	             
              char zcid[8];                   /***轧差场次***/	             
              char packstat[STAT_NMLEN];      /***包处理状态***/	             
              char resp[8];                   /***报文处理码***/	             
              char recvno[6];                 /***接收操作员号***/	             
              char recvwssrno[6];             /***接收流水号***/	             
              char stat[STAT_NMLEN];          /***交易状态***/	             
              char prtflag[3];                /***打印标志***/	             
      }NOTPAY_MESG_QUTOS_BASE;                                       
/*****返回的重要的标准字段********/                                  
                                                                         
/*****状态栏区*******/   
/**等状态确定了以后,把这里的状态数组移到各个查询中去***/
char RET_STAT[STAT_NUM1][STAT_FLAG][STAT_NMLEN1]={
	                     "00" ,"成功",              
	                     "01" ,"帐号不符",          
	                     "02" ,"帐号户名不符",      
	                     "03" ,"账户余额不足支付",  
	                     "10" ,"账户密码错误",      
	                     "11" ,"帐号状态错误",      
	                     "20" ,"业务已撤销",        
	                     "90" ,"其他错误",          
	                     "\0"          
	                       };
char TRAN_STAT[STAT_NUM][STAT_FLAG][STAT_NMLEN]={
	                     "0" ,"无效状态",                      
	                     "1" ,"往账录入",                         
	                     "2" ,"往账复核挂帐",            
	                     "3" ,"往账复核记帐",            
	                     "4" ,"往账发送",                      
	                     "5" ,"往账拒绝",                      
	                     "6" ,"往账扎差",                      
	                     "7" ,"往账排队",                  
	                     "8" ,"往账撤销等待",              
	                     "9" ,"往账撤销",                  
	                     "A" ,"往账退汇等待",              
	                     "B" ,"往账退回",                  
	                     "G" ,"往账异常挂帐",              
	                     "H" ,"临时接收不记帐",           
	                     "I" ,"正式接收正常记帐",        
	                     "J" ,"全国押错",                  
	                     "K" ,"地方押错",                  
	                     "L" ,"全国押和地方押都错",        
	                     "M" ,"信息不符清算中心挂帐",    
	                     "N" ,"信息不符支行挂帐",        
	                     "O" ,"来账退回等待",              
	                     "P" ,"来账退回",                  
	                     "Q" ,"已做同意应答且未发送",      
	                     "R" ,"已做同意应答且已发送",      
	                     "S" ,"已做不同意应答且未发送",    
	                     "T" ,"已做不同意应答且已发送",    
	                     "Z" ,"来账异常挂帐",              
	                     "\0"
	                      };


char LOCALDEF_STAT[STAT_NUM1][STAT_FLAG][STAT_NMLEN1]={
	                       "0" ,"无效",       
	                       "1" ,"正在组包",   
	                       "2" ,"已组包",     
	                       "3" ,"已发送",     
	                       "4" ,"往包已应答", 
	                       "5" ,"已接收",     
	                       "6" ,"已应答",     
	                       "\0"               
	                      };

char CENBANK_STAT[STAT_NUM1][STAT_FLAG][STAT_NMLEN1]={
    	                       "01" ,"已拒绝检查未通过",        
                               "02" ,"已发送检查通过",          
                               "03" ,"已扎差",                   
                               "04" ,"已排队",                   
                               "05" ,"已清算",                   
                               "06" ,"已撤销",                   
                               "10" ,"已回执",                   
                               "11" ,"已拒付",                   
                               "12" ,"已冲正",                   
                               "13" ,"已止付",                   
                               "14" ,"已超时",                   
	                           "\0"
	                      };
/*****状态栏区********/                                          
                            
#endif                                                           
