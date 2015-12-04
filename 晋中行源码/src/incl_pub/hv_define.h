#ifndef _HV_DEFINE_H_
#define _HV_DEFINE_H_

#include "public.h"
#include "hv_zf_c.h"
#include "hv_fzf_c.h"
#include "hv_uniontab_c.h"
#include "hv_fd123.h"

#ifndef TLRLENGTH
#define TLRLENGTH 6
#endif

#ifndef BRNO_LEN
#define BRNO_LEN 7
#endif

#ifndef HV_BR_NO
#define HV_BR_NO "00000"
#endif

#ifndef HV_CZ_QSBRNO
#define HV_CZ_QSBRNO    "10001"    /**�������Ļ�����**/
#endif

#ifndef HV_CUNZHEN_BR_NO
#define HV_CUNZHEN_BR_NO "9999" /**�������е�ͷ4λ**/
#endif

#ifndef HV_QS_BR_NO
#define HV_QS_BR_NO "313175000011"
#endif

#ifndef HV_ORDERNO_IND
#define HV_ORDERNO_IND "99"         /****���֧���������****/ 
#endif

#ifndef HV_REFNO_IND
#define HV_REFNO_IND "98"           /****���֧�����ױ��Ĳο���****/  
#endif

#ifndef HV_ADDID_IND
#define HV_ADDID_IND "97"           /****���������****/
#endif

#ifndef MBFE_FZF_IND
#define MBFE_FZF_IND "92"           /****���������****/
#endif

#ifndef MBFE_FZF_BR_NO
#define MBFE_FZF_BR_NO TOT_BR_NO
#endif

#ifndef HP_CENTER
#define HP_CENTER "1234"
#endif

#ifndef MAX_803_REC
#define MAX_803_REC 30
#endif
/** add by LiuHuafeng ��Ʊ�ʺŲ������ ***/
#ifndef PARM_HPQF          /** 2440401������Ʊǩ���ʺ� */
#define PARM_HPQF 1
#endif

#ifndef PARM_HPYC          /** 2440402������Ʊ�ƴ��ʺ� */
#define PARM_HPYC 2
#endif

#ifndef PARM_HPTH          /** 2440403������Ʊ�����˻��ʺ� */
#define PARM_HPTH 3
#endif

#ifndef PARM_HPGZ          /** 26106������Ʊ�����ʺ� */
#define PARM_HPGZ 4
#endif

#ifndef PARM_HVQS          /** MBFE 110��Ŀר���˺� */
#define PARM_HVQS 5
#endif

#ifndef PARM_HVGZ          /** MBFE 26104���ʹ��ʿ�Ŀר���˺� */
#define PARM_HVGZ 6 
#endif

#ifndef PARM_HVCJ          /** ��� 2618����Ŀר���˺� */
#define PARM_HVCJ 7 
#endif

/*** move to lv_define.h 
#ifndef PARM_LVGZ          * BEPS  С�����ר���˺� * 
#define PARM_LVGZ 8 
#endif
******************************************************/

#ifndef PARM_CZYH_QS_NO     /* BEPS  �������ר���˺� */
#define PARM_CZYH_QS_NO 12 
#endif


#ifndef HVLZGD          /** ��� ���ʹ��ɿ�Ŀ */
#define HVLZGD  "41201"
#endif

#ifndef HVWZGD          /** ���  ���ʹ��ɿ�Ŀ */
#define HVWZGD  "41101"
#endif


#define HV_PAYIN_FIX_BLOCK \
	char    F_crdb          [ 1];/**�����־0��1��**/ \
	char    F_opcd          [ 2];/**�������� ��С��Ϊ��*/ \
	char    F_lw_ind        [ 1]; /* ������ʶ */ \
	char    F_tx_br_no      [ 7]; /* ���׻��� */ \
	char    F_tx_date       [ 8]; /*�������� */ \
	char    F_wt_date       [ 8] ;/**ί������       **/ \
	char    F_orderno       [ 8];/**֧���������   **/  \
	char    F_cmtno         [ 3];/**cmt���    **/ \
	char    F_cur_no        [ 3]; /* �������� */ \
	char    F_note_type     [ 3];/**ƾ֤���� **/ \
	char    F_note_no       [16];/**ƾ֤���� **/ \
	char    F_tx_amt        [16];/**���׽��   **/ \
	char    F_tx_chrg_ind   [ 1]; /**�����ѱ�־ **/ \
	char    F_tx_chrg_amt   [15];/**�����ѽ��     **/ \
	char    F_or_br_no      [12];/**�������к�     **/  \
	char    F_ac_br_no      [12];/**�������к�     **/  \
	char    F_pay_opn_br_no [12];/**�����˿�����   **/  \
	char    F_pay_ac_no     [32];/**�������ʺ�     **/  \
	char    F_pay_name      [60];/**����������     **/  \
	char    F_pay_addr      [60];/**�����˵�ַ     **/  \
	char    F_userpswd      [ 8];/**��������       **/  \
	char    F_cash_opn_br_no[12];/**�տ��˿������к�**/ \
	char    F_cash_ac_no    [32];/**�տ����ʺ�     **/  \
	char    F_cash_name     [60];/**�տ�������     **/  \
	char    F_cash_addr     [60];/**�տ��˵�ַ     **/  \
	char    F_yw_type       [ 2];/** ҵ������ **/ \
	char    F_brf           [60];/**����           **/ \
	char    F_operlevel     [1 ];/**���ȼ�         **/ \
	char    F_tel       [TLRLENGTH];/**����Ա��   **/ \
	char    F_chk       [TLRLENGTH];/**���˲���Ա��   **/ \
	char    F_auth_tel  [TLRLENGTH];/**��Ȩ����Ա��   **/ \
	char    F_auth_pwd  [8]        ;/**��Ȩ����Ա���� **/ \
	char    F_refno     [20]; /* ���Ĳο��� */            \
	/**�����ֶι��ɱ�׼���벿�֣�����������潻�ײ�ͬ��ѡ���Ը�ֵ**/ \
	char    F_o_tx_date [ 8];/* ԭ�������� */ \
	char    F_o_cmtno   [ 3];/**ԭCMT���**/ \
	char    F_hv_brf    [60];/**����***/     \
	char    F_o_wt_date [ 8];/**ԭί������    **/ \
	char    F_o_orderno [ 8];/**ԭ֧���������**/ \
	char    F_beg_sts   [ 1];/* ԭ֧������״̬ */ \
	char    F_hv_osts   [1 ]; /* �˻㽻���õ���ԭ֧�����ʽ���״̬ */ \
	char    F_resp1     [ 8];/**ccpcӦ����    **/ 
