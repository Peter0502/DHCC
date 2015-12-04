#ifndef INCLUDE_LV_DEFINE
#define INCLUDE_LV_DEFINE
#include "lv_add.h"
#include "lv_parad_c.h"
#include "lv_param_c.h"
#include "lv_chk842_d_c.h" 
#include "lv_chk842_m_c.h"
#include "lv_chk850_d_c.h"
#include "lv_chk850_m_c.h"
#include "lv_chk851_d_c.h"
#include "lv_chk851_m_c.h"
#include "lv_chk852_m_c.h"
#include "lv_chk852_d_c.h"
#include "lv_lbctl_c.h"
#include "lv_wbctl_c.h"
#include "lv_pkgreg_c.h"
#include "lv_sysctl_c.h"
#include "lvpack.h"
#include "lvnet.h"
#include "exitlvpack.h"

/**��������**/
#ifndef TLRLENGTH
#define TLRLENGTH 6
#endif
#ifndef __FL__
#define __FL__ __FILE__,__LINE__
#endif

#ifndef LV_WAIT_QS
#define LV_WAIT_QS "41403"
#endif

#ifndef LVLZGD
#define LVLZGD "41202"
#endif

#ifndef BOOL
#define BOOL int
#define TRUE      1
#define FALSE     0
#endif
#ifndef DB_NOTFOUND
#define DB_NOTFOUND 100
#endif
#ifndef DB_OK
#define DB_OK 0
#endif

#ifndef PARM_LVGZ          /* BEPS  С�����ר���˺� */
#define PARM_LVGZ 8
#endif


#define LV_CZ_QSBRNO    "10001"    /**�������Ļ�����**/
#define LV_QSBRNO    "313175000011"    /**�������Ĳ����ߺ�**/
/**wyw **/
#ifndef PARM_CZ_LVLZGZ
#define PARM_CZ_LVLZGZ 10
#endif

/**ҵ�����ͱ��txnum�Ͱ����pkgno����**/
/**��ͨ����ҵ�������ҵ�����ͱ��**/
#define PKGNO_PTDJ    "001"   /*��ͨ���ǰ�*/
#define TXNUM_HD      "00100" /*���-----------��*/
#define TXNUM_WT      "00101" /*ί���տ��*/
#define TXNUM_TSCF    "00102" /*���ճи�����*/
#define TXNUM_GKDJ    "00103" /*�����ʽ���ǻ���*/
#define TXNUM_QTDJ    "00106" /*��������ҵ��---��*/
#define TXNUM_JS      "20001" /*��˰-----------��*/
#define TXNUM_GZDJ    "20005" /*��ծ����*/
#define TXNUM_FSSR    "20002" /*��˰����*/
#define TXNUM_JF      "30002" /*����*/
/**��ͨ���ҵ�������ҵ�����ͱ��**/
#define PKGNO_PTJJ    "002"   /*��ͨ��ǰ�*/
#define PKGNO_PTJJHZ  "008"   /*��ͨ��ǻ�ִ��*/
#define TXNUM_GKJJ    "00113" /*�����ʽ��ǻ���*/
#define TXNUM_QTJJ    "00119" /*�������ҵ��------��*/
#define TXNUM_ZJZF    "20103" /*����ֱ��֧��*/
#define TXNUM_SQZF    "20104" /*������Ȩ֧��*/
#define TXNUM_ZJTH    "20003" /*����ֱ��֧���˻�*/
#define TXNUM_SQTH    "20004" /*������Ȩ֧���˻�*/
#define TXNUM_ZPJL    "30102" /*֧Ʊ����ҵ��*/
#define TXNUM_GZJJ    "20105" /*��ծ���*/
#define TXNUM_TYJL    "30103" /*ͨ�ý���ҵ��*/
#define TXNUM_ZPJLHZ  "30104" /*֧Ʊ����ҵ���ִ*/
#define TXNUM_TYJLHZ  "30105" /*ͨ�ý���ҵ���ִ*/
/**�����˻�ҵ������ҵ�����ͱ��**/
#define PKGNO_DJTH    "007"    /*�����˻��*/
#define TXNUM_DJTH    "00108"  /*�����˻�ҵ��------��*/
/**���ڴ���ҵ��**/
#define PKGNO_DQDJ    "005"    /*���ڴ��ǰ�*/
#define TXNUM_DQDJ    "00104"  /*���ڴ���ҵ��------��*/

/**���ڽ��ҵ��**/
#define PKGNO_DQJJ    "006"    /*���ڽ�ǰ�*/
#define PKGNO_DQJJHZ  "011"    /*���ڽ�ǻ�ִ��*/
#define TXNUM_DQJJ    "00114"  /*���ڽ��ҵ��------��**/
#define TXNUM_PLKS    "20102"  /*������˰*/
/**ʵʱ����ҵ��**/
#define PKGNO_SSDJ    "003"    /*ʵʱ���ǰ�*/
#define PKGNO_SSDJHZ  "009"    /*ʵʱ���ǻ�ִ��*/
#define TXNUM_TC      "30001"  /*ͨ��ҵ��*/
/**ʵʱ���**/
#define PKGNO_SSJJ    "004"    /*ʵʱ��ǰ�*/
#define PKGNO_SSJJHZ  "010"    /*ʵʱ��ǻ�ִ��*/
#define TXNUM_SSKS    "20101"  /*ʵʱ��˰*/
#define TXNUM_TD      "30101"  /*ͨ��ҵ��*/
#define TXNUM_DGTD    "XXXXX"  /*NEWYX:ɾ�����Թ�ͨ��ҵ��*/
/**ͨ����Ϣ**/
#define PKGNO_TYXX    "012"    /*ͨ����Ϣ��*/
#define TXNUM_TYXX    "00500"  /*ͨ����Ϣҵ��*/
#define TXNUM_DF      "40501"  /*����ҵ��*/
#define TXNUM_DS      "40502"  /*����ҵ��*/
/**ʵʱ��Ϣ**/
#define PKGNO_SSXX    "013"    /*ʵʱ��Ϣ����*/
#define TXNUM_ZPQC    "30501"  /*֧ƱȦ��*/
#define TXNUM_ZPYD    "30504"  /*֧ƱȦ��Ӧ��*/
#define TXNUM_ZSCX    "30505"  /*֤���ѯ*/
#define TXNUM_ZSYD    "30506"  /*֤���ѯӦ��*/
#define TXNUM_ZHCX    "30502"  /*�ͻ��˻���ѯ*/
#define TXNUM_CXYD    "30503"  /*�ͻ��˻���ѯӦ��*/
/**��ִ״̬���˻�ԭ��**/
#define RET_SUCCESS    "00"    /*�ɹ�*/
#define RET_NOACTNO    "01"    /*�ʺŲ���*/
#define RET_NONAME     "02"    /*�ʺŻ�������*/
#define RET_NOMONEY    "03"    /*�˻�����֧��*/
#define RET_PSWDERR    "10"    /*�˻��������*/
#define RET_ACTCLSERR  "11"    /*�ʺ�״̬����*/
#define RET_HASCANCEL  "20"    /*ҵ���ѳ���*/
#define RET_OTHER      "90"    /*��������*/
/**ί���տ�ƾ֤����**/
#define WTVOC_CDHP     "01"    /*���гжһ�Ʊ*/
#define WTVOC_DQCD     "02"    /*���ڴ浥*/
#define WTVOC_PZGZ     "03"    /*ƾ֤ʽ��ծ*/
#define WTVOC_HQCZ     "04"    /*��ػ��ڴ���*/
#define WTVOC_OTHER    "99"    /*����*/
/**����״̬**/
#define STAT_INVALID        '0' /*��Ч״̬*/
#define STAT_WZLR           '1' /*����¼��*/
#define STAT_WZCHECK1       '2' /*���˸��ˣ����� --��Ч��20051130 by lhf*/
#define STAT_WZCHECK2       '3' /*���˸��ˣ�����*/
#define STAT_WZSEND         '4' /*���˷���*/
#define STAT_WZREJECT       '5' /*���˾ܾ�*/
#define STAT_WZSUCCESS      '6' /*��������*/
#define STAT_WZINQUEUE      '7' /*�����Ŷ�*/
#define STAT_WZCANCEL_WAIT  '8' /*���˳����ȴ�*/
#define STAT_WZCANCEL       '9' /*���˳���*/
#define STAT_WZBACK_WAIT    'A' /*�����˻ء�������ֹ���ȴ�*/
                                /**�˻�--��ͨ�����ڴ���
                                ** ����--ʵʱ��ǡ�����
                                ** ֹ��--��ͨ�����ڽ��
                                ****************************/
