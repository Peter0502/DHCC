	/**小额系统控制表**/
drop table lv_sysctl;
create table lv_sysctl(
   	tx_date     number(8) , /*交易日期*/ 
	stat        varchar2(1) , /*交易状态*/ 
	next_date   number(8) , /*下一个工作日期*/
	chgnode     varchar2(4) , /*变更节点*/  
	curstat     varchar2(8) , /*营业状态*/  
	chgtime     varchar2(14), /*变更时间*/ 
	message	    varchar2(60), /*更新描述*/  
	filler	    varchar2(100) /*备注*/  
)tablespace users;
