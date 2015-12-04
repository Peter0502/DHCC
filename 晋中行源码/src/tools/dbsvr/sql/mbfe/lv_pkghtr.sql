drop index lv_pkghtr_idx;
drop table lv_pkghtr;
create table lv_pkghtr(
  pkgno           varchar2(3) ,/*Pkg编号           */
  jz_date         number(8)   ,/*记帐日期          */
  in_date         number(8)   ,/*进入系统日期 综合系统日期*/
  wt_date         number(8)   ,/*委托日期小额系统日期 */
  pack_date       number(8)   ,/*打包日期小额系统日期 */
  orderno         varchar2(8) ,/*业务编号全行排序，兼做支付交易序号*/
  tx_time         number(9)   ,/*交易时间yyyymmddhhmmss，保存第一次*/
  txnum           varchar2(5) ,/*业务类型编号      */
  or_br_no        varchar2(12),/*发起行行号        */
  pay_qs_no       varchar2(12),/*发起清算行行号    */
  sendco          varchar2(4) ,/*发报中心          */
  ac_br_no        varchar2(12),/*接收行行号        */
  cash_qs_no      varchar2(12),/*接收清算行行号    */
  receco          varchar2(4) ,/*收报中心          */
  tx_amt          number(17,2),/*交易金额          */
  pay_opn_br_no   varchar2(12),/*付款人开户行      */
  pay_ac_no       varchar2(32),/*付款人帐号        */
  pay_name        varchar2(60),/*付款人名称        */
  pay_addr        varchar2(60),/*付款人地址        */
  cash_opn_br_no  varchar2(12),/*收款人开户行      */
  cash_ac_no      varchar2(32),/*收款人帐号        */
  cash_name       varchar2(60),/*收款人名称        */
  cash_addr       varchar2(60),/*收款人地址        */
  lv_yw_ind       varchar2(12),/*业务种类电汇为2位代码，其他为12位描述*/
  lv_brf          varchar2(60),/*附言              */
  resp_date       number(8)   ,/*应答日期          */
  rcpstat         varchar2(2) ,/*业务回执状态      */
  resp1           varchar2(8) ,/*Mbfe应答码        */
  resp2           varchar2(8) ,/*CCPC应答码        */
  note_type       varchar2(2) ,/*凭证种类          */
  note_no         varchar2(12),/*凭证号码          */
  addid           varchar(8)  ,/*附加数据id        */
  br_no           varchar2(7) ,/*交易机构          */
  input_tel       varchar2(6) ,/*录入操作员号      */
  check_tel       varchar2(6) ,/*复核操作员号      */
  auth_tel        varchar2(6) ,/*授权操作员号      */
  send_tel        varchar2(6) ,/*发送操作员号      */
  resp_tel        varchar2(6) ,/*应答操作员号      */
  deal_tel        varchar2(6) ,/* 错帐处理操作员 */
  input_trace_no  varchar2(6) ,/*录入流水号        */
  check_trace_no  varchar2(6) ,/*复核流水号        */
  send_trace_no   varchar2(6) ,/*发送流水号        */
  resp_trace_no   varchar2(6) ,/*应答流水号        */
  deal_trace_no   varchar2(6) ,/* 错帐处理操作员  */
  lv_sts          varchar2(1) ,/*交易状态          */
  beg_sts         Varchar2(1) ,/*原始交易状态      */
  operlevel       varchar2(1) ,/*优先级            */
  tx_chrg_ind     varchar2(1) ,/*手续费标志        */
  chrg_amt        number(17,2),/*手续费金额        */
  checkflag       varchar2(1) ,/*对账标志          */
  brprtflag       varchar2(64) ,/*支行打印标志      */
  qsprtflag       varchar2(64) ,/*清算中心打印标志  */
  packid          varchar2(8) ,/*所属包id          */
  retcode         varchar2(2) ,/*退回原因          */
  o_txnum          varchar2(5) ,/*原业务类型编号    */
  ywdtlid         varchar2(8) ,/*业务序号          */
  protno          varchar2(60),/*多方协议号        */
  ac_type         varchar2(1) ,/*帐户类型0=银行帐号1＝贷记卡2＝信用卡9＝其他*/
  refno           varchar2(20),/*参考号录入的时候产生*/
  o_or_br_no      varchar2(12),/*原发起行号        */
  o_wt_date       number(8)   ,/*原委托日期        */
  o_pack_date     number(8)   ,/*原包委托日期      */
  o_orderno       varchar2(12),/*原交易序号        */
  o_packid        varchar2(8) ,/*原包序号          */
  o_ac_br_no      varchar2(12),/* 原接收行行号     */
  df_chrg_amt     number(16,2),/*实时业务对方手续费*/
  lv_o_sts        varchar2(1) ,/*原始来帐交易状态退汇交易用*/     
  lw_ind          varchar2(1) ,/*来往标识 1往2来   */
  cur_no	  varchar2(3) ,/*币种		   */
  filler          varchar2(60)/*备注              */
)tablespace users;
create unique index lv_pkghtr_idx on lv_pkghtr(or_br_no,pkgno,wt_date,orderno,lw_ind)  tablespace indx;

