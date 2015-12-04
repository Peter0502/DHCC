#ifndef __LV_ADD_H__
#define __LV_ADD_H__
/*************һЩ������ṹ����************/    
/**���ҵ��-----(�޸�����)**/
/**ί���տ�(����)ҵ��**/
typedef struct{
	char wtpzday[8];	/***ί��ƾ֤����****/	
	char wtpzno [8];	/***ί��ƾ֤����****/	
	char pztype [2];	/***ƾ֤�����	****/	
}WTSK_ADD_AREA;
/**���ճи�(����)ҵ��**/
typedef struct{
	char tspzday[8 ];	/***����ƾ֤���� ***/
	char tspzno [8 ];	/***����ƾ֤���� ***/
	char pcamt  [15];	/***�⳥����	 ***/
	char jfamt  [15];	/***�ܸ����	 ***/
	char oamt   [15];	/***ԭ�н��	 ***/
	char zfamt  [15];	/***֧�����	 ***/
	char dfamt  [15];	/***�ึ���	 ***/
}LVTSCF_ADD_AREA;
/**�����ʽ���ǻ���ҵ��----ͬ(�����ʽ��ǻ���ҵ��)**/
typedef struct{
	char infono  [8 ];	/**��Ϣ��ˮ��      **/
	char amt     [18];	/**��ϸ���ܽ��    **/
	char upgkno  [10];	/**�ϱ��������    **/
	char downgkno[10];      /**���չ������    **/
	char yslevel [1 ];	/**Ԥ�㼶��        **/
	char adjflag [1 ];	/**�����ڱ�־      **/
	char ystype  [1 ];      /**Ԥ������        **/
	char rptday  [8 ];	/**��������        **/
	char rptno   [3 ];	/**�������        **/
	char dtlcnt  [3 ];	/**��ϸ����,�̶�Ϊ1**/
	char orgtype [10];	/**���ջ��ش���    **/
	char ysacno  [20];	/**Ԥ���Ŀ����    **/
	char dtlamt  [18];	/**������          **/
}GKDJ_ADD_AREA;
#define GKJJ_ADD_AREA GKDJ_ADD_AREA
/**���ڴ���ҵ��-----(�޸�����)**/
/**�����˻�ҵ��-----(�޸�����)**/
/**���ڽ��ҵ��-----(�޸�����)**/
/**�������ҵ��-----(�޸�����)**/
/**��������ҵ��-----(�޸�����)**/
/**һ��ͨ����Ϣҵ��**/	
typedef struct{
	char title  [60]; 	/**����         **/
	char content[255];	/**��������	**/
	char fjname [60]; 	/**�����ļ���   **/
	char fjlen  [8 ];  	/**��������	**/
}TYXX_ADD_AREA;  
/**��˰----(�޸�����)**/
/**ʵʱ��˰������������----ͬ(������˰)**/
typedef struct{
	char orgno  [11];	/**���ջ��ش���	**/
	char wtday  [8 ];	/**ί������	**/
	char wssrno [8 ];       /**������ˮ	**/
	char ksday  [8 ];	/**��˰����	**/
	char spno   [18];	/**˰Ʊ����	**/
	char filler1[60];	/**��ע1     	**/
	char filler2[60];	/**��ע2     	**/
	char filler3[60];	/**��ע3     	**/
}SSKS_ADD_AREA;
#define PLKS_ADD_AREA SSKS_ADD_AREA
/****��˰���븽����������********/
typedef struct{
	char payunit[60];	/**���ѵ�λ  **/
	char payterm[16];	/**ί������  **/
	char paytype[2];	/**��������  **/
}FSSR_ADD_AREA;
/****ֱ��֧��������������********/
typedef struct{
	char gkno    [10];	/**�������		**/
	char agacgtno[32];	/**�������ʺ�		**/
	char agbrno  [12];	/**�������д���		**/
	char ystype  [1 ];	/**Ԥ������		**/
	char adjflag [1 ];	/**�����ڱ�־		**/
	char zeroamt [18];	/**�������ܷ�����	**/
	char hdamt   [18];	/**С���ֽ��ҷ�����	**/
	char vocnum  [8 ];	/**ƾ֤���		**/
	char vocday  [8 ];	/**ƾ֤����		**/
	char dtlcnt  [3 ];	/**��ϸ����,�涨Ϊ1	**/
	char ysorgno [15];	/**Ԥ�㵥λ����		**/
	char gnacno  [20];	/**���ܷ����Ŀ����	**/
	char jjacno  [20];	/**���÷����Ŀ����	**/
	char acttype [1 ];	/**�ʻ�����		**/
	char dtlamt  [18];	/**��ϸ���		**/
}ZJZF_ADD_AREA;
#define SQZF_ADD_AREA ZJZF_ADD_AREA
#define SQTH_ADD_AREA ZJZF_ADD_AREA
#define ZJTH_ADD_AREA ZJZF_ADD_AREA
/****ͨ��ͨ�Ҹ�����������********/
typedef struct{
	char txamt      [15];	/**���׽��	 **/
	char agentfee   [15];	/**������������  **/
	char payacttype [1 ];	/**�������ʺ�����**/
	char cashacttype[1 ];	/**�տ����ʺ�����**/
	char pswdtype   [1 ];	/**��������	 **/
	char pswdmethod [2 ];	/**������֤���㷨**/
	char pswdcode   [512];	/**������֤��	 **/
}TCTD_ADD_AREA;


