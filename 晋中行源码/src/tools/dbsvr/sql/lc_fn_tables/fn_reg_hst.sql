drop table fn_reg_hst;
create table fn_reg_hst
(
	tx_date		number(8),	/*交易日期				*/	
	trace_no	number(9),	/*主机流水号		                */
	tx_br_no	varchar2(5),	/*交易机构		                */
	tel		varchar2(6),	/*交易柜员		                */
	type		varchar2(1),	/*交易类别		0：认购交易     */
					/*			1：扣本金交易   */
					/*			2：分配收益交易 */
					/*			3：募集失败交易 */
					/*			4:申购交易      */
					/*			5:申购确认交易  */
					/*			6:赎回交易      */
					/*			*：撤销交易     */
	tx_amt		number(16,2),	/*交易金额		                */
	agent_id_type	varchar2(1),	/*代理人证类型		                */
	agent_id	varchar2(20),	/*代理人证件号		                */
	agent_name	varchar2(60),	/*代理人姓名		                */
	buy_date	number(8),	/*认购日期		                */
	buy_trace_no	number(9),	/*认购流水		                */
	brf		varchar2(20),	/*摘要		                        */
	filler		varchar2(60)	/*备注		                        */
)tablespace users;
create index fn_reg_hst_1 on fn_reg_hst(tx_date,trace_no) tablespace indx;
create index fn_reg_hst_2 on fn_reg_hst(tx_date,type)tablespace indx;
