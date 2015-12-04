drop index ic_subac_parm_1;
drop  table ic_subac_parm;
create table ic_subac_parm(
	sub_seqn number(4),
	sub_name varchar2(30),
	main_prdt_code varchar2(3),
	app_prdt_code varchar2(3),
	sts varchar2(1),
	tagval varchar2(10),
	filler varchar2(30)
)tablespace users;
create	 unique index ic_subac_parm_1 on ic_subac_parm(sub_seqn)tablespace indx;