/****֧Ʊ����������������********/
typedef struct{
	char cpday      [8];	/**��Ʊ����	    ****/
	char cpno       [20];	/**��Ʊ����	    ****/
	char payno      [12];	/**�����к�	    ****/
	char cpactno    [32];	/**��Ʊ���ʺ�	    ****/
	char cashactno  [32];	/**�տ����ʺ�	    ****/
	char amt        [15];	/**���		    ****/
	char use        [60];	/**��;		    ****/
	char bscnt      [2];	/**��������,�̶�Ϊ0 ****/
	char bschecktype[2];	/**У��ģʽ 	    ****/
	char bscheckpswd[512];/**У������ 	    ****/
	char bsbitmaplen[8];  /**ͼ�����ݳ���,Ϊ0 ****/
}ZPJL_ADD_AREA;


/*������*/
typedef struct{
    /*ͨ����Ϣҵ��Ŀǰû�й̶��ĸ����������ʽ*/
    char    body;
}TYXXYW_ADD;

/*����/��ҵ��*/
typedef struct{
    char    unit     [60]; /*�ո���λ*/
    char    belongcyc[16]; /*��������*/
    char    sftype   [2];  /*�ո�����*/
}SFYW_ADD;


/**֧ƱȦ������ҵ��**/
typedef struct{
	char cpday      [8 ];  /**��Ʊ����	   **/
	char cpno       [12];  /**֧Ʊ����	   **/
	char payno      [12];  /**�������к�	   **/
	char cpactno    [32];  /**��Ʊ���˺�	   **/
	char cashactno  [32];  /**�տ����˺�	   **/
	char amt        [15];  /**���	           **/
	char use        [60];  /**��;	           **/
	char bscnt      [2 ];  /**��������	   **/
	char bschecktype[2 ];  /**У��ģʽ	   **/
	char bscheckpswd[512]; /**У������	   **/      
	char bsbitmaplen[8 ];  /**Ʊ��ͼ�����ݳ��� **/
}QCSQ_ADD_AREA;


/**֧ƱȦ���Ӧ**/
typedef struct{
	char hytype [2 ];  /**��Ӧ��ʶ  **/
	char cpday  [8 ];  /**��Ʊ����  **/
	char zpactno[12];  /**֧Ʊ����  **/
	char payno  [12];  /**�������к�**/
	char cpactno[32];  /**��Ʊ���˺�**/
	char amt    [15];  /**���      **/
}QCYD_ADD_AREA;


/*�ͻ��ʻ���ѯ*/
typedef struct{
    char    acttype     ;  /*�ʺ�����*/
    char    actno  [32] ;  /*�ʺ�*/
    char    pswdtp [2] ;  /*��������*/
    char    pswdlen[8]   ;  /*���볤��*/
    char    passwd [8];  /*����*/
    char    qtype       ;  /*��ѯ����*/
    char    reason [60] ;  /*����*/
}ZHCX_ADD;

/*�ͻ��ʻ���ѯ* �����ڸ���������������16λ�� add by wangyongwei20070920*/
typedef struct{
    char    acttype     ;  /*�ʺ�����*/
    char    actno  [32] ;  /*�ʺ�*/
    char    pswdtp [2] ;  /*��������*/
    char    pswdlen[8]   ;  /*���볤��*/
    char    passwd [16];  /*����*/
    char    qtype       ;  /*��ѯ����*/
    char    reason [60] ;  /*����*/
}ZHCX_ADD_PWD16;


