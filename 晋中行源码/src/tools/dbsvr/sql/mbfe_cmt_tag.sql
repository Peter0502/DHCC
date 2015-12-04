drop index mbfe_cmt_tag_idx1;
drop index mbfe_cmt_tag_idx2;
drop  table mbfe_cmt_tag;
create table mbfe_cmt_tag(
	cmtno varchar2(3),
	tag_seqn number(9),
	tag varchar2(3),
	tag_maxlen number(9),
	tag_name varchar2(40),
	tag_data_type varchar2(2),
	null_flag varchar2(1),
	len1 number(9),
	field1 varchar2(20),
	tag_data_code1 varchar2(4),
	len2 number(9),
	field2 varchar2(20),
	tag_data_code2 varchar2(4) 
)tablespace users;
create	 unique index mbfe_cmt_tag_idx1 on mbfe_cmt_tag( cmtno,tag)tablespace indx;
create	 unique index mbfe_cmt_tag_idx2 on mbfe_cmt_tag( cmtno,tag_seqn)tablespace indx;
