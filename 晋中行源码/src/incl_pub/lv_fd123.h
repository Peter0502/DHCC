#define LV_FD123_AREA_BLOCK \
    char    crdb        [1];/**�����־0��1��**/ \
    char    opcd        [2];/**��������  10��¼��֮¼��11��¼��֮��ѯ \
                    20���޸�֮�޸� 21���޸�֮��ѯ30������ 40������֮����\
                    41������֮��ѯ 50��Ӧ�� 60������70�����˴���֮���� \
                    71�����˴���֮�޸�72�����˴���֮��ѯ 80 - ������ 90--������\
                    */ \
    char    pkgno          [3 ]    ;/**Package���    **/ \
    char    orderno        [8 ]    ;/**֧���������   **/ \
    char    txnum          [5 ]    ;/**ҵ�����ͱ��   **/ \
    char    or_br_no       [12]    ;/**�������к�     **/ \
    char    ac_br_no       [12]    ;/**�������к�     **/ \
    char    tx_amt         [15]    ;/**���׽��       **/ \
    char    pay_opn_br_no  [12]    ;/**�����˿�����   **/ \
    char    pay_qs_no      [12]    ;/**�����������к� **/\
    char    pay_ac_no      [32]    ;/**�������ʺ�     **/ \
    char    pay_name       [60]    ;/**����������     **/ \
    char    pay_addr       [60]    ;/**�����˵�ַ     **/ \
    char    pwd            [8]     ;/**��������       **/ \
    char    cash_qs_no     [12]    ;/**�տ��������к� **/\
    char    cash_opn_br_no [12]    ;/**�տ��˿������к�**/ \
    char    cash_ac_no     [32]    ;/**�տ����ʺ�     **/ \
    char    cash_name      [60]    ;/**�տ�������     **/ \
    char    cash_addr      [60]    ;/**�տ��˵�ַ     **/ \
    char    yw_type        [12]    ;/**ҵ������       **/ \
    char    brf            [60]    ;/**����           **/ \
    char    wt_date        [8]     ;/**ί������       **/ \
    char    operlevel      [1]     ;/**���ȼ�         **/ \
    char    auth_tel  [TLRLENGTH]  ;/**��Ȩ����Ա��   **/ \
    char    auth_pwd       [8]     ;/**��Ȩ����Ա���� **/ \
    char    note_type      [2]     ;/**ƾ֤����       **/ \
    char    note_no        [12]    ;/**ƾ֤����       **/ \
    char    pack_date      [8]     ;/**������,������  **/ \
    char    pack_id        [8]     ;/**��id,������    **/ \
    char    chrg_ind       [1]     ;/**�����ѱ�־     **/ \
    char    chrg_amt       [15]    ;/**�����ѽ��     **/ \
    char    resp_day       [8]     ;/**�ڴ�Ӧ������  **/ \
    char    po_type        [2]     ;/**Ʊ������      **/ \
    char    po_date        [8]     ;/**Ʊ������      **/ \
    char    po_num         [8]     ;/**Ʊ�ݺ���      **/ \
    char    pc_tx_amt      [15]    ;/**�⳥���      **/ \
    char    jf_tx_amt      [15]    ;/**�ܸ����      **/ \
    char    o_wt_date      [8]     ;/**ԭί������    **/ \
    char    o_txnum        [5]     ;/**ԭҵ����    **/ \
    char    o_orderno      [8]     ;/**ԭ֧���������**/ \
    char    ret_code       [2]     ;/**�˻�ԭ�����  **/ \
    char    rcp_sts        [2]     ;/**ҵ���ִ״̬  **/ \
    char    resp1          [8]     ;/**MbfeӦ����    **/ \
    char    resp2          [8]     ;/**CcpcӦ����    **/ \
    char    pack_end       [1]     ;/**��������־    **/ \
    char    lv_sts         [1]     ;/**����״̬      **/ \
    char    lw_ind         [1]     ;/**������ʶ      **/ \
    char    dfxyno         [60]    ;/**�෽Э���    **/ \
    char    o_or_br_no     [12]    ;/**ԭ�������к�  **/ \
    char    o_ac_br_no     [12]    ;/**ԭ�������к�  **/ \
    char    buff           [975-742];
typedef struct{
        LV_FD123_AREA_BLOCK
}LV_FD123_AREA;

#define LV_FD125_AREA_BLOCK \
	char      pack_date   [8 ];     /**������  **/\
	char      packid      [8 ];     /**�����  **/\
	char      pkgno       [3 ];     /**������  **/\
	char      pay_qs_no   [12];     /**����������**/\
	char      cash_qs_no  [12];     /**�տ�������**/\
	char      in_date     [8 ];     /**��������**/\
	char      packmy      [40];     /**����Ѻ  **/\
  char      dtlcnt      [5 ];     /**�ܱ���  **/\
	char      totamt      [16];     /**�ܽ��  **/\
	char      zcnode      [1 ];     /**����ڵ�**/\
	char      zc_date     [8 ];     /**��������**/\
	char      zcid        [2 ];     /**�������**/\
	char      hz_date     [8 ];     /**��ִ����**/\
	char      o_pack_date [8 ];     /**ԭ������**/\
	char      o_packid    [8 ];     /**ԭ�����**/\
	char      dtlsuccnt   [5 ];     /**��ϸ����**/\
	char      dtlsucamt   [16];     /**��ϸ���**/\
	char      o_pkgno     [3 ];     /**ԭ������**/\
	char      ornode      [12];     /**����ڵ�**/\
	char      acnode      [12];     /**���սڵ�**/\
	char      ffflag      [1 ];     /**����ʲô**/\
	char      txnum       [5 ];     /**ҵ������**/\
	char      bfflag      [2 ];     /**������־**/\
	char      qs_date     [8 ];     /**��������**/\
	char      packstat    [2 ];     /**����״̬**/\
	char      sts         [1 ];     /**����״̬**/\
	char      br_no       [7 ];     /**���׻���**/\
	char      filler      [60];     /**��    ע**/
typedef struct{
    LV_FD125_AREA_BLOCK
}LV_FD125_AREA;
