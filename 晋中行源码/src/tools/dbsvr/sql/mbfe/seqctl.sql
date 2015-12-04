 /****序号控制挡****/ 
drop index seqctl_idx;
drop table seqctl;
create table seqctl(
	br_no       char(12),/*** 交易行行号 ******/
	ac_ind      char(2), /****序号类型,99-大额支付交易往帐序号， 
			98-大额支付交易参考号 97 大额附加域序号 
			96-小额支付交易序号,95 小额支付交易参考号
			94-小额支付交易附加域 ******/
	seqno       number(10)	/**** 序号顺序号 ****/
)tablespace users;
create unique index seqctl_idx on seqctl (br_no,ac_ind) tablespace indx;
