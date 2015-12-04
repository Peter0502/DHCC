drop table redempt_rate;
create table redempt_rate
(
	prdt_code	varchar2(20),	/*产品代码				*/
    rate_code   varchar2(3),  /*利率编号001赎回利率002收益率*/
	beg_date	number(8),	/*启用起始日期		                */
	end_date	number(8),	/*结束日期		默认99999999    */
	min_days	number(8),	/*起始天数		包含            */
	max_days	number(8),	/*终止天数		不包含          */
	redempt_rate	number(9,6),	/*利率		   */
	profit_rate	number(9,6)	/* 分润利率 */
)tablespace users;
create unique index redempt_rate_1 on redempt_rate(prdt_code,rate_code,beg_date,end_date,min_days,max_days) tablespace indx;