#define STAT_WZBACK         'B'  /*�����˻�*/
#define STAT_WZCZ           'C'  /*�����ѳ���*/
#define STAT_WZZF           'D'  /*������ֹ��*/
#define STAT_WZEXCEPT       'G'  /*�����쳣����*/

#define STAT_LZRECV1        'H'  /*��ʱ����,������*/
#define STAT_LZRECV2        'I'  /*��ʽ���գ���������*/
#define STAT_LZNMAC_ERR     'J'  /*ȫ��Ѻ�� -��֧��*/
#define STAT_LZLMAC_ERR     'K'  /*�ط�Ѻ�� -��֧��*/
#define STAT_LZAMAC_ERR     'L'  /*ȫ��Ѻ�͵ط�Ѻ���� -��֧��*/
#define STAT_LZGZ_QS        'M'  /*��Ϣ�������������Ĺ���*/
#define STAT_LZGZ_BR        'N'  /*��Ϣ������֧�й���*/
#define STAT_LZBACK_WAIT    'O'  /*�����˻صȴ�*/
#define STAT_LZBACK         'P'  /*�����˻�*/
#define STAT_YES_NOTSEND    'Q'  /*����ͬ��Ӧ����δ����*/
#define STAT_YES_SEND       'R'  /*����ͬ��Ӧ�����ѷ���*/
#define STAT_NO_NOTSEND     'S'  /*������ͬ��Ӧ����δ����*/
#define STAT_NO_SEND        'T'  /*������ͬ��Ӧ�����ѷ���*/
#define STAT_LZCZ           'U'  /*�����ѳ���*/
#define STAT_LZZF           'V'  /*������ֹ��*/
#define STAT_LZFK           'W'  /*�����Ѹ���*/
#define STAT_LZEXCEPT       'Z'  /*�����쳣����*/
/**�ո���־**/
#define SFFLAG_CASH         '0'  /*��*/
#define SFFLAG_PAY          '1'  /*��*/
/**��ѯ�ʺ�����**/
#define QACTTYPE_ACTNO      '0'  /*�����ʺ�*/
#define QACTTYPE_CRCARD     '1'  /*���ǿ�*/
#define QACTTYPE_CRECARD    '2'  /*���ÿ�*/
#define QACTTYPE_OTHER      '9'  /*����*/
/**��ѯ��������*/
#define QPSWDCD_ACTNO       '0'  /*�˻�����*/
#define QPSWDCD_CARD        '1'  /*������*/
#define QPSWDCD_TX          '3'  /*��������*/ 
#define QPSWDCD_QUERY       '4'  /*��ѯ����*/
#define QPSWDCD_OTHER       '9'  /*��������*/
/**�˻���ѯ����*/
#define QTYPE_AVBAL         '1'  /*��ѯ���*/
/**����ڵ�����**/
#define ZCNODE_NPC          '1'  /*NPC����*/
#define ZCNODE_CCPC         '2'  /*CCPC����*/
/**Ͻ�ڷַ���־**/
#define FFFLAG_NO           '0'  /*���ַ�*/
#define FFFLAG_YES          '1'  /*�ַ�*/
/**������־**/
#define BFFLAG_NORMAL       '0'  /*����*/
#define BFFLAG_BF           '1'  /*����*/
/**��״̬���Զ���״̬*/
#define PSTAT_INVALID       '0'  /*��Ч*/
#define PSTAT_PACKING       '1'  /*�������*/
#define PSTAT_PACKED        '2'  /*�����*/
#define PSTAT_SEND          '3'  /*�ѷ���*/
#define PSTAT_WRESP         '4'  /*������Ӧ��*/
#define PSTAT_RECEIVE       '5'  /*�ѽ���*/
#define PSTAT_LRESP         '6'  /*��Ӧ��*/
/**��״̬��������Ҫ��*/
#define PACKSTAT_REJECT     "01" /*�Ѿܾ�,���δͨ��*/
#define PACKSTAT_SEND       "02" /*�ѷ���,���ͨ��*/
#define PACKSTAT_SUCCESS    "03" /*������*/
#define PACKSTAT_INQUEUE    "04" /*���Ŷ�*/
#define PACKSTAT_QS         "05" /*������*/
#define PACKSTAT_CANCEL     "06" /*�ѳ���*/
#define PACKSTAT_RECEIPT    "10" /*�ѻ�ִ*/
#define PACKSTAT_REJECTPAY  "11" /*�Ѿܸ�*/
#define PACKSTAT_CZ         "12" /*�ѳ���*/
#define PACKSTAT_STOPPAY    "13" /*��ֹ��*/
#define PACKSTAT_TIMEOUT    "14" /*�ѳ�ʱ*/
/****��������״̬(��ʱƴ������)Add by SSH,2005.12.12****/
#define SERVSTAT_STOP		'0'	/****����ֹͣ***/
#define SERVSTAT_START		'1'	/****��������***/
/**�˻�ֹ����������**/
#define REQTYPE_SINGLE      '0'  /*����*/
#define REQTYPE_WHOLE       '1'  /*����*/
/**�˻�ֹ�������Ӧ������**/
#define RESP_CAN            '1'  /*����*/
#define RESP_CANNOT         '2'  /*������*/
/**��������**/
#define ZCTYPE_WORKING      '1'  /*�ռ�����*/
#define ZCTYPE_WORKEND      '2'  /*��������*/
/**�����־�������б�׼**/
#define CRDB_DR             '0'  /*��*/
#define CRDB_CR             '1'  /*��*/
/**������**/
#define OPCD_LR_QUERY       "11" /*¼��֮��ѯ*/
#define OPCD_LR             "10" /*¼��*/
#define OPCD_CHECK          "30" /*����*/
#define OPCD_SEND_QUERY     "41" /*����֮��ѯ*/
#define OPCD_SEND           "40" /*����*/
#define OPCD_RESP           "50" /*Ӧ��*/
#define OPCD_RECEIVE        "60" /*����*/
#define OPCD_INACT_QUERY    "72" /*���˴���֮��ѯ*/
#define OPCD_INACT_MODIFY   "71" /*���˴���֮�޸�*/
#define OPCD_INACT          "70" /*���˴���֮����*/
#define OPCD_REV_RECEIVE    "80" /*������*/
#define OPCD_REV_SEND       "90" /*������*/
/***********��ȡ�����ѱ�־****************************/
#define FEEFLAG_CASH    '0'  /**�ֽ�**/
#define FEEFLAG_CASHJJ  '1'  /**�ֽ�Ӽ�**/
#define FEEFLAG_ACT     '2'  /**ת��**/
#define FEEFLAG_ACTJJ   '3'  /**ת�ʼӼ�**/
#define FEEFLAG_NO      '4'  /**����**/
#define TX_SEND         0    /**���ͽ���**/
#define TX_RECEIVE      1    /**���ս���**/
/******�����c8���ؽ��****/
#define PCHK_NEW    0 /***�°�***/
#define PCHK_EXISTS 1 /***�Ѵ���**/
#define PCHK_UPDATE 2 /***�Ѹ���**/
/******��ӡ���Ʊ�־*********/
#define PRTFLG_INVALID		'0'	/**�޴˷�¼***/
#define PRTFLG_HASNOT		'1'	/**��¼δ��ӡ**/
#define PRTFLG_HAS			'2'	/**��¼�Ѵ�ӡ***/
#define PRTFLG_CANCEL1		'3'	/**����¼δ�򣬳��δ��**/
#define PRTFLG_CANCEL2		'4'	/**����¼δ�򣬳���Ѵ�**/
#define PRTFLG_CANCEL3		'5'	/**����¼�Ѵ򣬳��δ��**/
#define PRTFLG_CANCEL4		'6'	/**����¼�Ѵ򣬳���Ѵ�**/
/****��ӡ��¼�����������****/
#define MAX_FLCNT    3 /**���ֵֻ�������ã����ܸ���**/
/*****������Ͷ���****/
#define LV_ACTYPE 	"96"		/****С�����������****/
#define LV_FLGACTYPE    "95"		/****���ı�ʶ������****/
#define LV_ADDID_IND 	"94"       	/****С��������****/
#define LV_PACKID_IND 	"93"       	/****С��ƴ�����****/

