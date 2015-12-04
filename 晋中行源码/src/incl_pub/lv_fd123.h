#define LV_FD123_AREA_BLOCK \
    char    crdb        [1];/**借贷标志0借1贷**/ \
    char    opcd        [2];/**操作代码  10－录入之录入11－录入之查询 \
                    20－修改之修改 21－修改之查询30－复核 40－发送之发送\
                    41－发送之查询 50－应答 60－接收70－错账处理之入帐 \
                    71－错账处理之修改72－错账处理之查询 80 - 冲来帐 90--冲往帐\
                    */ \
    char    pkgno          [3 ]    ;/**Package编号    **/ \
    char    orderno        [8 ]    ;/**支付交易序号   **/ \
    char    txnum          [5 ]    ;/**业务类型编号   **/ \
    char    or_br_no       [12]    ;/**发起行行号     **/ \
    char    ac_br_no       [12]    ;/**接收行行号     **/ \
    char    tx_amt         [15]    ;/**交易金额       **/ \
    char    pay_opn_br_no  [12]    ;/**付款人开户行   **/ \
    char    pay_qs_no      [12]    ;/**付款清算行行号 **/\
    char    pay_ac_no      [32]    ;/**付款人帐号     **/ \
    char    pay_name       [60]    ;/**付款人名称     **/ \
    char    pay_addr       [60]    ;/**付款人地址     **/ \
    char    pwd            [8]     ;/**储户密码       **/ \
    char    cash_qs_no     [12]    ;/**收款清算行行号 **/\
    char    cash_opn_br_no [12]    ;/**收款人开户行行号**/ \
    char    cash_ac_no     [32]    ;/**收款人帐号     **/ \
    char    cash_name      [60]    ;/**收款人名称     **/ \
    char    cash_addr      [60]    ;/**收款人地址     **/ \
    char    yw_type        [12]    ;/**业务种类       **/ \
    char    brf            [60]    ;/**附言           **/ \
    char    wt_date        [8]     ;/**委托日期       **/ \
    char    operlevel      [1]     ;/**优先级         **/ \
    char    auth_tel  [TLRLENGTH]  ;/**授权操作员号   **/ \
    char    auth_pwd       [8]     ;/**授权操作员密码 **/ \
    char    note_type      [2]     ;/**凭证种类       **/ \
    char    note_no        [12]    ;/**凭证号码       **/ \
    char    pack_date      [8]     ;/**包日期,定期用  **/ \
    char    pack_id        [8]     ;/**包id,定期用    **/ \
    char    chrg_ind       [1]     ;/**手续费标志     **/ \
    char    chrg_amt       [15]    ;/**手续费金额     **/ \
    char    resp_day       [8]     ;/**期待应答日期  **/ \
    char    po_type        [2]     ;/**票据类型      **/ \
    char    po_date        [8]     ;/**票据日期      **/ \
    char    po_num         [8]     ;/**票据号码      **/ \
    char    pc_tx_amt      [15]    ;/**赔偿金额      **/ \
    char    jf_tx_amt      [15]    ;/**拒付金额      **/ \
    char    o_wt_date      [8]     ;/**原委托日期    **/ \
    char    o_txnum        [5]     ;/**原业务编号    **/ \
    char    o_orderno      [8]     ;/**原支付交易序号**/ \
    char    ret_code       [2]     ;/**退汇原因代码  **/ \
    char    rcp_sts        [2]     ;/**业务回执状态  **/ \
    char    resp1          [8]     ;/**Mbfe应答码    **/ \
    char    resp2          [8]     ;/**Ccpc应答码    **/ \
    char    pack_end       [1]     ;/**包结束标志    **/ \
    char    lv_sts         [1]     ;/**交易状态      **/ \
    char    lw_ind         [1]     ;/**来往标识      **/ \
    char    dfxyno         [60]    ;/**多方协议号    **/ \
    char    o_or_br_no     [12]    ;/**原发起行行号  **/ \
    char    o_ac_br_no     [12]    ;/**原接收行行号  **/ \
    char    buff           [975-742];
typedef struct{
        LV_FD123_AREA_BLOCK
}LV_FD123_AREA;

#define LV_FD125_AREA_BLOCK \
	char      pack_date   [8 ];     /**包日期  **/\
	char      packid      [8 ];     /**包序号  **/\
	char      pkgno       [3 ];     /**包类型  **/\
	char      pay_qs_no   [12];     /**付款清算行**/\
	char      cash_qs_no  [12];     /**收款清算行**/\
	char      in_date     [8 ];     /**来帐日期**/\
	char      packmy      [40];     /**包密押  **/\
  char      dtlcnt      [5 ];     /**总笔数  **/\
	char      totamt      [16];     /**总金额  **/\
	char      zcnode      [1 ];     /**扎差节点**/\
	char      zc_date     [8 ];     /**扎差日期**/\
	char      zcid        [2 ];     /**扎差序号**/\
	char      hz_date     [8 ];     /**回执日期**/\
	char      o_pack_date [8 ];     /**原包日期**/\
	char      o_packid    [8 ];     /**原包序号**/\
	char      dtlsuccnt   [5 ];     /**明细笔数**/\
	char      dtlsucamt   [16];     /**明细金额**/\
	char      o_pkgno     [3 ];     /**原包类型**/\
	char      ornode      [12];     /**发起节点**/\
	char      acnode      [12];     /**接收节点**/\
	char      ffflag      [1 ];     /**这是什么**/\
	char      txnum       [5 ];     /**业务类型**/\
	char      bfflag      [2 ];     /**补发标志**/\
	char      qs_date     [8 ];     /**清算日期**/\
	char      packstat    [2 ];     /**人行状态**/\
	char      sts         [1 ];     /**我行状态**/\
	char      br_no       [7 ];     /**交易机构**/\
	char      filler      [60];     /**备    注**/
typedef struct{
    LV_FD125_AREA_BLOCK
}LV_FD125_AREA;
