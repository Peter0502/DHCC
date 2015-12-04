drop index weixin_acct_hst_1;
drop index weixin_acct_hst_2;
drop table weixin_acct_hst;
create table weixin_acct_hst(
	pt_trace_no number(6),
	acct_date   number(9),
	bussi_name  varchar2(60),
	bussi_ac_no varchar2(20),
	opn_br_no   varchar2(60),
	br_name    varchar2(60),
	tx_type     varchar2(60),
	tx_cnt      number(6),
	tx_amt      number(16,2),
	refud_cnt   number(6),
	refud_amt   number(16,2),
	retan_amt   number(16,2),
	bussi_rate  number(8,6),
	bussi_chrg  number(16,2),
	wx_chrg      number(16,2),
	bk_amt      number(16,2),
	wft_chrg    number(16,2),
	tx_bk_chrg  number(16,2),
	bussi_amt   number(16,2),
	hx_trace_no number(9),
	sts         varchar2(1),
	filename    varchar2(50),
	filler1     varchar2(30),
	filler2     varchar2(30),
	key1        varchar2(16),
	key2        varchar2(16)
)tablespace users;
create index weixin_acct_hst_1 on weixin_acct_hst(sts,pt_trace_no,hx_trace_no);
create index weixin_acct_hst_2 on weixin_acct_hst(bussi_name);