typedef struct {
    HV_PAYIN_FIX_BLOCK
}HV_PAYIN_FIX_AREA;

#define HV_PAYIN_TABLE_BLOCK \
	char   T_tx_br_no     [BRNO_LEN]; /* �������к�*/  \
	char   T_tx_date         [8 ]  ; /* ��������*/    \
	char   T_wt_date         [8 ]  ; /* ί������*/    \
	char   T_tx_time         [6 ]  ; /* ����ʱ��*/    \
	char   T_orderno         [8 ]; /* ���еĽ������*/ \
	char   T_cur_no          [3 ]; /* ���ҷ���*/  \
	char   T_note_type       [3 ]; /* ƾ֤���� */ \
	char   T_note_no         [16]; /* ƾ֤�� */   \
	char   T_tx_amt          [16]; /* ���׽�� */ \
	char   T_tx_chrg_ind     [1 ]; /* �����ѱ�־ 0.�ֽ� 1.�ֽ�Ӽ� 2.ת��  \
                                  3.ת�ʼӼ� 4.����*/ \
	char   T_chrg_amt        [16]; /* ������ */ \
	char   T_pay_qs_no       [12]; /* �����������к�*/ \
	char   T_or_br_no        [12]; /* �������к�--�˻㽻�׵�ԭ������ */ \
	char   T_pay_opn_br_no   [12]; /* �����˿������к� */	\
	char   T_pay_ac_no       [32]; /* �������ʺ� */	\
	char   T_pay_name        [60]; /* ����������-�˻㽻�׵�ԭ�տ��˻���*/ \
	char   T_pay_addr        [60]; /* �����˵�ַ */                     \
	char   T_cash_qs_no      [12]; /* �����������к� */                 \
	char   T_ac_br_no        [12]; /* �������к�--�˻㽻�׵�ԭ������ */ \
	char   T_userpswd        [8 ]; \
	char   T_cash_opn_br_no  [12]; /* �տ��˿������к� */ \
	char   T_cash_ac_no      [32]; /* �տ����ʺ�       */ \
	char   T_cash_name       [60]; /* �տ�������-�˻㽻�׵�ԭ�����˻���*/ \
	char   T_cash_addr       [60]; /* �տ��˵�ַ   */  \
	char   T_yw_type         [2 ]; /* ҵ������     */  \
	char   T_sendco          [4 ]; /* �������Ĵ��� */  \
	char   T_receco          [4 ]; /* �ձ����Ĵ��� */  \
	char   T_cmtno           [3 ]; /* CMT���      */  \
	char   T_beg_sts         [1 ]; /* ���statԭʼ״̬����ʱ��ල�� */ \
	char   T_operlevel       [1 ]; /* ���ȼ���0-һ��1-����       \
                                  2-�ؼ�[����ս�����֧��]*/ \
	char   T_hv_sts          [1 ]; /* ״̬ �����hv_define.h���� */ \
	char   T_tel             [6 ]; /* ����Ա */ \
	char   T_trace_no        [6 ]  ; /* ��ˮ��*/      \
	char   T_hv_osts         [1 ]; /* �˻㽻���õ���ԭ֧�����ʽ���״̬ */ \
	char   T_checkflag       [1 ]; /* ���ʼ���־*/ \
	char   T_hv_prt_ind      [10]; /* ��ӡ��־��0δ��ӡ��1�Ѵ�ӡ*/ \
	char   T_resend_ind      [1 ]; /* �ܾ��ط���־0δ���·��ͣ�  \
                                  1��ʾ�Ѿ����·��;�����;   \
                                  2����ʾ�ֹ��˻�֧�У�      \
                                  M;��ʾ���·��͵�������  */ \
	char   T_resend_date     [8 ]; /* �ܾ����·������� */ \
	char   T_o_orderno       [8 ]; /* ԭ֧���������--�˻㽻���õ� */ \
	char   T_o_tx_date       [8 ]; /* ԭ֧����������--�˻㽻���õ� */ \
	char   T_o_cmtno         [3 ]; /* ԭcmtno--�˻㽻���õ� */ \
	char   T_lw_ind          [1 ]; /* ������ʶ 1-�� 2-�� */    \
	char   T_addid           [8 ]; /* ������ id --addit��*/    \
	char   T_refno           [20]; /* ���Ĳο��� */            \
	char   T_auth_tel        [6 ]; /* ��Ȩ����Ա*/             \
	char   T_auth_trace_no   [8 ]; /* ��Ȩ����Ա��ˮ��*/       \
	char   T_chk             [6 ]; /* ����Ա */ \
	char   T_chk_trace_no    [6 ]; /* ���˲���Ա��ˮ��*/       \
	char   T_dealerr_date    [8 ]; /* ���ʴ�������*/           \
	char   T_dealerr_tel     [6 ]; /* ���ʴ����Ա*/           \
	char   T_resp1           [8 ]; /* Ӧ����*/                 \
	char   T_o_wt_date       [8 ]; /*ԭί������*/         \
	char   T_send_tel     [TLRLENGTH];/* ���Ͳ���Ա */ \
	char   T_send_tel_trace_no[6]; /* ���Ͳ���Ա��ˮ�� */ \
	char   T_hv_brf          [60];  /* ��ע */

