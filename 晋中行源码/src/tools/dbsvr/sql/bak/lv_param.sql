/****С������������Ʊ�****/
drop index lv_param_idx;
drop table lv_param;
create table lv_param(
   	par_code  	Varchar2(32) , /** ��������	 **/
   	par_name  	Varchar2(60) , /** ��������	 **/
	par_value    Varchar2(250) , /** ��������	 **/
	filler     Varchar2(60) /**��ע***/
)tablespace users;
create unique index lv_param_idx on lv_param(par_code);