/**���濪ʼ�������������Ͷ���**/
#define ATYPE_PAY_IN       "1000"  /*֧������������*/
#define ATYPE_PAY_OUT1     "1001"  /*֧������һ�������*/
#define ATYPE_PAY_OUT2     "1002"  /*֧�����׷�¼�����*/
#define ATYPE_PAY_OUT3     "1003"  /*֧�����������������*/
#define ATYPE_NOTPAY_IN    "2000"  /*��֧������������*/
#define ATYPE_NOTPAY_OUT   "2001"  /*��֧�����������*/
#define ATYPE_CMT_IN       "3000"  /*cmt������*/
#define ATYPE_CMT_OUT      "3001"  /*CMT�����*/
#define BASE_EXCHANGE_BLOCK    char type[4]; \
                               char length[4];

#define LV_RECV_BRNO "9999999"
#define LV_RECV_TLRNO "9999"

#define _lv_errmsg lv_setf(__LINE__, __FILE__); lv_errmsg
typedef struct{
	int errline;
	char errfm[128];
	int rtcode;
	char lv_fd123[1000];
	char brno[8];
	char tlrno[8];
	char tbsdy[8];
	char lv_date[9];
	char txamt[16];
	char aptype;
	char txno[2];
	char wssrno[7];
	char sqlbuf[512];
	char errmsg[300];
}t_lvca  ;
t_lvca lvca;
#define GETHEADTAG_DATE(pk, s, lvd)   {GETHEADTAG(pk, s, lvca.lv_date); lvd = apatoi(lvca.lv_date, 8);}
#define lv_pkg_batch_date(pk, s, lvd) {lv_pkg_batch(pk, s, lvca.lv_date); lvd = apatoi(lvca.lv_date, 8);}
typedef struct {/**������������**/
    BASE_EXCHANGE_BLOCK
}BASE_EXCHANGE_AREA;
#define PAY_IN_FIX_BLOCK \
    char    F_crdb   [1];/**�����־0��1��**/ \
    char    F_opcd   [2];/**��������    10��¼��֮¼��11��¼��֮��ѯ \
                           20���޸�֮�޸�21���޸�֮��ѯ30������ \
                           40������֮����41������֮��ѯ50��Ӧ�� \
                           60������70�����˴���֮���� \
                           71�����˴���֮�޸�72�����˴���֮��ѯ \
                           80--������ 90--������\
                         **/ \
    char    F_pkgno      [3 ]  ;/**Package���    **/ \
    char    F_orderno    [8 ]  ;/**֧���������   **/ \
    char    F_txnum      [5 ]  ;/**ҵ�����ͱ��   **/ \
    char    F_orbrno     [12]  ;/**�������к�     **/ \
    char    F_acbrno     [12]  ;/**�������к�     **/ \
    char    F_txamt      [15]  ;/**���׽��       **/ \
    char    F_payno      [12]  ;/**�����˿�����   **/ \
    char    F_payqsno    [12]  ;/**�������������� **/ \
    char    F_payactno   [32]  ;/**�������ʺ�     **/ \
    char    F_payname    [60]  ;/**����������     **/ \
    char    F_payaddress [60]  ;/**�����˵�ַ     **/ \
    char    F_userpswd   [8]   ;/**��������       **/ \
    char    F_cashqsno   [12]  ;/**�տ�����������**/ \
    char    F_cashno     [12]  ;/**�տ��˿������к�**/ \
    char    F_cashactno  [32]  ;/**�տ����ʺ�     **/ \
    char    F_cashname   [60]  ;/**�տ�������     **/ \
    char    F_cashaddress[60]  ;/**�տ��˵�ַ     **/ \
    char    F_ywtype     [12]  ;/**ҵ������       **/ \
    char    F_content    [60]  ;/**����           **/ \
    char    F_wtday      [8]   ;/**ί������       **/ \
    char    F_operlevel  [1]   ;/**���ȼ�         **/ \
    char    F_authno[TLRLENGTH];/**��Ȩ����Ա��   **/ \
    char    F_authpswd[8]      ;/**��Ȩ����Ա���� **/ \
    char    F_voctype [2]      ;/**ƾ֤����       **/ \
    char    F_vocnum  [12]     ;/**ƾ֤����       **/ \
    char    F_packday [8]      ;/**������,������  **/ \
    char    F_packid  [8]      ;/**��id,������    **/ \
    char    F_feeflag [1]      ;/**�����ѱ�־     **/ \
    char    F_feeamt  [15]     ;/**�����ѽ��     **/ \
    /**�����ֶι��ɱ�׼���벿�֣�����������潻�ײ�ͬ��ѡ���Ը�ֵ**/ \
    char    F_respday [8] ;/**�ڴ�Ӧ������  **/ \
    char    F_ptype   [2] ;/**Ʊ������      **/ \
    char    F_pdate   [8] ;/**Ʊ������      **/ \
    char    F_pnum    [8] ;/**Ʊ�ݺ���      **/ \
    char    F_ptxamt  [15];/**�⳥���      **/ \
    char    F_jtxamt  [15];/**�ܸ����      **/ \
    char    F_owtday  [8] ;/**ԭί������    **/ \
    char    F_otxnum  [5] ;/**ԭҵ����    **/ \
    char    F_oorderno[8] ;/**ԭ֧���������**/ \
    char    F_retcode [2] ;/**�˻�ԭ�����  **/ \
    char    F_rcpstat [2] ;/**ҵ���ִ״̬  **/ \
    char    F_resp1   [8] ;/**MbfeӦ����    **/ \
    char    F_resp2   [8] ;/**CcpcӦ����    **/ \
    char    F_packend [1] ;/**��������־    **/ \
    char    F_lvsts   [1] ;/**����״̬      **/ \
    char    F_lw_ind  [1] ;/**������ʶ	    **/ \
    char    F_dfxyno  [60];/**�෽Э���    **/ \
    char    F_o_or_br_no [12];/**ԭ�������к�  **/ \
    char    F_o_ac_br_no [12];/**ԭ�������к�  **/