#define HV_PAYIN_AUTO_BLOCK \
	char A_tx_date       [8 ];/* ��������  */                   \
	char A_trace_no      [6 ];/**��ǰ����Ա��ˮ��**/            \
	char A_sts           [1 ];/**״̬;����ֶζ�������Ч��      \
                          �������ʱ��ֵ����ֵΪ��������**/ \
	char A_sendco        [4 ];/**��������**/                    \
	char A_pay_qs_ac_no  [12];/**�����������к�**/              \
	char A_receco        [4 ];/**�ձ�����**/                    \
	char A_cash_qs_ac_no [12];/**����������**/                  \
	char A_dr_br_no  [BRNO_LEN];/* ֧���˻����ڻ��� */          \
	char A_dr_acc_no     [7 ];/* ֧���˻���Ӧ��Ŀ**/            \
	char A_avbal1        [15];/**����ǰ���**/                  \
	char A_avbal2        [15];/**���˺����**/                  \
	char A_dr_ac_ind     [1 ];/**�ʺ�����;Ϊ�˽�ʡ���ʼ��ʱ��*/ \
	char A_tx_br_no  [BRNO_LEN];/**������������Ļ�����****/


typedef struct {
	HV_PAYIN_FIX_BLOCK
	HV_PAYIN_TABLE_BLOCK
	HV_PAYIN_AUTO_BLOCK
}HV_PAYIN_AREA;

/******ί���տ�*******/
typedef struct{
	char  pj_date [8];
	char  pj_num  [8];
	char  note_type[2]; /* ƾ֤���� add by LiuHuafeng */
}HV_WTSK_ADD;

/***���ճи�********/
typedef struct{
	char	pj_date    [8];
	char	pj_num     [8];
	char	ptxamt   [15];
	char	jtxamt   [15];
}HV_TSCF_ADD;

/****����ͬҵ���*****/
typedef struct{
	char  rate          [7];
	char  ratelimit     [5];
}HV_TYCJ_ADD;
/****���л�Ʊ**********/
typedef struct{
	char	last_ac_no      [32];/****����Ʊ���ʺ�***/
	char	last_name       [60];/****����Ʊ������***/
	char	pj_date         [8 ];/****Ʊ������***/
	char	pj_num          [8 ];/****Ʊ�ݺ���***/
	char	hpmy            [10];/****��Ʊ��Ѻ***/	
	char	cp_amt          [15];/****��Ʊ���***/	
	char	dy_amt          [15];/****������**/	
}HV_HP_ADD;

/******721���ĸ���������Ҫ�õ�***********/
typedef struct{
	char  pj_date         [8 ];/****Ʊ������****/
	char  pj_num          [8 ];/****Ʊ�ݺ���****/
	char  hp_type         [1 ];/****��Ʊ����****/
	char  hp_miya         [10];/****��Ʊ��Ѻ****/
	char  cp_amt          [15];/****��Ʊ���****/
	char  real_js_amt  [15];/****ʵ�ʽ�����*/
	char  dy_amt          [15];/****������****/
	char  last_ac_no      [32];/****����Ʊ���ʺ�****/
	char  last_name       [60];/****����Ʊ������****/
	char  return_date     [ 8];/****��ʾ��������****/
	char  pay_opn_br_no  [12];/****�����˿�����,�Ҹ���**/
	char  cash_opn_br_no  [12];/****�տ��˿�����,����Ʊ�˿�����**/
}HV_721_ADD;

