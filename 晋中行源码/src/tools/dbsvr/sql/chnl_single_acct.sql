drop index chnl_single_acct_1;
drop table chnl_single_acct;
create table chnl_single_acct(
Tx_date	Number(8),
tx_time	Varchar2(6),
Trace_no	Number(6),
Cur_no	Varchar2(2),
Ac_no	Varchar2(32),
Ac_no1	Varchar2(32),
Ac_no2	Varchar2(32),
New_ac_no2	Varchar2(32),
New_ac_name2	Varchar2(60),
G_ac_no2	Varchar2(32),
Tx_code	Varchar2(4),
chnl_type Varchar2(1),  --2012年9月新增加的字段 渠道类型
Tx_br_no	Varchar2(5),
Opn_br_no	Varchar2(5),
Tel	Varchar2(6),
bbmk	Varchar2(4),
Ttyn	Varchar2(20),
Pt_date	Number(8),
Pt_time	Varchar2(6),
Pt_trace_no	Varchar2(12),
Tx_amt	Number(17,2),
Th_amt  Number(17,2),
Ct_ind	Varchar2(1),
Brf	Varchar2(30),
Fee_amt	Number(17,2),
Sts	Varchar2(1),
O_tx_date	Number(8),
O_trace_no	Varchar2(12),
O_pt_date	Number(8),
O_pt_trace_no	Varchar2(12),
Dz_ind	Varchar2(1),
Dz_ref	Varchar2(28)
)tablespace users;
create unique index chnl_single_acct_1 on chnl_single_acct (bbmk,pt_date,pt_trace_no,Tx_date,trace_no);