typedef struct {
    BASE_EXCHANGE_BLOCK
    PAY_IN_FIX_BLOCK
}PAY_IN_FIX_AREA;
typedef struct {/**֧�����������������**/
    BASE_EXCHANGE_BLOCK
    /**���벿��**/
    PAY_IN_FIX_BLOCK
    char    F_actype  [1] ;/**ͨ��ͨ���˺�����***/ 
    char    F_pswdtype[1] ;/**ͨ��ͨ����������**/ 
    char    F_pswd    [8] ;/**ͨ��ͨ������****/
    
    char    F_payunit[60] ;/**����(˰)��λ**/
    char    F_payterm[16] ;/**����(˰)�ڼ�**/
    char    F_paytype[2]  ;/**����(˰)����**/
    /**����ȡ������**/
    char    T_pkgno      [3]    ;/**Package���     **/
    char    T_inday      [8]    ;/**����ϵͳ����    **/
    char    T_jzday      [8]    ;/**��������        **/
    char    T_wtday      [8]    ;/**ί������        **/
    char    T_packday    [8]    ;/**�������        **/
    char    T_orderno    [8]    ;/**֧���������    **/
    char    T_ywdtlid    [8]    ;/**ҵ����ţ�����ҵ����**/
    char    T_txtime     [14]   ;/**����ʱ��        **/
    char    T_txnum      [5]    ;/**ҵ�����ͱ��    **/
    char    T_orbrno     [12]   ;/**�������к�      **/
    char    T_payqsactno [12]   ;/**�����������к�  **/
    char    T_sendco     [4]     ;/**��������       **/
    char    T_acbrno     [12]   ;/**�������к�      **/
    char    T_cashqsactno[12]   ;/**�����������к�  **/
    char    T_receco     [4]    ;/**�ձ�����        **/
    char    T_txamt      [15]   ;/**���׽��        **/
    char    T_payno      [12]   ;/**�����˿�����    **/
    char    T_payactno   [32]   ;/**�������ʺ�      **/
    char    T_payname    [60]   ;/**����������      **/
    char    T_payaddress [60]   ;/**�����˵�ַ      **/
    char    T_cashno     [12]   ;/**�տ��˿�����    **/
    char    T_cashactno  [32]   ;/**�տ����ʺ�      **/
    char    T_cashname   [60]   ;/**�տ�������      **/
    char    T_cashaddress[60]   ;/**�տ��˵�ַ      **/
    char    T_ywtype     [12]   ;/**��ԭ��ҵ������  **/
    char    T_content    [60]   ;/**����            **/
    char    T_respday    [8]    ;/**Ӧ������        **/
    char    T_rcpstat    [2]    ;/**ҵ���ִ״̬    **/
    char    T_resp1      [8]    ;/**MbfeӦ����      **/
    char    T_resp2      [8]    ;/**CcpcӦ����      **/
    char    T_voctype    [2]    ;/**ƾ֤����        **/
    char    T_vocnum     [12]   ;/**ƾ֤����        **/
    char    T_ptype      [2]    ;/**Ʊ������        **/
    char    T_pdate      [8]    ;/**Ʊ������        **/
    char    T_pnum       [8]    ;/**Ʊ�ݺ���        **/
    char    T_ptxamt     [15]   ;/**�⳥���        **/
    char    T_jtxamt     [15]   ;/**�ܸ����        **/
    char    T_actype     [1]    ;/**ͨ��ͨ���ʺ�����**/
    char    T_pswdtype   [1]    ;/**ͨ��ͨ����������**/
    char    T_pswdcode   [128]  ;/**ͨ��ͨ�����뽻����**/
    char    T_payunit    [60]   ;/**����(˰)��λ    **/
    char    T_payterm    [16]   ;/**����(˰)�ڼ�    **/
    char    T_paytype    [2]    ;/**����(˰)����    **/
    char    T_brno       [7]    ;/**���׻���        **/
    char    T_inputno[TLRLENGTH];/**¼�����Ա    **/
    char    T_checkno[TLRLENGTH];/**���˲���Ա    **/
    char    T_authno [TLRLENGTH];/**��Ȩ����Ա    **/
    char    T_sendno [TLRLENGTH];/**���Ͳ���Ա    **/
    char    T_respno [TLRLENGTH];/**Ӧ�����Ա    **/
    char    T_recvno [TLRLENGTH];/**���ղ���Ա��  **/
    char    T_dealno [TLRLENGTH];/**���˴������Ա��**/
    char    T_inwssrno   [6]    ;/**¼����ˮ��    **/
    char    T_checkwssrno[6]    ;/**������ˮ��    **/
    char    T_sendwssrno [6]    ;/**������ˮ��    **/
    char    T_respwssrno [6]    ;/**Ӧ����ˮ��    **/
    char    T_recvwssrno [6]    ;/**���ղ�����ˮ��**/
    char    T_dealwssrno [6]    ;/**���˴�����ˮ��**/
    char    T_stat       [1]    ;/**����״̬      **/
    char    T_begstat    [1]    ;/**��ʼ����״̬  **/
    char    T_operlevel  [1]    ;/**���ȼ�        **/
    char    T_feeflag    [1]    ;/**�����ѱ�־    **/
    char    T_feeamt     [15]   ;/**�����ѽ��    **/
    char    T_checkflag  [1]    ;/**���˱�־      **/
    char    T_brprtflag  [64]   ;/**֧�д�ӡ��־  **/
    char    T_qsprtflag  [64]   ;/**�������Ĵ�ӡ��־**/
    char    T_packid     [8]    ;/**������        **/
    char    T_owtday     [8]    ;/**ԭί������    **/
    char    T_otxnum     [5]    ;/**ԭҵ����    **/
    char    T_retcode    [2]    ;/**�˻�ԭ�����  **/
    char    T_ostat      [1]    ;/**ԭ����״̬    **/
    char    T_oorderno   [8]    ;/**ԭ֧���������**/
    char    T_addid      [8]    ;/**����������id  **/
    char    T_lw_ind     [1]    ;/**������ʶ      **/
    char    T_filler     [60]   ;/**��ע          **/
    char    T_dfxyno     [60]   ;/**�෽Э���    **/
    char    T_o_or_br_no [12];
    char    T_o_ac_br_no [12];
    char    T_o_pack_date[8];
    char    T_o_packid   [8];
    /**�Զ���ȡ����**/
    char    A_tbsdy      [8]    ;/**��ǰ����**/
    char    A_wssrno     [6]    ;/**��ǰ����Ա��ˮ��**/
    char    A_stat       [1]    ;/**״̬,����ֶζ�������Ч��
                                �������ʱ��ֵ����ֵΪ��������**/
    char    A_sendco     [4]    ;/**��������**/
    char    A_payqsactno[12]    ;/**�����������к�**/
    char    A_receco     [4]    ;/**�ձ�����**/
    char    A_cashqsactno[12]   ;/**����������**/        
    char    A_acno       [7]    ;/**�ͻ���Ŀ**/
    char    A_avbal1     [15]   ;/**����ǰ���**/
    char    A_avbal2     [15]   ;/**���˺����**/
    char    A_oldrespday [8]    ;/**����ԭ�����ڴ�Ӧ������**/
    char    A_brno       [7]	;/**������������Ļ�����****/
    char    A_acttype    [1]	;/**�ʺ�����,Ϊ�˽�ʡ���ʼ��ʱ��,��check3****/
}PAY_IN_AREA;            
typedef struct{
    BASE_EXCHANGE_BLOCK
    /**�̶�����**/
    char txname[20]            ;/**��������            **/           
    char txday[8]              ;/**��������,С������   **/
    char brno[7]               ;/**���׻���            **/
    char orderno[8]            ;/**֧���������        **/
    char txtype[12]            ;/**ҵ������,Txnum������**/
    char orbrno[12]            ;/**�������к�            **/
    char payno[12]             ;/**�����˿�����        **/
    char acbrno[12]            ;/**������                **/
    char cashno[12]            ;/**�տ��˿�����        **/
    char txamt[15]             ;/**���׽��            **/
    char payactno[32]          ;/**�������ʺ�            **/
    char payaddress[60]        ;/**�����˵�ַ            **/
    char payname[60]           ;/**�����˻���            **/
    char cashactno[32]         ;/**�տ����ʺ�            **/
    char cashname[60]          ;/**�տ��˻���            **/
    char cashaddress[60]       ;/**�տ��˵�ַ            **/
    char ywtype[12]            ;/**ҵ������,���Ҫ�Ѵ���ת��Ϊ����**/
    char content[60]           ;/**����    **/
    char tlrno[TLRLENGTH]      ;/**����Ա    **/
    char authno[TLRLENGTH]     ;/**��ȨԱ    **/
    char wssrno[TLRLENGTH]     ;/**��ˮ��    **/
    /**���䲿��**/
    char owtday[8]   ;/**ԭί������    �˻���        **/
    char otxtype[12] ;/**ԭҵ������    Txnum������   **/
    char oorderno[8] ;/**ԭ֧���������              **/
    char retdtl[12]  ;/**�˻�ԭ��    Retcode������ **/
    char feeflag[1]  ;/**�����ѱ�־                  **/
    char feeamt[15]  ;/**�����ѽ��                  **/
    char ptype[2]    ;/**Ʊ������    ί��          **/
    char pdate[8]    ;/**Ʊ������                  **/
    char pnum[8]     ;/**Ʊ�ݺ���                  **/
    char ptxamt[15]  ;/**�⳥���                  **/
    char jtxamt[15]  ;/**�ܸ����                  **/
    char cashqsactno[12]; /* �տ��������к� */
}PAY_OUT1_AREA;
typedef struct{            /**��¼�����***/
    BASE_EXCHANGE_BLOCK
    char dracno1[12]        ;/**�跽��Ŀ**/
    char cracno1[12]        ;/**������Ŀ**/
    char dracno2[12]        ;/**�跽��Ŀ**/
    char cracno2[12]        ;/**������Ŀ**/
    char dracno3[12]        ;/**�跽��Ŀ**/
    char cracno3[12]        ;/**������Ŀ**/        
}PAY_OUT2_AREA;
typedef struct{            /**�����������***/
    BASE_EXCHANGE_BLOCK
    char feeamt[15]            ;/**�����ѽ��***/
    char dracno[12]            ;/**�跽��Ŀ****/
    char cracno[12]            ;/**������Ŀ****/
}PAY_OUT3_AREA;
/****���ڴ�����ϸ����*******/
typedef struct{
	char orderno[8];		/**֧���������**/
	char ywdtlid[8];		/**ҵ�����**/
	char acbrno[12];		/**�տ����к�**/
	char cashno[12];		/**�տ��˿������к�**/
	char cashactno[32];		/**�տ����ʺ�**/
	char cashname[60];		/**�տ�������**/
	char cashaddress[60];	/**�տ��˵�ַ**/
	char txamt[15];			/**���׽��**/
	char content[60];		/**����**/
	char addlen[8];			/**�������ݳ���**/
	char zero;				/**��β0****/
}DQDJ_DTL_AREA;