typedef struct{
	char  pj_date         [8 ];/****Ʊ������****/
	char  pj_num          [8 ];/****Ʊ�ݺ���****/
	char  hp_miya         [10];/****��Ʊ��Ѻ****/
	char  cp_amt          [15];/****��Ʊ���****/
	char  real_js_amt  [15];/****ʵ�ʽ�����*/
	char  cash_opn_br_no  [12];/****�տ��˿�����,����Ʊ�˿�����**/
	char  last_ac_no      [32];/****����Ʊ���ʺ�****/
	char  last_name       [60];/****����Ʊ������****/
	char  return_date     [ 8];/****��ʾ��������****/
}HV_123_ADD;

typedef struct{
	char  spec_br_no		  [12];/****���������,�ֽ��Ʊ�����****/
	char  pj_date         [8 ];/****Ʊ������****/
	char  pj_num          [8 ];/****Ʊ�ݺ���****/
	char  hp_type         [1 ];/****Ʊ������****/
	char  hp_miya         [10];/****��Ʊ��Ѻ****/
}HV_121_ADD;


typedef struct{
	char  spec_br_no		  [12];/****���������,�ֽ��Ʊ�����****/
	char  pj_date         [8 ];/****Ʊ������****/
	char  pj_num          [8 ];/****Ʊ�ݺ���****/
	char  hp_miya         [10];/****��Ʊ��Ѻ****/
}HV_124_ADD;

/****CMT232�����漰���ĸ�����**/
typedef struct{
	char tag_90A 		  [ 6];/*�����������/��������*/
	char tag_CEB 		  [ 3];/*ҵ������,��ʱת������*/
	char tag_CF2		  [ 9];/*ծȯ������ƥ��ָ���*/
	char tag_CF1 		  [12];/*ծȯ����*/
	char tag_CNN 		  [15];/*���۽��*/
	char tag_CNP 		  [15];/*ծȯ��Ϣ*/
 	char tag_CNM		  [15];/*ծȯ���*/
	char tag_CNQ		  [15];/*�ع����ڽ����*/
	char tag_CJ8		  [ 8];/*�ع�������*/
	char tag_CNR		  [15];/*�ع���Ϣ*/
	char tag_90C          [ 1];/*�����ʶ*/
	char tag_CCF          [ 4];/*�����������CCPC����*/
	char tag_CCG		  [ 4];/*������������CCPC����*/
}HV_232_ADD;

/******�ռ�͸֧��Ϣ���� 803�嵥����**BEG ADD BY ChenMing*****/
typedef struct{
		char	rate_time [ 6];   /*��Ϣʱ��*/
		char	over_amt  [22];   /*͸֧���*/
		char  rate	    [ 6];     /*����*/
		char  over_intr [22];   /*͸֧��Ϣ*/ 
}CMT_803_RATE_INFO;

typedef struct{
    char tag_CD4		  [ 4];     /*��Ϣʱ����*/
    CMT_803_RATE_INFO tag_CLD[MAX_803_REC];
}HV_803_MAIN_ADD;		


typedef struct{
	char  pj_date         [8 ];/****Ʊ������****/
	char  pj_num          [8 ];/****Ʊ�ݺ���****/
	char  hp_type         [1 ];/****��Ʊ����****/
	char  df_br_no       [12];/****ԭָ���Ҹ���***/
	char  cp_amt          [15];/****��Ʊ���****/
	char  hp_miya         [10];/****��Ʊ��Ѻ****/
	char  pay_opn_br_no   [12];/***�����˿����м�ǩ���к�***/
}HV_724_ADD;

typedef struct{
	char  pay_ac_no      [32];/****�������ʺ�,��Ʊ�������ʺ�*****/
	char  pay_name        [60];/****����������,��Ʊ����������*****/
	char  pay_addr        [60];/****�����˵�ַ,��Ʊ�����˵�ַ*****/
	char  pj_date         [8 ];/****Ʊ������****/
	char  pj_num          [8 ];/****Ʊ�ݺ���****/
	char  hp_miya         [10];/****��Ʊ��Ѻ****/
	char  cp_amt          [15];/****��Ʊ���****/
	char  real_js_amt  [15];/****ʵ�ʽ�����*/
	char  cash_opn_br_no  [12];/****�տ��˿�����,����Ʊ�˿�����**/
	char  last_ac_no      [32];/****����Ʊ���ʺ�****/
	char  last_name       [60];/****����Ʊ������****/
	char  return_date     [ 8];/****��ʾ��������****/
}HV_725_ADD;

