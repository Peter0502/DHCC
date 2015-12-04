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

/*****���ص���Ҫ�ı�׼�ֶ�,����Ĺؼ��ֶ�ֻΪ��ϸ��ѯʹ�ã���ʷ��ѯ������Զ���********/ 


   /****************************************/  
   /*******����֧�����ײ�ѯ�Ļ�������*******/ 
   /****************************************/     
   typedef struct {
   	         char filename[16];         /***���ؽ���ǰ̨��***/
   	         char txname[20];           /***��������****/
   	         char brno[7]; 			    /**���׻���**/
             char wtday[8];             /**��������,С������   **/
             char orderno[8];           /**֧���������**/
             char txtype[12];           /**ҵ������,Txnum������**/
             char orbrno[12];           /**�������к�**/
             char payno[12];            /**�����˿�����**/
             char acbrno[12];           /**������**/
             char cashno[12];           /**�տ��˿�����**/
             char txamt[15];            /**���׽��**/
             char payactno[32];         /**�������ʺ�**/
             char payaddress[60];       /**�����˵�ַ**/
             char payname[60];          /**�����˻���**/
             char cashactno[32];        /**�տ����ʺ�**/
             char cashname[60];         /**�տ��˻���**/
             char cashaddress[60];      /**�տ��˵�ַ**/
             char ywtype[12];           /**ҵ������,���Ҫ�Ѵ���ת��Ϊ����**/
             char tlrno[3*TLRLENGTH];   /**����Ա  **/
             char recvwssrno[18];       /**������ˮ��**/ 
             char stat[STAT_NMLEN];     /***����״̬***/
             char ostat[STAT_NMLEN];    /***ԭ����״̬***/
             char operlevel;            /***���ȼ�***/
             char prtflag[12];          /***��ӡ��־����ֵñȽ�ϸ�������¼ ***/
             char content[60];          /**����    **/
   }PAY_QUTOS_BASE;
   
   /****************************************/  
   /**���з�֧�����ײ�ѯ(��ͨ��)�Ļ�������**/ 
   /****************************************/ 
   typedef struct {
              char filename[16];        /***���ؽ���ǰ̨��***/
   	          char txname[20];          /***��������****/
   	          char brno[7];      /***���׻���*/
              char orderno[8];          /***֧���������*/
              char qday[8];	            /***��ѯ����***/
			  char cbday[8];            /**�鸴����****/
              char qbrno[12];  	        /***��ѯ���к�***/
              char oorderno[8];	        /***ԭ��ѯ���***/
              char cbbrno[12];	        /***�鸴���к�***/
              char owtday[8];	        /***ԭί������***/
              char oorbrno[12];	        /***ԭ�������к�***/
              char oacbrno[12];	        /***ԭ�������к�***/
              char otype;	            /***ԭ��������***/
              char porderno[8];	        /***ԭ֧���������***/
              char otxamt[16];	        /***ԭ���׽��***/
              char tlrno[TLRLENGTH];    /***����Ա  **/
              char wssrno[6];           /***��ˮ��  **/      
              char stat[STAT_NMLEN];    /***����״̬***/
              char prtflag[3];          /***��ӡ��־*0.δ��ӡ 1.֧�д�ӡ 2.�������Ĵ�ӡ***/
              char qcontent[255];    	/***��ѯ����***/
              char cbcontent[255];	    /***�鸴����***/
   }NOTPAY_TRAN_QUTOS_BASE;
   /****************************************/  
   /**���з�֧�����ײ�ѯ(ֹ����)�Ļ�������**/ 
   /****************************************/ 
   typedef struct {
             char filename[16];          /***���ؽ���ǰ̨��***/
   	         char txname[20];            /***��������****/
			 char cmtno[3];              /***���ı��***/
             char brno[7];	     /***���ͻ�����***/       
             char wtday[8];	             /***ί������***/         
             char reqbrno[12];	         /***������***/     
             char orderno[8];	         /***�����***/     
             char respbrno[12];	         /***Ӧ����***/     
             char reqtype;	             /***��������***/                    
             char opkgno[3];             /***ԭ�����ͺ�***/	        
             char opackwtday[8];         /***ԭ��ί������***/	        
             char opackid[8];            /***ԭ�����***/	        
             char otxnum[5];             /***ԭҵ������***/	        
             char owtday[8];             /***ԭί������***/	        
             char oreqday[8];            /***ԭ��������***/	        
             char respone;	             /***Ӧ����***/                
             char oorderno[8];	         /***ԭ֧���������***/        
             char prtflag[3];	         /***��ӡ��־***/
             char reqqcontent[60];	     /***���븽��***/       
             char respcontent[60];	     /***Ӧ����***/       
   }NOTPAY_FRZE_QUTOS_BASE;
   /****************************************/  
   /**���з�֧�����ײ�ѯ(��Ϣ��)�Ļ�������**/ 
   /****************************************/
   typedef struct {  
   	         char filename[16];          /***���ؽ���ǰ̨��***/ 
   	         char txname[20];            /***��������****/ 
   	         char brno[7];	     /***���׻���***/            
             char wtday[8];	             /***ί������***/      
             /*char orderno[6];*/
             char orderno[8];	         /***ҵ���ż���֧���������***/      
             char txnum[5];	             /***ҵ�����ͱ��***/              
             char orbrno[12];	         /***�������к�***/      
             char acbrno[12];	         /***�������к�***/      
             char datatype;	             /***������������***/              
             char content[255];	         /***��������	***/
             char qacttype;	             /***�ʺ�����	***/      
             char qactno[32];	         /***�ʺ�	***/      
             char qname[60];	         /***����	***/      
             char qpswdcd;	             /***��������	***/      
             char qpasswd[128];	         /***������֤��***/
             char qtype;	             /***��ѯ����	***/      
             char respday[8];	         /***Ӧ������	***/      
             char opackday[8];           /***ԭ����������***/
             char opackid[8];	         /***ԭ������id	 ***/      
             char qavbal[16];	         /***��ǰ���	 ***/      
             char addid[8];	             /***��������id	 ***/      
             char sendno[6];	         /***���Ͳ���Ա��***/              
             char inwssrno[6];           /***¼����ˮ��	 ***/
             char stat[STAT_NMLEN];	     /***����״̬	 ***/      
             char feeflag;	             /***�����ѱ�־	 ***/      
             char feeamt[16];	         /***�����ѽ��	 ***/
             char prtflag[3];	         /***��ӡ��־	 ***/
             char qcontent[60];	         /***��ѯ����	***/
   }NOTPAY_INFO_QUTOS_BASE; 
   /****************************************/  
   /**���з�֧�����ײ�ѯ(֪ͨ��)�Ļ�������**/ 
   /****************************************/ 
   typedef struct{ 
              char filename[16];              /***���ؽ���ǰ̨��***/ 
     	      char txname[20];                /***��������****/
    	      char brno[7];            /***���׻���***/	                      
              char wtday[8];                  /***ί������***/
              char opayqsactno[12];           /***ԭ������������***/             
              char osendco[4];                /***ԭ����ڵ�***/	             
              char orececo[4];                /***ԭ���սڵ�***/	             
              char dtlid[3];                  /***��ϸ���***/	             
              char opkgno[3];                 /***ԭ�����ͺ�***/	               
              char owtday[8];                 /***ԭ��ί������***/	             
              char opackid[8];                /***ԭ�����***/	             
              char zcnode;                    /***����ڵ�����***/	             
              char zcday[8];                  /***��������***/	             
              char zcid[8];                   /***�����***/	             
              char packstat[STAT_NMLEN];      /***������״̬***/	             
              char resp[8];                   /***���Ĵ�����***/	             
              char recvno[6];                 /***���ղ���Ա��***/	             
              char recvwssrno[6];             /***������ˮ��***/	             
              char stat[STAT_NMLEN];          /***����״̬***/	             
              char prtflag[3];                /***��ӡ��־***/	             
      }NOTPAY_MESG_QUTOS_BASE;                                       