/****���ڽ����ϸ����*******/
typedef struct{
	char orderno[8];		/**֧���������**/
	char ywdtlid[8];		/**ҵ�����**/
	char acbrno[12];		/**�������к�**/
	char payno[12];			/**�����˿������к�**/
	char payactno[32];		/**�������ʺ�**/
	char payname[60];		/**����������**/
	char payaddress[60];	/**�����˵�ַ**/
	char txamt[15];			/**���׽��**/
	char protno[60];		/**�෽Э���**/
	char content[60];		/**����**/
	char addlen[8];			/**�������ݳ���**/
	char zero;				/**��β0****/
}DQJJ_DTL_AREA;

/**��������**/
/**����ǰ̨tis����pay_in��ֵ**/
int lv_tis_to_pay_in(PAY_IN_AREA *wp_pay_in);
/**����pay_in�������ݲ���һ���µ�֧����¼*/
int lv_newrec_from_pay_in(PAY_IN_AREA *wp_pay_in);
/**����pay_in����������������������**/
int lv_reset_pay_in(PAY_IN_AREA *wp_pay_in);
/**����pay_in����������ȡһ��֧����¼**/
int lv_fetchrec_to_pay_in(PAY_IN_AREA *wp_pay_in);
/**����pay_in�е����ݸ������ݿ��,��������ϸ**/
int lv_updrec_from_pay_in(PAY_IN_AREA *wp_pay_in,int insflg);
/**ͨ�ü��**
int lv_pay_in_check0(PAY_IN_AREA *wp_pay_in);
*/
/****�кż��******************************
int lv_pay_in_check1(PAY_IN_AREA *wp_pay_in);
*/
/****�����ʺż��******************************
int lv_pay_in_check2(PAY_IN_AREA *wp_pay_in);
*/
/****�տ��ʺż��*****************************
int lv_pay_in_check3(PAY_IN_AREA *wp_pay_in);
*/
/****Ҫ�غϷ��Լ��*************************
int lv_pay_in_check4(PAY_IN_AREA *wp_pay_in);
*/
/****��Ȩ��ϵ���**************************
int lv_pay_in_check5(PAY_IN_AREA *wp_pay_in);
*/
/****Ҫ��һ���Լ��**************************
int lv_pay_in_check6(PAY_IN_AREA *wp_pay_in);
*/
/****��¼�����Լ��**************************
int lv_pay_in_check7(PAY_IN_AREA *wp_pay_in);
*/
/****�����******************************
int lv_pay_in_check8(PAY_IN_AREA *wp_pay_in,struct wd_lv_wbctl_area *wp_lv_wbctl,struct wd_lv_lbctl_area *wp_lv_lbctl);
*/
/**����pay_in���ݲ���һ���µĻ�ִ��¼*/
/***ֻ��������򣬱����ݿ��Ե���newrec����������***
int lv_hz_from_pay_in(PAY_IN_AREA *wp_pay_in,PAY_IN_AREA *wp_pay_in_hz);
*/
/**����pay_in���ݲ���һ���µ��˻��¼*/
/***ֻ��������򣬱����ݿ��Ե���newrec����������***
int lv_th_from_pay_in(PAY_IN_AREA *wp_pay_in,PAY_IN_AREA *wp_pay_in_th);
*/
/****������������ƴһ�������****
int lv_set_pay_out1(PAY_IN_AREA  *wp_pay_in,PAY_OUT1_AREA *wp_pay_out1);
int lv_set_pay_out2(PAY_IN_AREA  *wp_pay_in,PAY_OUT2_AREA *wp_pay_out2);
*/
/****����������������ƴ�����������**
int lv_set_pay_out3(PAY_IN_AREA  *wp_pay_in,PAY_OUT3_AREA *wp_pay_out3);
*/
/****һ�������****
int lv_pay_putmsg1(PAY_OUT1_AREA *wp_pay_out1,char flag);
*/
/****��¼�����****
int lv_pay_putmsg2(PAY_OUT2_AREA *wp_pay_out2,char flag);
*/
/****�����������****
int lv_pay_putmsg3(PAY_OUT2_AREA *wp_pay_out3,char flag);
*/
/****���÷���״̬****
int lv_setprg(char *prgname,char prgstat,int pid );
*/
/****�õ�����״̬****
int lv_getprgstat(char *prgname);
*/
/****�õ�����pid***
int lv_getprgpid(char *prgname);
*/