/******�շ�802�嵥����**BEG ADD BY LiuHuafeng*****/
typedef struct{
	char beg_date [8]; /* �Ʒѿ�ʼ���� */
	char end_date [8]; /* �Ʒ���ֹ���� */
	char zjcy_rate [6];/* ֱ�Ӳ������շѱ��� */
	char hb_rate[6];   /* �б��շѱ��� */
	char ywl_rate[6];  /* ���ۼ�ҵ�����շѱ��� */
	char zf_fee[15];   /* ֧����ҵ���շѽ�� */
	char zf_cnt [8];   /* ֧����ҵ���շѼ�¼���� */ 
	char db_fee[15];   /* ����ҵ���շѽ�� */
	char db_cnt [8];   /* ����ҵ���շѼ�¼���� */
	char xx_fee [15];  /* ��Ϣ��ҵ���շѽ�� */
	char xx_cnt [8];   /* ��Ϣ��ҵ���շѼ�¼���� */
}HV_802_MAIN_ADD;
/******�շ�802�嵥����**END ADD BY LiuHuafeng*****/
/******�շ�81x�嵥����**BEG ADD BY Dongxy*****/
typedef struct{
	char beg_date [8]; /* �Ʒѿ�ʼ���� */
	char end_date [8]; /* �Ʒ���ֹ���� */
	char hv_amt	  [15];   /* ���Ʒѽ�� */
	char lv_amt	  [15];   /* С��Ʒѽ�� */
	char hv_cnt [5];   /* �����ߴ��Ʒ���Ŀ */ 
	char lv_cnt [5];   /* ������С��Ʒ���Ŀ */
}HV_81X_MAIN_ADD;
/******�շ�81x�嵥����**END ADD BY Dongxy*****/

#define HV_NOTPAY_FIX_BLOCK \
	char  F_opcd [ 2];/**�������� ����С��ͳһ */ \
	char  F_lw_ind    [ 1];/**������־ 1 �� 2 ��**/ \
	char  F_tx_br_no  [ 7];/**���׻���**/ \
	char  F_tx_date  [ 8];/**��֧����������**/ \
	char  F_wt_date  [ 8];/**ί������**/ \
	char  F_cmtno     [ 3];/**CMT ���**/ \
	char  F_or_br_no  [12];/**�������к�,��ѯ��**/ \
	char  F_ac_br_no  [12];/**�������к�,�鸴��,ԭ��ѯ��**/ \
	char  F_orderno	  [ 8];/**��֧���������**/ \
	char  F_actno     [32];/**��֧���������漰���ʺ�**/\
	char  F_name      [60];/**�ʺŻ���**/ \
	char  F_content   [255];/**����, ��ѯ���鸴�����ɸ�ʽʹ��**/ \
	char  F_hv_brf    [60];/**����***/ \
	char  F_tel       [TLRLENGTH];/**����Ա**/ \
	char  F_respcode  [ 1];/**�˻�/����Ӧ��,0, ���� 1��������***/ \
	char  F_o_payin_orderno [ 8];/*ԭ֧���������*/ \
	char  F_o_notpay_orderno[ 8];/*ԭ��֧���������, �鸴֮ԭ��ѯ��ŵ�Ӧ�����Ӧ�����뽻�׵����*/\
	char  F_o_cur_no        [ 3];/**ԭ���ҷ���,�鸴�Ƚ���ʹ��**/ \
	char  F_o_txamt         [15];/**֧�����׽��,�鸴��ʹ��**/ \
	char  F_o_tx_type       [ 1];/**ԭ��������**/ \
	char  F_o_wt_date       [ 8];/**ԭί������,�ṩ����Ӧ��֧������**/ \
	char  F_o_fzf_date      [ 8];/**ԭ��֧����������,�鸴���õ��Ĳ�ѯ����**/\
	char  F_o_ac_br_no      [12];/**ԭ֧�����׽����к�**/ \
	char  F_o_or_br_no      [12];/**ԭ֧�����׷����к�**/ \
	char  F_o_cmtno         [ 3];/**ԭCMT���**/ \
	char  F_beg_sts         [1 ];/**ԭ֧������״̬**/ \
	char  F_rcpstat         [2 ];/**ҵ���ִ״̬**/ \
	char  F_refno           [20]; /* ���Ĳο��� */ \
	char  F_resp1           [8 ];/**ccpcӦ���� */ \
	char  F_send_tel  	[6]; /*send_tel*/ \
	char  F_otype		[1];/**1.��� 2.С��**/ \
	char  F_hp_add_pwd    	[1];/* ��Ʊ�Ƿ��Ѻ */

#define	HV_NOTPAY_AUTO_BLOCK \
	char  A_tx_date  [ 8];/**��������**/ \
	char  A_trace_no [ 6];/**��ǰ����Ա��ˮ��**/ \
	char  A_sts      [ 1];/**״̬������ֶζ�������Ч, ��������ʱ��ֵ����ʼֵΪ��������**/ \
	char  A_sendco        [ 4];/**��������**/ \
	char  A_receco        [ 4];/**�ձ�����**/ \
	char  A_pay_qs_ac_no  [12];/**�����������к�**/ \
	char  A_cash_qs_ac_no [12];/**�����������к�**/


