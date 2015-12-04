drop index mbfebrbkno_idx1;
drop index mbfebrbkno_idx3;
drop  table mbfebrbkno;
create table mbfebrbkno(
	seq_no varchar2(2),
	brno varchar2(5),
	bkno varchar2(12) 
)tablespace users;
create	 unique index mbfebrbkno_idx1 on mbfebrbkno( seq_no)tablespace indx;
create	 unique index mbfebrbkno_idx3 on mbfebrbkno( bkno)tablespace indx;
