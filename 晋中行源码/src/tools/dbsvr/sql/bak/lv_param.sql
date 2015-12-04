/****小额批量服务控制表****/
drop index lv_param_idx;
drop table lv_param;
create table lv_param(
   	par_code  	Varchar2(32) , /** 参数代码	 **/
   	par_name  	Varchar2(60) , /** 参数名称	 **/
	par_value    Varchar2(250) , /** 参数内容	 **/
	filler     Varchar2(60) /**备注***/
)tablespace users;
create unique index lv_param_idx on lv_param(par_code);