/*
char *lv_getday();**�õ�С��ϵͳ����**/    
/**�ж�һ�ʽ����Ƿ��ͽ��׻��ǽ��ս���**
int lv_get_orient(PAY_IN_AREA *wp_pay_in);
*/
/****�õ�֧���������ݽ��������Զ���Ϣ****
int lv_pay_in_autoinfo(PAY_IN_AREA *wp_pay_in);
*/
/****�õ���һ����id,�뽻�׻����޹�****
int lv_get_packid();
*/
/****�õ���һ��֧���������,�뽻�׻������***
int lv_get_orderno();
*/
/****�õ����׵�������Ϣ******
char *lv_pay_in_txname(PAY_IN_AREA *wp_pay_in);
*/
/****�õ��˻���������**************
char* lv_fetch_retdtl(PAY_IN_AREA *wp_pay_in);
*/
/****�ж�һ���ڲ��ʺţ�ȡ�����ʺţ��Ƿ�����͸֧**
BOOL lv_account_cantz(PAY_IN_AREA *wp_pay_in);
*/
/****���������ѽ��********
int lv_count_feeamt(PAY_IN_AREA *wp_pay_in);
*/
/****�����뽻����****
char * lv_des_pswdcode(char *);
*/
/****�������뽻����****
char * lv_undes_pswdcode(char *);
*/
/****�õ�ҵ�����͵�����******
char *lv_pay_in_txtype(char *txnum);
*/
/****�õ�ҵ�����������******
char *lv_pay_in_ywtype(char *pywtype);
*/
/***
char* pcLvGetRecvno();
char *pcLvGetDay();
char *pcLvGetDay_work();
char *pcPayinTxname(PAY_IN_AREA *wp_payin);
char* pcPayinRetdtl(PAY_IN_AREA *wp_payin);
char *pcLvPayinTxtype(char *txnum);
char *pcLvPayinYwtype(char *pywtype);
char * pcLvDesPswdCode(char *code);
char * pcUndesPswdCode(char *code);
char *Rtn_info_SqlBuf(int val);
char *Rtn_Tran_SqlBuf(int val);
******************/



/**************************************************************************************/
/*��֧�����׽ṹ�嶨�忪ʼ*/
typedef struct{
    char    optype [ 2];    /*��������*/
    char    pkgno  [ 3];    /*package���*/
    char    cmtno  [ 3];    /*cmt���*/
    char    orderno[ 8];    /*֧���������*/
    char    txnum  [ 5];    /*ҵ�����ͱ��*/
    char    acbrno [12];    /*�������к�*/
    char    rettype    ;    /*�˻�����0-����1-����*/
    char    respcode   ;    /*Ӧ����1-ͬ��2-����*/
    char    opackday[8];    /*ԭ����������*/
    char    opkgno  [3];    /*ԭ�����ͺ�*/
    char    opackid [8];    /*ԭ������id*/
    char    otxnum  [5];    /*ԭҵ�����ͱ��*/
    char    owtday  [8];    /*ԭί������*/
    char    oorderno[8];    /*ԭ֧���������*/
    char    oorbrno [12];   /*ԭ�������к�*/
    char    oacbrno [12];   /*ԭ�������к�*/
    char    otxtype;        /*ԭ��������*/
    char    cour     [3];   /*���ҷ���*/
    char    otxamt  [15];   /*ԭ���׽��*/
    char    content[255];   /*����*/
    char    refno[20];     /*���Ĳο���*/
    
    char    unit[60];        /*�ո���λ*/
    char    belongcyc[16];   /*��������*/
    char    sftype[2];       /*�ո�����*/
    char    pdate[8];        /*��Ʊ����*/
    char    pnum[20];        /*֧Ʊ����*/
    char    paybrname[60];   /*����������*/
    char    actno[32];       /*��Ʊ���ʺţ��ʻ���ѯҵ��ʱ�����ʺ�*/
    char    name[60];        /*�տ������ƣ��ʻ���ѯҵ��ʱ��������*/
    char    reason[60];      /*��;*/
    char    bsnum[2];        /*��������*/
    char    passwd[60];      /*֧������*/
    char    datalen[8];      /*���ݳ���*/
    char    acttype;         /*�ʺ�����*/
    char    pswdcd;          /*��������*/
    char    pascode[128];    /*������֤��*/
    char    qtype;           /*��ѯ���ͣ���ѯӦ��ʱ��Ϊԭ��ѯ����*/

    /*�Զ���ȡ��Ϣ*/
    char    orbrno     [12]; /*�������к�*/
    char    payqsactno [12]; /*�����������к�*/
    char    sendco     [4];  /*��������*/
    char    cashqsactno[12]; /*�����������к�*/
    char    receco     [4];  /*�ձ�����*/
    char    wssrno     [6];  /*��ˮ��*/
    char    txday      [8];  /*��������*/
    char    stat;            /*״̬*/
    char    datatype;        /*������������0������1���ļ�*/
    char    addid      [8];  /*��������id*/
    /****Add by SSH,2006.2.8****/
    /***��֧�����׵��ֶ�***/
    char    qorderno[8];     /****�������****/
    char    qwtday[8];       /****��������****/
    char    qorbrno[12];     /****�������к�****/
    char    racbrno[12];     /****Ӧ�����к�****/
    char    rwtday[12];      /****Ӧ������****/
    char    rorderno[12];    /****Ӧ�����****/
	/** add by LiuHuafeng 20060307 **/
	char    totamt[15];   /* ���е��ܱ��� */
	char    totcnt[8];    /* ���е��ܽ�� */
}NOTPAY_IN_AREA;

