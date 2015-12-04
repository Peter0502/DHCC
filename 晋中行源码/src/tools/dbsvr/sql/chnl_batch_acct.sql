drop  index chnl_batch_acct_1;
drop  index chnl_batch_acct_2;
drop  table chnl_batch_acct;
create table chnl_batch_acct
(
tx_date	number(9)            ,
tx_time	varchar(6)          ,
batch_ref	varchar(60)       ,
dz_ref	varchar(32)         ,
trace_no	number(9)          ,
cur_no	varchar(2)          ,
ac_no	varchar(32)           ,
ac_no1	varchar(32)         ,
ac_no2	varchar(32)         ,
new_ac_no2	varchar(32)     ,
new_ac_name2	varchar(60)   ,
g_ac_no2	varchar(32)       ,
tx_code	varchar(4)          ,
tx_br_no	varchar(5)        ,
opn_br_no	varchar(5)        ,
tel	varchar(6)              ,
bbmk	varchar(4)            ,
ttyn	varchar(20)           ,
pt_date	number(9)            ,
pt_time	varchar(6)          ,
pt_trace_no	varchar(12)     ,
tx_amt	number(17,2)         ,
ct_ind	varchar(1)          ,
brf	varchar(30)             ,
fee_amt	number(17,2)         ,
sts	varchar(1)              ,
o_tx_date	number(9)          ,
o_trace_no	varchar(12)     ,
o_pt_date	number(9)          ,
o_pt_trace_no	varchar(12)   ,
dz_ind	varchar(1)
);
create unique index chnl_batch_acct_1 on chnl_batch_acct(batch_ref,dz_ref);
create index chnl_batch_acct_2 on chnl_batch_acct(tx_date,trace_no,bbmk,pt_date,pt_trace_no);


