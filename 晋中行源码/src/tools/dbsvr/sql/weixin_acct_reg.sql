drop index weixin_acct_reg_1;
drop table weixin_acct_reg;
create table weixin_acct_reg(
	filename  varchar2(50),
	acct_date number(9),
	acct_time number(9),
	sts       varchar2(1),
	acct_seqn number(6),
	key1      varchar2(16),
	key2      varchar2(16),
	filler    varchar2(60)
)tablespace users;
create unique index weixin_acct_reg_1 on weixin_acct_reg(filename) tablespace indx;