/*****���ص���Ҫ�ı�׼�ֶ�********/                                  
                                                                         
/*****״̬����*******/   
/**��״̬ȷ�����Ժ�,�������״̬�����Ƶ�������ѯ��ȥ***/
char RET_STAT[STAT_NUM1][STAT_FLAG][STAT_NMLEN1]={
	                     "00" ,"�ɹ�",              
	                     "01" ,"�ʺŲ���",          
	                     "02" ,"�ʺŻ�������",      
	                     "03" ,"�˻�����֧��",  
	                     "10" ,"�˻��������",      
	                     "11" ,"�ʺ�״̬����",      
	                     "20" ,"ҵ���ѳ���",        
	                     "90" ,"��������",          
	                     "\0"          
	                       };
char TRAN_STAT[STAT_NUM][STAT_FLAG][STAT_NMLEN]={
	                     "0" ,"��Ч״̬",                      
	                     "1" ,"����¼��",                         
	                     "2" ,"���˸��˹���",            
	                     "3" ,"���˸��˼���",            
	                     "4" ,"���˷���",                      
	                     "5" ,"���˾ܾ�",                      
	                     "6" ,"��������",                      
	                     "7" ,"�����Ŷ�",                  
	                     "8" ,"���˳����ȴ�",              
	                     "9" ,"���˳���",                  
	                     "A" ,"�����˻�ȴ�",              
	                     "B" ,"�����˻�",                  
	                     "G" ,"�����쳣����",              
	                     "H" ,"��ʱ���ղ�����",           
	                     "I" ,"��ʽ������������",        
	                     "J" ,"ȫ��Ѻ��",                  
	                     "K" ,"�ط�Ѻ��",                  
	                     "L" ,"ȫ��Ѻ�͵ط�Ѻ����",        
	                     "M" ,"��Ϣ�����������Ĺ���",    
	                     "N" ,"��Ϣ����֧�й���",        
	                     "O" ,"�����˻صȴ�",              
	                     "P" ,"�����˻�",                  
	                     "Q" ,"����ͬ��Ӧ����δ����",      
	                     "R" ,"����ͬ��Ӧ�����ѷ���",      
	                     "S" ,"������ͬ��Ӧ����δ����",    
	                     "T" ,"������ͬ��Ӧ�����ѷ���",    
	                     "Z" ,"�����쳣����",              
	                     "\0"
	                      };


char LOCALDEF_STAT[STAT_NUM1][STAT_FLAG][STAT_NMLEN1]={
	                       "0" ,"��Ч",       
	                       "1" ,"�������",   
	                       "2" ,"�����",     
	                       "3" ,"�ѷ���",     
	                       "4" ,"������Ӧ��", 
	                       "5" ,"�ѽ���",     
	                       "6" ,"��Ӧ��",     
	                       "\0"               
	                      };

char CENBANK_STAT[STAT_NUM1][STAT_FLAG][STAT_NMLEN1]={
    	                       "01" ,"�Ѿܾ����δͨ��",        
                               "02" ,"�ѷ��ͼ��ͨ��",          
                               "03" ,"������",                   
                               "04" ,"���Ŷ�",                   
                               "05" ,"������",                   
                               "06" ,"�ѳ���",                   
                               "10" ,"�ѻ�ִ",                   
                               "11" ,"�Ѿܸ�",                   
                               "12" ,"�ѳ���",                   
                               "13" ,"��ֹ��",                   
                               "14" ,"�ѳ�ʱ",                   
	                           "\0"
	                      };
/*****״̬����********/                                          
                            
#endif                                                           