/*�ͻ��ʻ���ѯӦ��*/
typedef struct{
    /*char    oqtype       ; ԭ��ѯ����*/
    /*char    addlength[8] ; �������ݳ���*/
    /*char    actno    [32]; �ʺ�*/
    /*char    name     [60]; ����*/
    /*�������ݣ��䳤*/
    
    /***change by liuyue 20060919******/
    char opackday	[8];	/*ԭ����������*/
    char opackid	[8];	/*ԭ���������*/
    char owtday		[8];	/*ԭί������  */
    char oorderno	[8];	/*ԭ��Ϣ���  */
    char actno		[32];	/*�ʺ�	      */
    char qrytype	    ;	/*ԭ��ѯ����  */
    char resptype	    ;	/*Ӧ���ʶ    */
    char otxamt		[18];	/*��ǰ���    */
    char actStat	[2] ;	/*��ǰ�ʻ�״̬*/
    char content	[60] ;	/*��ǰ�ʻ�״̬*/
}ZHCXYD_ADD;

/**֤���ѯ����**/
typedef struct{
	char content[60];  		/**����    	**/
	char jslen[8];   		/**֤�鳤��	**/
	/****NEWYX2****/
	char jsdaa[64];   		/**֤������	**/
}ZSSQ_ADD_AREA;

/**֤���ѯӦ��**/
typedef struct{
	char result[1 ];  	/**�����	**/
	char brf   [60];  	/**��ע	    	**/   
}ZSYD_ADD_AREA;

/****�ɷѸ�����������********/
typedef struct{
	char paywssrno[20];	/**�շѵ�λ��ˮ��**/
	char payterm  [16];	/**�����ڼ�	***/
	char jftype   [1 ];	/**�ɷ�����	***/
	char filler   [60];	/**�շѸ���	***/
}JF_ADD_AREA;
/****�����ʽ��ծ�Ҹ����Ǹ�����������********/
typedef struct{
	char infono  [8 ];	/**��Ϣ��ˮ��       **/
	char amt     [18];	/**��ϸ���ܽ��     **/
	char upgkno  [10];	/**�ϱ��������     **/
	char downgkno[10];	/**���չ������     **/
	char rptday  [8 ];	/**��������         **/
	char rptno   [3 ];	/**�������         **/
	char dtlcnt  [3 ];	/**��ϸ����,�̶�Ϊ1 **/
	char orgtype [12];	/**���д���         **/
	char bjtype  [12];	/**�������         **/
	char bjamt   [18];	/**������         **/
	char lxtype  [12];	/**��Ϣ����         **/
	char lxamt   [18];	/**��Ϣ���         **/
}GZDJ_ADD_AREA;
#define GZJJ_ADD_AREA GZDJ_ADD_AREA


/****֧Ʊ����������������********/
typedef struct{
	char cpday[8];		/**��Ʊ����****/
	char cpno[12];		/**��Ʊ����****/
	char payno[12];		/**�����к�****/
	char cpactno[32];	/**��Ʊ���ʺ�**/
	char cashactno[32];	/**�տ����ʺ�**/
	char cpamt[15];		/**���***/
	char use[60];		/**��;***/
        char tsfkday[8];/**��ʾ��������**/ /*add by lwb 20150417 ϵͳ����ʱȱ����� */
	/*NEWYX2:û������ֶ�char tsfkday[8];**��ʾ��������**/
}ZPJL1_ADD_AREA;
typedef struct{
   char cpday[8];/**��Ʊ����****/
   char cptype[3];/**֧Ʊ����***/
   char cpno[12];/**��Ʊ����****/
   char payno[12];/**�����к�****/
   char cpactno[32];/**��Ʊ���ʺ�**/
   char cashactno[32];     /**�տ����ʺ�**/
   char cpamt[15];/**���***/
   char use[60];/**��;***/
   char tsfkday[8];/**��ʾ��������**/
}ZPJL11_ADD_AREA;