/*��֧�������������*/
typedef struct{
    char    filename [16];        /*��������*/
    char    txday    [8];         /*��������*/
    char    brno     [7];		  /*���׻���*/
    char    orderno  [8];         /*֧���������*/
    char    txtype   [12];        /*ҵ�����ͣ�txnum������*/
    char    orbrno   [12];        /*�������к�*/
    char    acbrno   [12];        /*�������к�*/
    char    content  [255];       /*����*/
    char    wssrno   [6];         /*��ˮ��*/
    char    sfflag;               /*�ո���־0����1����*/
    char    unit     [60];        /*�ո���λ*/
    char    belongcyc[16];        /*��������*/
    char    pdate    [8];         /*��Ʊ����*/
    char    pnum     [20];        /*֧Ʊ����*/
    char    paybrname[60];        /*����������*/
    char    bsnum    [2];         /*��������*/
    char    datalen  [8];         /*���ݳ���*/
    char    reason   [60];        /*��;*/
    char    sftype   [2];         /*�ո�����*/
    char    acttype;              /*�ʻ�����*/
    char    actno    [32];        /*�ʺ�*/
    char    name     [60];        /*����*/
    char    pswdcd;               /*��������*/
    char    passwd   [128];       /*������֤��*/
    char    qtype;                /*��ѯ����*/
    char    respday  [8];         /*Ӧ������*/
    char    qcontent [60];        /*��ѯ����*/
    char    opackday [8];         /*ԭ����������*/
    char    opackid  [8];         /*ԭ������id*/
    char    opkgno   [3];         /*ԭ�����ͺ�*/
    char    otxtype  [12];        /*ԭҵ�����ͣ�otxnum������*/
    char    oorderno [8];         /*ԭ֧���������*/
    char    avbal    [15];        /*��ǰ���*/
}NOTPAY_OUT_AREA;

/*��֧�����ױ�׼���벿�֣�ǰ̨��������tis�ṹ*/
typedef struct{
    char    optype   [2];   /*��������*/
    char    pkgno    [3];   /*package���*/
    char    cmtno    [3];   /*cmt���*/
    char    orderno  [8];   /*֧���������*/
    char    txnum    [5];   /*ҵ�����ͱ��*/
    char    acbrno   [12];  /*�������к�*/
    char    rettype     ;   /*�˻�����0-����1-����*/
    char    respcode    ;   /*Ӧ����1-ͬ��2-����*/
    char    opackday [8];   /*ԭ����������*/
    char    opkgno   [3];   /*ԭ�����ͺ�*/
    char    opackid  [8];   /*ԭ������id*/
    char    otxnum   [5];   /*ԭҵ�����ͱ��*/
    char    owtday   [8];   /*ԭί������*/
    char    oorderno [8];   /*ԭ֧���������*/
    char    oqorderno[8];   /*ԭ��ѯ���*/
    char    oqday    [8];   /*ԭ��ѯ����*/
    char    oorbrno  [12];  /*ԭ�������к�*/
    char    oacbrno  [12];  /*ԭ�������к�*/
    char    otxtype     ;   /*ԭ��������*/
    char    cour     [3];   /*���ҷ���*/
    char    otxamt   [15];  /*ԭ���׽��*/
    char    reason   [255]; /*����*/
}NOTPAY_IN_COMM;

/*����һ���Զ���ֵ����*/
typedef struct{
    char    orbrno     [12];
    char    payqsactno [12];
    char    sendco     [4];
    char    cashqsactno[12];
    char    receco     [4];
    char    wssrno     [6];
    char    txday      [8];
    char    stat          ;
}NOTPAY_IN_AUTO;


/*��֧�����ױ�׼�������*/
typedef struct{
    char    filename[16];       /*��������*/
    char    txday   [8];        /*�������ڣ�С������*/
    char    brno    [7];		 /*���׻���*/
    char    orderno [8];        /*֧���������*/
    char    txtype  [12];       /*ҵ�����ͣ�txnum������*/
    char    orbrno  [12];       /*�������к�*/
    char    acbrno  [12];       /*�������к�*/
    char    content [255];      /*����*/
    char    wssrno  [6];        /*��ˮ��*/
}NOTPAY_OUT_COMM;

/*cmt���������������*/
typedef struct{
    char    opackday[8];  /*ԭ����������*/
    char    opackid [8];  /*ԭ������id*/
    char    oorderno[8];  /*ԭ֧���������*/
    char    opkgno  [3];  /*ԭ�����ͺ�*/
    char    otxtype [12]; /*ԭҵ�����ͣ�otxnum������*/
}NOTPAY_OUT_CMT;



/*֧ƱȦ��*/
typedef struct{
    char    pdate    [8];   /*��Ʊ����*/
    char    pnum     [20];  /*֧Ʊ����*/
    char    paybrname[60];  /*����������*/
    char    payactno [32];  /*��Ʊ���ʺ�*/
    char    cashname [60];  /*�տ�������*/
    char    reason   [60];  /*��;*/
    char    bsnum    [2];   /*��������*/
    char    passwd   [60];  /*֧������*/
    char    piclen   [8];   /*Ʊ��ͼ�����ݳ���*/
}ZPQC_ADD;


/*��������ʼ�¼�嵥*/
typedef struct{
    char    zclx    [1]; /*����ڵ�����*/
    char    zcday   [8]; /*��������*/
    char    zccc    [2]; /*�����*/
    char    pkgno  [3]; /*�����ͺ�*/
    char    wbcnt   [8]; /*��������*/
    char    wbtxamt [22];/*�������ϼ�*/
    char    lbcnt   [8]; /*��������*/
    char    lbtxamt [22];/*�������ϼ�*/
}FINISH_QS;
/*����״̬���ʼ�¼�嵥*/
typedef struct{
    char    pkgno  [3]; /*�����ͺ�*/
	char    packstat[2]; /* ������״̬ */
    char    wbcnt   [8]; /*��������*/
    char    wbtxamt [22];/*�������ϼ�*/
    char    lbcnt   [8]; /*��������*/
    char    lbtxamt [22];/*�������ϼ�*/
}UNFINISH_QS;

/*�����������ϸ��¼ͷ�嵥*/
typedef struct{
	char    lwbs    [1]; /* �����ʱ�ʶ 1����,2����*/
    char    zcjdlx  [1]; /*����ڵ�����*/
    char    zcday   [8]; /*��������*/
    char    zccc    [2]; /*�����*/
    char    pkgno  [3]; /*�����ͺ�*/
    char    cnt     [8]; /*��������*/
}MX_FINISH_QS_HEAD;
/*�����������ϸ��¼���嵥*/
typedef struct{
	char    payqsactno  [12]; /* ������������*/
    char    packday  [8]; /*��ί������*/
    char    packid   [8]; /*�����*/
    char    dtlcnt  [8]; /*���ܱ���*/
    char    totamt  [22]; /*���ܽ��*/
    char    qsday   [8]; /*��������*/
}MX_FINISH_QS_BODY;

/*�����������ϸ��¼�嵥*/
typedef struct{
	char    lwbs    [1]; /* �����ʱ�ʶ 1����,2����*/
	char    pkgno  [3]; /*�����ͺ�*/
	char    packstat[2]; /* ������״̬ */
	char    cnt     [8]; /*��������*/
}MX_UNFINISH_QS_HEAD;

/*����״̬Ӧ����ϸ�嵥*/
typedef struct{
	char    payqsactno  [12]; /* ������������*/
	char    packday  [8]; /*��ί������*/
	char    packid   [8]; /*�����*/
	char    dtlcnt  [8]; /*���ܱ���*/
	char    totamt  [22]; /*���ܽ��*/
}MX_UNFINISH_QS_BODY;