#define	HV_NOTPAY_TABLE_BLOCK \
	char  T_or_br_no    [12];/**�������к�**/ \
	char  T_orderno     [ 8];/**�������,��ѯ��ţ��鸴���**/ \
	char  T_tx_date     [ 8];/**��������**/ \
	char  T_wt_date     [ 8];/**ί������,��ѯ�鸴����**/ \
	char  T_br_no       [BRNO_LEN];/**�������к�**/ \
	char  T_hv_fzf_sts  [ 1];/**��֧������״̬,¼�롢����**/ \
	char  T_cmtno       [ 3];/**CMT���**/  \
	char  T_tx_time     [ 6];/**����ʱ��**/ \
	char  T_trace_no    [ 6];/**������ˮ**/ \
	char  T_pay_qs_no   [12];/**���������к�**/ \
	char  T_ac_br_no    [12];/**�������к�**/ \
	char  T_cash_qs_no  [12];/**���������к�**/ \
	char  T_o_wt_date    [ 8];/**ԭί������(֧�����׵�ί������)**/ \
	char  T_o_or_br_no  [12];/**ԭ֧�����׷�����**/ \
	char  T_o_ac_br_no  [12];/**ԭ֧�����׽�����**/ \
	char  T_o_payin_orderno[ 8];/**ԭ֧�����׽������**/ \
	char  T_o_cur_no    [ 3];/**��Ӧ֧�����׵Ļ�������**/ \
	char  T_o_tx_amt    [16];/**��Ӧ֧�����׵Ľ��׽��**/ \
	char  T_o_tx_type   [ 1];/**ԭ�������� ���1 ����2 ����**/ \
	char  T_o_cmtno     [ 3];/**ԭCMT���**/ \
	char  T_sendco      [ 4];/**�������Ĵ���**/ \
	char  T_receco      [ 4];/**�ձ����Ĵ���**/ \
	char  T_req_date    [ 8];/**��������**/ \
	char  T_req_br_no   [12];/**�����к�**/ \
	char  T_req_content [255];/**����,255λ����,��ѯ,�鸴,���ɸ�ʽ**/ \
	char  T_req_orderno [ 8];/**�������,noteԭ��֧���������������ֶ�**/ \
	char  T_res_date    [ 8];/**Ӧ������**/ \
	char  T_res_br_no   [12];/**Ӧ���к�**/ \
	char  T_res_content [255];/**����,255λ����,��ѯ,�鸴,���ɸ�ʽ**/ \
	char  T_res_orderno [ 8];/**Ӧ�����**/ \
	char  T_respcode    [ 1];/**�˻�/����Ӧ��,0, ���� 1��������***/ \
	char  T_addid       [ 8];/**������id,addit--���ݱ�**/ \
	char  T_refno       [20];/**���Ĳο���**/ \
	char  T_hv_prt_ind  [10];/**��ӡ���,0δ��ӡ1�Ѵ�ӡ**/ \
	char  T_tel	    [6];/*����Ա��*/ \
	char  T_hv_brf	    [60];/*��ע,note�ⲻ�Ǹ���**/ \
	char  T_lw_ind      [1];/*������ʶ*/ \
	char  T_o_trace_no  [6];/*old wssrno*/ \
	char  T_send_tel    [6];/*send_tel*/ \
	char  T_resp1       [8];/* ccpc���صĴ����� */ \
	char  T_otype	    [1];/**0.��� 1.С��**/
		
typedef struct{
	HV_NOTPAY_FIX_BLOCK
	HV_NOTPAY_AUTO_BLOCK
	HV_NOTPAY_TABLE_BLOCK
}HV_NOTPAY_AREA;

	
#ifndef HV_HD
#define HV_HD "100"
#endif

#ifndef HV_WT
#define HV_WT "101"
#endif

#ifndef HV_TSCF
#define HV_TSCF "102"
#endif

#ifndef HV_GKZJHH
#define HV_GKZJHH "103"
#endif

#ifndef HV_CJ
#define HV_CJ "105"
#endif

#ifndef HV_TH
#define HV_TH "108"
#endif

#ifndef HV_HPYC
#define HV_HPYC "121"
#endif

#ifndef HV_HP_QS_RECV
#define HV_HP_QS_RECV "122"
#endif

#ifndef HV_HP_PART_RETURN
#define HV_HP_PART_RETURN "123"
#endif

#ifndef HV_HP_ALL_RETURN
#define HV_HP_ALL_RETURN "124"
#endif

/************************************************
* ���ļ������д��֧��ҵ�������һЩ��������    *
* Write by SSH,2004.9.8                         *
************************************************/
#ifndef HV_DEFINE_H
#define HV_DEFINE_H

