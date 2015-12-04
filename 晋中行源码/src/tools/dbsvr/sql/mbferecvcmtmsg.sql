drop index mbferecvcmtmsg_idx;
drop  table mbferecvcmtmsg;
create table mbferecvcmtmsg(
	cmt_len number(9),
	cmt_use varchar2(1),
	srflag varchar2(1),
	oper_type varchar2(1),
	oper_syscode varchar2(3),
	comsno varchar2(8),
	respno varchar2(20),
	sendtime varchar2(14),
	cmtno varchar2(3),
	cnapsdate number(9),
	recvtime varchar2(14) 
)tablespace users;
create	 unique index mbferecvcmtmsg_idx on mbferecvcmtmsg( comsno,cnapsdate)tablespace indx;
