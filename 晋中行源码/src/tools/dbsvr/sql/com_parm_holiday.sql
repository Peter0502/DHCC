drop  table com_parm_holiday;
create table com_parm_holiday(
        date_code varchar2(3), /**节假日代码**/
        date_name varchar2(10),/**节假日名称**/
	beg_date number(8),    /**起始日期**/
	end_date number(8),    /**终止日期**/
	days number(6),        /**总共天数**/
	type varchar2(1),      /**是否生效0:未生效 1:已生效**/
	filler varchar2(60)    /**备    注**/
)tablespace users;