#define HP_SERVERNAME1 "172.168.99.194"  /* ��Ʊ��Ѻ������1��ַ mod by zyl 20110701 for test last value is 66 */
#define HP_SERVERNAME2 "172.168.99.196"  /* ��Ʊ��Ѻ������2��ַ */
#define TIME_OUT 60  /* �ȴ�ʱ�� */
typedef struct STR_BaseSend {
	char cmtno   [3];
	char separator1;
	char txday   [8];
	char separator2;
	char brno    [BRNO_LEN];
	char separator3;
	char wssrno[6];
	char separator4;
	char orderno[8];
	char separator5;
    char resp1[8];
	char separator6;
	char nextline;
};
typedef struct STR_ChkSend {
	char cmtno  [3];
	char separator1;
	char orderno  [8];
	char separator2;
	char orbrno   [12];
	char separator3;
	char txday   [8];
	char separator4;
	char errmsg  [20];
	char separator5;
	char nextline;
};
#define MAXLINE 38
#define SEND_ACCT_CHECK 	'0'	/*���˼���(����)*/
#define SEND_ACCT_WAIT 		'1'	/*���˷���(����)*/
#define SEND_ACCT_QS		'2'	/*���˼�������/���з�����ѯ�Ѳ鸴;*/
#define SEND_BACK_ACCT		'3'	/*�����˻ؼ���;*/
#define SEND_BACK_WAIT		'4'	/*�����˻صȴ�*/
#define SEND_REJECT		'5'	/*���˾ܾ�*/
#define SEND_CANCEL_ACCT	'6'	/*���˳�������*/	
#define SEND_CANCEL_WAIT	'7'	/*���˳����ȴ�*/
#define RECEIVE_REJECT		'8'	/*���˾ܾ� -ע��:���պ˶��ø�״̬��ʾ���ʳ��*/
#define RECEIVE_ACCT_QS		'9'	/*���˼�������*/
#define SEND_REVERSE_REG	'A'	/*���˵Ǽ�ȡ��*/
#define RECEIVE_BACK_ACCT	'B'	/*�����˻�/���з�����ѯ�Ѳ鸴*/
#define SEND_REGIST		'C'	/*���ʵǼ�(������)*/
#define SEND_REVERSE_ACCT	'D'	/*���˸���ȡ�� ������*/
#define RECEIVE_ACCT_REG	'E'	/*���˼��˵Ǽ�-���ʼǴ����ʱ��Ŀ,��ʱ����*/
#define RECEIVE_LMAC_ERR	'F'	/*���˺˵ط�Ѻ��,��ʱ��Ŀ�Լ��� --��֧��*/
#define RECEIVE_NMAC_ERR	'G'	/*���˺�ȫ��Ѻ��,��ʱ��Ŀ�Լ��� --��֧��*/
#define RECEIVE_DATA_ERR	'H'	/*�������ݲ������������Ĺ��� -���ջ���������*/
#define RECEIVE_CUSTNAME_ERR 'I' /*���˻������������ʺŲ����ڻ���������״̬��������,����*/
#define SEND_INQUEUE 		'J' /*�����Ŷ���*/
#define CHECK_QS 			'K' /* �˶�������� */
/**********����Ϊ�������ݺ˶�ר��״̬ *****************/
#define REJECT_EACTCLS 'L' /* �˶���������Ҫ�����ʻ�״̬��������� �������Ĺ��� */ 
/*********����Ϊ���ݺ˶ԱȽϽ��״̬ *******************/
#define NOTFOUND_CCPC_1      '1'  /* ccpcû�е����гɹ����ʵĳɹ�����״̬��� */
#define NOTFOUND_CCPC_2      '2'  /* ccpcû�е����гɹ����ʵ�֧�й��˳���״̬��� */
#define NOTFOUND_CCPC_3      '3'  /* ccpcû�е����гɹ����ʵ��������Ĺ��˳���״̬��� */
#define AMTDIFF_CCPC_BANK_1  '4'  /* ˫�������˵����׽�ͬ�ҳɹ����ʵ���� */
#define AMTDIFF_CCPC_BANK_2  '5'  /* ˫�������˵����׽�ͬ��֧�й��˳��ʵ���� */
#define AMTDIFF_CCPC_BANK_3  '6'  /* ˫�������˵����׽�ͬ���������Ĺ��˳��˵���� */
#define NOTFOUND_BANK_1      '7'  /* ccpc�е�����û�е����--�ɹ����� */
#define NOTFOUND_BANK_2      '8'  /* ccpc�е�����û�е����--֧�й��� */
#define NOTFOUND_BANK_3      '9'  /* ccpc�е�����û�е����--�������Ĺ��� */
#define CCPC_SAME_BANK       'A'  /* ���к�CCPC��ͬ */
#define NOTFOUND_CCPC        'B'  /* ccpcû�е����гɹ����ʵ���� */
#define AMTDIFF_CCPC_BANK    'C'  /* ˫�������˵����׽�ͬ����� */
#define NOTFOUND_BANK        'D'  /* ccpc�е�����û�е���� */
/**********************��������****************/
#define OP_QUERY	  '0'
#define OP_SEND		  '1'
#define HV_CZ_QSBRNO  "10001" /*���������к�*/
#define HV_HPCENTER   "904290099992" /*��Ʊ��������*/
/*********************Ӫҵ״̬***************************/
#define HV_STAT_PREPARE		'0' 	/*Ӫҵ׼��*/
#define HV_STAT_WORK		'1'		/*�ռ䴦��*/
#define HV_STAT_WARN		'2'		/*����׼��*/
#define HV_STAT_STOP		'3'		/*ҵ�����*/
#define HV_STAT_QSWIN		'4'		/*���㴰��*/
#define HV_STAT_WORKEND		'5'		/*���մ���ʼ*/
#define HV_STAT_CHECKERR	'6'		/*���ն��˲�ƽ*/
#define HV_STAT_CHECKRIGHT	'7'		/*���ն����Ѿ�ƽ*/
#define HV_STAT_UNKNOWN		'F'		/*δ֪״̬*/
#define HV_CHECK_MONEY     10000000 /* ����������С��Ȩ��� */
#define HV_HD_MIN_MONEY_INIT 10000000
extern  int HV_HD_MIN_MONEY;
#define HV_SYSPARA_POS  16
/*********************************************************/
#define   HV_EISBRNO    "040142"
#define   HV_QSBRNO HV_QS_BR_NO
#define   HV_SENDCO "1610"	/****TODO:Ӱ�����****/
/**********************************************************/
/***********Ӧ������***************************************/
#define   HV_RP_AGR         '0'    /*ͬ��Ӧ��*/
#define   HV_RP_DIS         '1'    /*��ͬ��Ӧ��*/
/**********��Ʊ����״̬************************************/
#define   HV_HP_REG         '0'    /* ��Ʊ�Ǽ� */
#define   HV_HP_SIGN        '1'    /* ��Ʊǩ��(����) */
#define   HV_HP_MV_CENTER   '2'    /* �ʽ��ƴ���� */
#define   HV_HP_ASKCASH     '3'    /* ����Ҹ� */
#define   HV_HP_CASH        '4'    /* ��Ʊ�Ҹ� */
#define   HV_HP_JQ          '5'    /* ȫ����� */
#define   HV_HP_PARTBACK    '6'    /* �����˻� */
#define   HV_HP_FULLBACK    '7'    /* δ���˻� */
#define   HV_HP_TOCUST      '8'    /* ��Ʊ�����⸶ */
#define   HV_HP_UNSIGN      '9'    /* ��Ʊǩ��ȡ�� */
#define   HV_HP_UNTOCUST    'A'    /* �⸶ȡ�� */
#define   HV_HP_SEND        'B'    /* ���� */
#define   HV_HP_UNCASH      'C'    /* ����Ҹ�ȡ�� --����Ҫ���״̬ */
#define   HV_HP_UNREG       'D'    /* ��Ʊ�Ǽ�ȡ�� */
#define   HV_HP_LOSS        'E'    /* ��ʧ */
#define   HV_HP_UNLOSS      'F'    /* ��� -- Ҳ����Ҫ��һ����Ҿ�����Ϊ�ϴ�״̬*/
#define   HV_HP_OVERDATE_UNTREAD    'G'    /* ����δ���˻� */
#define   HV_HP_UNUSED_TOCUST       'H'    /* δ�ý⸶ */
#define   HV_HP_UNUSED_FULLBACK     'I'    /* δ���˻س��� */
#define   HV_HP_CANCEL              'J'    /* ��Ʊ����ע�� */
#define   HV_HP_ASKRETURN           'K'    /* �����˻� */
#define HV_POLEN     8    /* ��Ʊ�ų��� */
/************** ��Ʊ���� ********************/
#define ATTORNED_PO   '0'  /* ��ת�õĻ�Ʊ */
#define UNATTORNED_PO '1'  /* ����ת�õĻ�Ʊ */
#define CASH_PO       '2'  /* �ֽ��Ʊ */
#endif



