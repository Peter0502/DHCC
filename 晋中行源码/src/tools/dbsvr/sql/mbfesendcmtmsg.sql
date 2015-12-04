drop index mbfesendcmtmsg_idx;
drop  table mbfesendcmtmsg;
create table mbfesendcmtmsg(
	oper_type varchar2(1),
	comsno varchar2(8),
	respno varchar2(20),
	sendtime varchar2(14),
	cmtno varchar2(3),
	cnapsdate number(9),
	msssno varchar2(8),
	bkcode varchar2(12),
	origrtnappdate number(9) 
)tablespace users;
create	 unique index mbfesendcmtmsg_idx on mbfesendcmtmsg( comsno,cnapsdate)tablespace indx;
