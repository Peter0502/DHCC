drop  table mob_acct_type_dele;
create table mob_acct_type_dele(
	mobile varchar2(15),
	acct_no varchar2(20),
	server_type varchar2(4),
	trn_date varchar2(8),
	oper_no varchar2(4),
	sfbz varchar2(1),
	card_no varchar2(20),
	"number" number(9),
	no_mark varchar2(1) 
)tablespace users;