/***********     ����Ϊԭ mbfe_pub.h ������      *********/
/*********************   �궨��   ************************/
#ifndef MBFE_PUB
#define MBFE_PUB 

#define MBFE_CMTNO 			"0160"
#define MBFE_PAYPRILEVEL 	"0160"
#define MBFE_COMSNO 		"0160"
#define MBFE_CNAPS_DATE 	"0160"
#define MBFE_CMT_LEN 		"0160"
#define MBFE_OPER_TYPE 		"0160"
#define MBFE_OPER_STSCODE 	"0160"
#define MBFE_RESPNO 		"0160"
#define MBFE_SEND_TIME 		"0160"
#define MBFE_CMT_FILE 		"0160"
#define MBFE_CMT_NO 		"0160"
#define MBFE_O_CMT_NO 		"0160"
#define MBFE_O_OPER_STSCODE "0160"
#define MBFE_DEF_TIMEOUT 	120

/*******���������־�궨�� MBFEERRDEAL��MBFEWRITEMSG*********/ 
/* ���֧��������궨�� */
#define MBFEERRDEAL \
		strcpy(g_pub_tx.reply,"MBFE"); \
		set_zd_data(DC_GET_MSG,acErrMsg); \
		WRITEMSG \
		return -1;

#define MBFEWRITEMSG \
		strcpy(g_pub_tx.reply,"MBFE"); \
		set_zd_data(DC_GET_MSG,acErrMsg); \
		WRITEMSG

#endif

#endif


char * cTmpfp;
char * cTmpfp1; 
