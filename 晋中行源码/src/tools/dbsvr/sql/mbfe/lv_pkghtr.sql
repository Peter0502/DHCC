drop index lv_pkghtr_idx;
drop table lv_pkghtr;
create table lv_pkghtr(
  pkgno           varchar2(3) ,/*Pkg���           */
  jz_date         number(8)   ,/*��������          */
  in_date         number(8)   ,/*����ϵͳ���� �ۺ�ϵͳ����*/
  wt_date         number(8)   ,/*ί������С��ϵͳ���� */
  pack_date       number(8)   ,/*�������С��ϵͳ���� */
  orderno         varchar2(8) ,/*ҵ����ȫ�����򣬼���֧���������*/
  tx_time         number(9)   ,/*����ʱ��yyyymmddhhmmss�������һ��*/
  txnum           varchar2(5) ,/*ҵ�����ͱ��      */
  or_br_no        varchar2(12),/*�������к�        */
  pay_qs_no       varchar2(12),/*�����������к�    */
  sendco          varchar2(4) ,/*��������          */
  ac_br_no        varchar2(12),/*�������к�        */
  cash_qs_no      varchar2(12),/*�����������к�    */
  receco          varchar2(4) ,/*�ձ�����          */
  tx_amt          number(17,2),/*���׽��          */
  pay_opn_br_no   varchar2(12),/*�����˿�����      */
  pay_ac_no       varchar2(32),/*�������ʺ�        */
  pay_name        varchar2(60),/*����������        */
  pay_addr        varchar2(60),/*�����˵�ַ        */
  cash_opn_br_no  varchar2(12),/*�տ��˿�����      */
  cash_ac_no      varchar2(32),/*�տ����ʺ�        */
  cash_name       varchar2(60),/*�տ�������        */
  cash_addr       varchar2(60),/*�տ��˵�ַ        */
  lv_yw_ind       varchar2(12),/*ҵ��������Ϊ2λ���룬����Ϊ12λ����*/
  lv_brf          varchar2(60),/*����              */
  resp_date       number(8)   ,/*Ӧ������          */
  rcpstat         varchar2(2) ,/*ҵ���ִ״̬      */
  resp1           varchar2(8) ,/*MbfeӦ����        */
  resp2           varchar2(8) ,/*CCPCӦ����        */
  note_type       varchar2(2) ,/*ƾ֤����          */
  note_no         varchar2(12),/*ƾ֤����          */
  addid           varchar(8)  ,/*��������id        */
  br_no           varchar2(7) ,/*���׻���          */
  input_tel       varchar2(6) ,/*¼�����Ա��      */
  check_tel       varchar2(6) ,/*���˲���Ա��      */
  auth_tel        varchar2(6) ,/*��Ȩ����Ա��      */
  send_tel        varchar2(6) ,/*���Ͳ���Ա��      */
  resp_tel        varchar2(6) ,/*Ӧ�����Ա��      */
  deal_tel        varchar2(6) ,/* ���ʴ������Ա */
  input_trace_no  varchar2(6) ,/*¼����ˮ��        */
  check_trace_no  varchar2(6) ,/*������ˮ��        */
  send_trace_no   varchar2(6) ,/*������ˮ��        */
  resp_trace_no   varchar2(6) ,/*Ӧ����ˮ��        */
  deal_trace_no   varchar2(6) ,/* ���ʴ������Ա  */
  lv_sts          varchar2(1) ,/*����״̬          */
  beg_sts         Varchar2(1) ,/*ԭʼ����״̬      */
  operlevel       varchar2(1) ,/*���ȼ�            */
  tx_chrg_ind     varchar2(1) ,/*�����ѱ�־        */
  chrg_amt        number(17,2),/*�����ѽ��        */
  checkflag       varchar2(1) ,/*���˱�־          */
  brprtflag       varchar2(64) ,/*֧�д�ӡ��־      */
  qsprtflag       varchar2(64) ,/*�������Ĵ�ӡ��־  */
  packid          varchar2(8) ,/*������id          */
  retcode         varchar2(2) ,/*�˻�ԭ��          */
  o_txnum          varchar2(5) ,/*ԭҵ�����ͱ��    */
  ywdtlid         varchar2(8) ,/*ҵ�����          */
  protno          varchar2(60),/*�෽Э���        */
  ac_type         varchar2(1) ,/*�ʻ�����0=�����ʺ�1�����ǿ�2�����ÿ�9������*/
  refno           varchar2(20),/*�ο���¼���ʱ�����*/
  o_or_br_no      varchar2(12),/*ԭ�����к�        */
  o_wt_date       number(8)   ,/*ԭί������        */
  o_pack_date     number(8)   ,/*ԭ��ί������      */
  o_orderno       varchar2(12),/*ԭ�������        */
  o_packid        varchar2(8) ,/*ԭ�����          */
  o_ac_br_no      varchar2(12),/* ԭ�������к�     */
  df_chrg_amt     number(16,2),/*ʵʱҵ��Է�������*/
  lv_o_sts        varchar2(1) ,/*ԭʼ���ʽ���״̬�˻㽻����*/     
  lw_ind          varchar2(1) ,/*������ʶ 1��2��   */
  cur_no	  varchar2(3) ,/*����		   */
  filler          varchar2(60)/*��ע              */
)tablespace users;
create unique index lv_pkghtr_idx on lv_pkghtr(or_br_no,pkgno,wt_date,orderno,lw_ind)  tablespace indx;

