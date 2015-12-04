drop table draw_loan_limit;
drop index draw_loan_limit_idx;
create table draw_loan_limit(
pact_no 	varchar2(20),	/**贷款借据号                    **/                         	
ac_no 		varchar2(19),	/**存款账号           		 **/
ac_id 		number(9),	/**存款账户id         		 **/
save_amt	decimal(16,2),	/**放款金额           		 **/
trust_amt    	decimal(16,2),	/**委托支付金额       		 **/
self_amt     	decimal(16,2),	/**自主支付金额       		 **/
trust_draw_amt	decimal(16,2),	/**已支取委托支付金额		 **/
self_draw_amt	decimal(16,2),	/**已支取自主支付金额		 **/
sts          	varchar2(1),	/**状态	X:信贷审批完成
				1.开户待发放 2.已发放 *.支取完结 **/
br_no        	varchar2(5),	/**审批机构 		         **/
tel          	varchar2(6),	/**审批柜员 		         **/
tx_date      	number(9),	/**签发日期 		         **/
brf1         	varchar2(60),	/**备注1              		 **/
brf2         	varchar2(60)	/**备注2              		 **/
)tablespace users;
create unique index draw_loan_limit_idx on draw_loan_limit (pact_no) tablespace indx;