typedef struct{
	char    lwbs    [1]; /* ��/���˱�ʶ */
	char    zcjdlx  [1]; /* ����ڵ�����*/
	char    zcday   [8]; /* �������� */
	char    zccc    [2]; /* ����� */
	char    pkgno   [3]; /* �����ͺ� */
}MXHD_850_QS;
typedef struct{
	char    lwbs    [1]; /* ��/���˱�ʶ */
	char    pkgno   [3]; /* �����ͺ� */
	char    packstat[2]; /* ������״̬ */
}MXHD_850_QT;

/*added by ���յ� 2006-02-16*/
typedef struct{
	char countno[19]; /*18λ�ʺ�*/
	char name[61]; /*����*/
	char leftvalue[17]; /*���*/
}HT_INTERFACE;

/*liuyue 20060912**����С��ķ�֧�������˴���123��,���漸���ֶ���Ҫ���⴦��***/
typedef struct{
	char opackid  [8];
	char opack_date[8];
	char otxnum    [5];
	char opkgno    [3];
	char reqtype   [1];
}LV_NOTPAY_ADD;

#define        TXNUM_LEN             5
#define        NOTPAY_IN_COMM_LEN    sizeof(NOTPAY_IN_COMM)
#define        NOTPAY_IN_AUTO_LEN    sizeof(NOTPAY_IN_AUTO)
#define        NOTPAY_OUT_COMM_LEN   sizeof(NOTPAY_OUT_COMM)
#define        SFYW_ADD_LEN          sizeof(SFYW_ADD)
#define        ZPQC_ADD_LEN          sizeof(ZPQC_ADD)
#define        ZHCX_ADD_LEN          sizeof(ZHCX_ADD)
#define        ZHCXYD_ADD_LEN        sizeof(ZHCXYD_ADD)
#define        NOTPAY_OUT_CMT_LEN    sizeof(NOTPAY_OUT_CMT)
/*���Ĳ����ɹ������ֺ͸��Ӳ��ֹ�ͬ���*/
/*��֧�����׽ṹ�嶨�����*/
/*******************************************************************************************/
/** ����С��ҵ��һЩ��Ŀ *******/
#define  LV_NEXTDAY_GZ_HANDFEE_INCOME   HV_SUB03_HAND_INCOME /* �¶�պ�������������ʿ�Ŀ */
#define  LV_NEXTDAY_GZ_RECV   HV_QSGZ_ACNO   /* С��պ����ʹ��ʿ�Ŀ */
#define  LV_NEXTDAY_GZ_SEND   HV_QSGZ_ACNO   /* С��պ����ʹ��ʿ�Ŀ */

/*** ����lv_pay_in_check2��������ʻ�״̬������Ϣ **/
#define ACT_OK          0   /* �ʻ�״̬���� */
#define ACT_ERR         1   /* �ʺŲ����� */
#define ACT_NAME_ERR    2   /* �ʻ��������� */
#define ACT_AVBAL_LOW   3   /* �ʻ����� */
#define ACT_PASSWD_ERR 10   /* �ʻ�������� */
#define ACT_FZN        11   /* ״̬���� */
#define ACT_OTHER_ERR  -1   /* �������� */
/*** ����С������ҵ�񱻾ܾ��Ƿ���˻���ԭ·��� ***/
#define REJECT_ACMR_FLAG  1 /* 1 ԭ·��� �����ǹ���*/
/* ���������Ĺ��ɿ�Ŀ */
#define WAIT_QS_SUBJECT "4140300" /* ��������ɿ�Ŀ*/
/*#define LV_SUB02_SEND "4110500"  С��֧������*/
#define LV_SUB02_SEND "41102" /*С��֧������*/
#define LV_SUB02_RECEIVE "4120500" /* С��֧������*/
#define LV_SUB02_HAND_INCOME   "51108"   /*С������������*/
#define LV_SUB02_HAND_PAYOUT   "52705"   /*С��������֧��*/
#define LV_QSGZ_ACNO  "2610500"   /*С����˿�Ŀ*/
char* lv_getrecvno();
/****HSYX BEGIN :��ӡ���****/
#define CHY_NOTCHECK '0'
#define CHY_CHECKSUCCESS '1'
#define CHY_CHECKFAIL    '2'
/****HSYX END :��ӡ���****/

/*********����С���Ӫҵ״̬*************/
#define LV_STAT_PREPARE		'0' 	/*Ӫҵ׼��*/
#define LV_STAT_WORK		'1'		/*�ռ䴦��*/
#define LV_STAT_WARN		'2'		/*����׼��*/
#define LV_STAT_STOP		'3'		/*ҵ�����*/
#define LV_STAT_QSWIN		'4'		/*���㴰��*/
#define LV_STAT_WORKEND		'5'		/*���մ���ʼ*/
#define LV_STAT_CHECKERR	'6'		/*���ն��˲�ƽ*/
#define LV_STAT_CHECKRIGHT	'7'		/*���ն����Ѿ�ƽ*/
#define LV_STAT_UNKNOWN		'F'		/*δ֪״̬*/

/***********************************************
    �йػ��ڽӿڵĶ��� 2006-3-4 20:38 ���յ�
***********************************************/
#define HT_SENDBUF_LEN			151
#define HT_RECVBUF_LEN    	147

struct str_czzf_area{
	char dwbh[4];/**����֧����λ���**/
	char capital;/**����֧���ʽ�����**/
	char finvoc[20];/**����֧��ƾ֤��**/
	char vocnum[12];
	char voctype[2];
};
/*****************************************
* ����֧��ҵ��ԭʼ��/��ϸ������Ϣ�ṹ��  *
* add by LiuHuafeng 20060309             *
*****************************************/
typedef struct{
	char    findtype   [1];/*0���� 1 ������ */
	char    packid     [8];/* findtype=1 ʱ������ */
	char    packday    [8];
	char    wtday      [8];/*������ */
	char    orbrno     [12];/*������ */
	char    orderno    [8]; /* ֧��������� */
	char    pkgno      [3]; /*findtype=1ʱ�������д ������ */
	char    lwbs       [1]; /*0��,1��,����������ͳһ */
/* �����Ǹ��ݹؼ����Ұ�/������ϸ */
	char    totcnt     [8];
	char    totamt     [15];
	char    payactno   [32];
	char    payname    [60];
	char    cashactno  [32];
	char    cashname   [60];
	char    acbrno     [12];
	char    payqsactno [12];
	char    cashqsactno[12];
	char    wssrno     [6];
	char    txday      [8];
	char    stat       [1];
	char    begstat    [1];
	char    rcpstat    [2];
}QUERY_PAY_INFO;

typedef struct{
	char pack_date[8];
	char packid[8];
	char respCode[8];
	char flg[1];/*������־*/
}CMT920_8583_JZ;

struct str_lv_852_mx_dtl{
char lw_ind    [ 1];
char pkg_no    [ 3];
char pay_qs_no [12];
char pack_date [ 8];
char packid    [ 8];
};
struct str_lv_852_mx_dtl_print{
char lw_ind    [ 2];
char pkg_no    [ 4];
char pay_qs_no [13];
long pack_date ;
char packid    [ 9];
};

struct str_lv_852_head_print{
       long wt_date ;
       char pay_qs_no [13];
       long dz_date ;
       long dtlcnt ;
};

struct str_lv_852_head{
       char wt_date[9] ;
       char pay_qs_no [13];
       char dz_date[9];
       char dtlcnt [9];
};

#endif /**����INCLUDE_LV_DEFINE**/