typedef struct{
 	char bscnt[2];/**��������,�̶�Ϊ0 ****/
 	char bslist[10][60];    /**�������б�**/
}ZPJL2_ADD_AREA;
typedef struct{
 	char bscheckpswd[512];/**У������ ****/
 	char bsbitmaplen[8];/**ͼ�����ݳ���,Ϊ0 ****/
 	char bsbitmaplen2[8];/**ͼ�����ݳ���,Ϊ0 ****/
}ZPJL3_ADD_AREA;
/****ͨ�ý��������������� HSYX BEGIN ********/
typedef struct{
        char cpday[8];          /**��Ʊ����****/
        char cpno[20];          /**��Ʊ����****/
        char payno[12];         /**�����к�****/
        char dlpayno[12];       /**�������к�****/
        char cpamt[15];         /**��Ʊ���****/
        char use[60];           /**��;***/
}TYJL1_ADD_AREA;
#define TYJL2_ADD_AREA ZPJL2_ADD_AREA
typedef struct{
        /***char bschecktype[2];У������ ****/
        char bscheckpswd[512];/**У������ ****/
}TYJL3_ADD_AREA;
typedef struct{
        char ptype[2];          /**Ʊ������**/
        char tsday[8];          /**��ʾ��������**/
        char mac[20];           /**��Ѻ**/
        char stopday[8];        /**������**/
        char xyno[20];          /**�ж�Э����**/
        char htno[20];          /**���׺�ͬ����**/
        char cdday[8];          /**�ж�����**/
        char cdname[60];        /**�ж���**/
        char reqname[60];       /**������**/
        char reqactno[32];      /**�������ʺ�**/
        char paybrname[60];     /**�����˿�������**/
        char cashbrname[60];    /**�տ��˿�������**/
        char cpname[60];        /**��Ʊ��ȫ��    **/
        char cpactno[32];       /**��Ʊ���ʺ�    **/
        char paybrname2[60];    /**����������    **/
        char content[60];       /**����**/
}TYJL4_ADD_AREA;
typedef struct{
        char bsbitmaplen[8];/**ͼ�����ݳ���,Ϊ0 ****/
        char bsbitmaplen2[8];/**ͼ�����ݳ���,Ϊ0 ****/
}TYJL5_ADD_AREA;
/****ͨ�ý��������������� HSYX END ********/
/**��������ͨ�ý����������ļ����ݵĽṹ�� wangyongwei***/
typedef struct{
	   char cpday[8];          /**��Ʊ����****/
       char cpno[20];          /**��Ʊ����****/
       char payno[12];         /**�����к�****/
       char dlpayno[12];       /**�������к�****/
       char cpamt[15];         /**��Ʊ���****/
       char use[60];           /**��;***/
	   char ptype[2];          /**Ʊ������**/
       char tsday[8];          /**��ʾ��������**/
       char mac[20];           /**��Ѻ**/
       char stopday[8];        /**������**/
       char xyno[20];          /**�ж�Э����**/
       char htno[20];          /**���׺�ͬ����**/
       char cdday[8];          /**�ж�����**/
       char cdname[60];        /**�ж���**/
       char reqname[60];       /**������**/
       char reqactno[32];      /**�������ʺ�**/
       char paybrname[60];     /**�����˿�������**/
       char cashbrname[60];    /**�տ��˿�������**/
       char cpname[60];        /**��Ʊ��ȫ��    **/
       char cpactno[32];       /**��Ʊ���ʺ�    **/
       char paybrname2[60];    /**����������    **/
       char content[60];       /**����**/
}TYJL6_ADD_AREA;
/*�¿ͻ��ʻ���ѯӦ�� NEWYX 20070404*/
typedef struct{
    char opackday	[8];	/*ԭ����������*/
    char opackid	[8];	/*ԭ���������*/
    char owtday		[8];	/*ԭί������  */
    char oorderno	[8];	/*ԭ��Ϣ���  */
    char actno		[32];	/*�ʺ�	      */
    char qrytype	    ;	/*ԭ��ѯ����  */
    char resptype	    ;	/*Ӧ���ʶ    */
    char curcd      [3];    /* ���� */
    char otxamt		[15];	/*��ǰ���    */
    char actStat	[2]    ;	/*��ǰ�ʻ�״̬*/
    char content    [60];/* ���� */
}NEW_ZHCXYD_ADD;

typedef struct{
	char	actype[1];		/**�տ��ʺ�����**/
	char	pactype[1];		/**�����ʺ�����**/
	char	ct_ind[1];	/**�ֽ�ת�ʱ�־**/
}PAY_IN_CXTC_AREA;			/**ͨ��ͨ��**/
/****���´���ͨ�渽����������********/
typedef struct{
	char payacttype [1 ];	/**�������ʺ�����**/
	char cashacttype[1 ];	/**�տ����ʺ�����**/
	char ct_ind   [1 ];	/**ͨ��ҵ��ģʽ	 **/
}CXTC_ADD_AREA;

/****��ͨ�Ҹ�����������********/
typedef struct{
	char payacttype [1 ];	/**�������ʺ�����**/
	char cashacttype[1 ];	/**�տ����ʺ�����**/
	char ct_ind     [1 ];   /* ͨ��ҵ��ģʽ */
	char pswdmethod [2 ];	/**������֤���㷨**/
	char pswdlength [8 ];	/**��������	 **/
	char pswdcode   [512];	/**������֤��	 **/
}TD_ADD_AREA;

/* end END NEWYX by Liuhuafeng 20070404 */
/***END ͨ�ý�����������սṹ�� *****/

/**************������ṹ��������************/

#endif
