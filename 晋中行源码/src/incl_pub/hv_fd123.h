#ifndef ISO8583__FD123__H__
#define ISO8583__FD123__H__
		 

#define HV_FD123_AREA_BLOCK \
	char  cmtno           [ 3];/*3*cmt���**/	\	
	char  o_cmtno         [ 3];/*6*ԭcmt���**/\
	char  pay_qs_no       [12];/*18*(ԭ)�����������к�**/		\
	char  or_br_no        [12];/*30*�����к� ԭ�����к� 		\
                           ���볷���к� �˻��к�**/	\
	char  sendco          [ 4];/*34*�������Ĵ���**/\
	char  pay_opn_br_no   [12];/*46*�����˿�����**/\
	char  pay_ac_no       [32];/*78*�������ʺ�(ԭ�տ����ʺ�)**/	\
	char  pay_name        [60];/*138*����������(ԭ�տ�������)**/	\
	char  pay_addr        [60];/*198*�����˵�ַ(ԭ�տ��˵�ַ)**/	\
	char  cash_qs_no      [12];/*210*���������к�**/\
	char  ac_br_no        [12];/*222*�����к�(ԭ�����к�)**/	\
	char  receco          [ 4];/*226*�ձ����Ĵ���**/\
	char  cash_opn_br_no  [12];/*238*�տ��˿����к�**/      \
	char  cash_ac_no      [32];/*270*�տ����ʺ�,ԭ�������ʺ�**/	\
	char  cash_name       [60];/*330*�տ�������,ԭ����������**/	\
	char  cash_addr       [60];/*390*�տ��˵�ַ**/\
	char  yw_type         [ 2];/*392*ҵ������**/	\
	char  orderno         [ 8];/*400������� ����֧���������*/	\
	char  brf             [60];/*460*����**/            \
	char  rate            [ 7];/*467*�������**/	\
	char  ratelimit       [ 5];/*472*�������**/	\
	char  o_orderno       [ 8];/*480*ԭ֧���������**/      \
	char  procode         [ 8];/*488*������**/	\
	char  cr_date         [ 8];/*496*�鸴����**/	\
	char  cr_br_no        [12];/*508*�鸴�к�**/	\
	char  cr_tx_num       [ 8];/*516*�鸴���**/\
	char  qr_br_no        [12];/*528*��ѯ�к�  ԭ��ѯ�к�**/	\
	char  qr_tx_num       [ 8] ;/*536*ԭ��ѯ��� ��ѯ���**/	\
	char  tx_type         [ 1];/*537*ԭ��������**/\
	char  notpay_content [255];/*792���� ��֧��ʱ���õ�**/		\
	char  notpay_orderno  [ 3];/*795���������   �˻������**/	\
	char  o_notpay_orderno[ 3];/*798ԭ���������  ԭ�˻������**/\	
	char  resp            [ 1];/*799����Ӧ�� �˻�Ӧ��**/		\
	char  operlevel       [ 1];/*800���ȼ���**/ \
	char  proc_sts        [ 1];/*801����״̬**/ \
	char  opcd            [ 2];/*803���״�������С���Ϊ׼ */ \
	char  lw_ind          [1 ];/*804������ʶ 1��,2�� */ \
	char  hp_add_pwd      [1 ];/* ��Ʊ�Ƿ��Ѻ */ \
	char  packid          [8 ];/* ����� */ \
	char  o_packid        [8 ];/* ԭ����� */ \
	char  pack_date       [8 ];/* ������ */ \
	char  o_pack_date     [8 ];/* ԭ������ */ \
	char  txnum           [5 ];/* ҵ�����ͱ�� */ \
	char  o_txnum         [5 ];/* ԭҵ�����ͱ�� */ \
	char  lv_yw_ind       [12];/* С��ҵ�������,2λ��Ż���12λ���� */ \
	char  resp_date       [8 ];/* Ӧ������ */ \
	char  rcpstat         [2 ];/* ҵ���ִ״̬ */ \
	char  resp2           [8 ];/* ccpc���״������ */ \
	char  retcode         [2 ];/* �˻�ԭ�� */ \
	char  ywdtlid         [8 ];/* ��ϸ��� */ \
	char  actype          [ 1];/* ʵʱҵ�񸶿����ʻ����� */ \
	char  reqtype         [ 1];/* �������� 0������	1������ */ \
	char  df_chrg_amt     [16];/* �Է������� */ \
	char  prot_no         [60];/* �෽Э��� */ \
	char  po_no           [8 ];/* ��Ʊ���� */ \
	char  po_type         [1 ];/* ��Ʊ����0��ת 1 ����ת 2�ֽ� */ \
	char  otype           [ 1];/* ��С���־ 0��1С�� */

typedef struct{
	HV_FD123_AREA_BLOCK
}HV_FD123_AREA;


#endif
