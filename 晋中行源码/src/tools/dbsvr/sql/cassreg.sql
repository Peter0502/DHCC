/* 3.1.29担保登记簿 	cassreg*/
drop index  cassreg_idx; 
drop table cassreg;
create table cassreg(
    br_no	Varchar2(5)	,/*机构编码	*/
    ac_id 	number(9)	,/*担保帐户	*/
    name	Varchar2(60),	/*担保单位户名	*/
    pact_no	Varchar2(20)	,/*借据号	*/
    bw_ac_id	number(9)	,/* 担保单位客户号 */
    in_tx_date	number(8)	,/*担保日期	*/
    out_tx_date	number(8)	,/*解除担保日期	*/
    amt		Number(16,2)	,/*担保金额	*/
    bal		Number(16,2)	,/*担保户当时余额*/
    tel  	Varchar2(4)	,/*操作员	*/
    flag  	Varchar2(1)	,/* 0建立  1解除 */
    in_trace_no	number(6),	/* 建立时流水号 */
    out_trace_no	number(6)	/* 解除时流水号 */
)tablespace users;
create index cassreg_idx on cassreg(pact_no) tablespace indx;
