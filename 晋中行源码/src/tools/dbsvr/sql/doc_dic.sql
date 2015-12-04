drop  table doc_dic;
create table doc_dic(
	seqn number(6),
	tab varchar2(20),
	fied varchar2(20),
	type varchar2(8),
	leng number(6),
	dec number(6),
	dic_code varchar2(20),
	mo varchar2(20),
	list varchar2(100) 
)tablespace users;
