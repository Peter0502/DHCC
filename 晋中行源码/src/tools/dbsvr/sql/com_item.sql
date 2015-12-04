drop index com_item_1;
drop  table com_item;
create table com_item(
	acc_no varchar2(7),
	acc_hrt varchar2(5),
	app_cod varchar2(5),
	up_acc_hrt varchar2(5),
	acc_name varchar2(30),
	sub_acc_yn varchar2(1),
	acc_lvl varchar2(1),
	dc_ind varchar2(1),
	roll_ind varchar2(1),
	acc_knd varchar2(1),
	fore_ind varchar2(1),
	equa_ind varchar2(1),
	amt_dc_ind varchar2(1),
	od_ind varchar2(1),
	in_ind varchar2(1),
	scope varchar2(1) 
)tablespace users;
create	 unique index com_item_1 on com_item( acc_hrt)tablespace indx;
create	  index com_item_2 on com_item( acc_no)tablespace indx;
