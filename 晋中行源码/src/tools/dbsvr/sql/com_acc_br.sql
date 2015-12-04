drop index com_acc_br_1;
drop  table com_acc_br;
create table com_acc_br(
	acc_hrt varchar2(5),
	br_no varchar2(5),
	ind varchar2(10) 
)tablespace users;
create	 unique index com_acc_br_1 on com_acc_br( acc_hrt)tablespace indx;
