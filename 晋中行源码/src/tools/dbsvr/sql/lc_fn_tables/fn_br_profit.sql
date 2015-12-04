drop table fn_br_profit;
create table fn_br_profit
(
	prdt_code	varchar2(20),	/*理财产品代码				*/
	br_no		varchar2(5),	/*机构号码		                */
	tx_amt		number(16,2),	/*募集金额	本机构募集到的总额      */
	redempt_amt	number(16,2),	/*赎回金额		                */
	profit_amt	number(16,2),	/*分润金额		                */
	reg_date	number(8),	/*登记日期		                */
	reg_trace_no	number(9),	/*登记流水		                */
	pay_date	number(8),	/*付款日期		                */
	pay_trace_no	number(9),	/*付款流水		                */
	sts		varchar2(1),	/*状态		0:登记                  */
					/*		1：入账                 */
	brf		varchar2(20),	/*摘要		                        */
	filler		varchar2(60)	/*备注		                        */
)tablespace users;
create unique index fn_br_profit_1 on fn_br_profit(prdt_code,br_no) tablespace indx;
