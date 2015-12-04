drop index march_info_1;
drop  table march_info;
create table march_info(
	mrchno varchar2(15),
	mrchnm varchar2(40),
	br_no varchar2(5),
	mrchtype varchar2(2),
	mrchstat varchar2(1),
	mrchacno varchar2(24),
	mrchacno_ls varchar(24),
	mrchaddr varchar2(40),
	mrchtel varchar2(16),
	mrchman varchar2(10),
	chrg number(8,6),
	reg_date number(9),
	beg_date number(9),
	end_date number(9) 
)tablespace users;
create	 unique index march_info_1 on march_info( mrchno)tablespace indx;
