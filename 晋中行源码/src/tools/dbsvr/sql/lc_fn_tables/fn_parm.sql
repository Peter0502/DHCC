drop table fn_parm; 
create table fn_parm
(
	prdt_code	varchar2(20),	/*产品代码								*/	
	title		varchar2(60),	/*产品名称		                                                */
	prdt_type	varchar2(1),	/*产品类型		1封闭式                                         */
					/*			2开放式                                         */
	intst_type	varchar2(1),	/*收益类型		1保本固定收益                                   */
					/*			2保本浮动收益                                   */
					/*			3不保本                                         */
	beg_date	number(8),	/*募集起始日期		                                                */
	end_date	number(8),	/*募集结束日期		该日扣款                                        */
	ic_date		number(8),	/*产品成立日期		募集结束日的第二天                              */
	mtr_date	number(8),	/*产品到期日期		产品到期日期                                    */
	return_date	number(8),	/*归还收益日期		返还本金和收益日期                              */
	term_type	varchar2(1),	/*期限类型		D日，M月，Y年                                   */
	term		number(8),	/*期限		                                                        */
	cif_type	varchar2(1),	/*客户类型		0.个人单位皆可；1.个人；2.对公                  */
	cur_no		varchar2(2),	/*币种		01.人民币                                               */
	chnl_type	varchar2(4),	/*渠道类型		限制购买渠道                                    */
					/*			**：所有渠道                                    */
					/*			0100：柜面                                        */
					/*			0400：网银                                        */
	risk_lvl	varchar2(1),	/*风险等级		1：极低风险产品；                               */
					/*			2：低风险产品；                                 */
					/*			3：中等风险产品；                               */
					/*			4：较高风险产品；                               */
					/*			5：高风险产品                                   */
	invest_type	varchar2(1),	/*投资方向		1：票据                                         */
					/*			2：信托                                         */
					/*			3：债券                                         */
					/*			4：信贷                                         */
	rate		number(9,6),	/*预期最大年化收益率		                                        */
	fct_rate	number(9,6),	/*实际年收益率		开放式的以及提前赎回的利率根据实际天数在        */
					/*			redempt_rat表获取。                             */
					/*			其他利率取该值。                                */
	float_rate_flag	varchar2(1),	/*浮动利率标志		Y:有浮动利率。                                  */
					/*			N:无浮动利率。                                  */
					/*			存在浮动利率时，在Fn_flt_radio_parm             */
					/*			表配置浮动利率参数。                            */
	capital_percent	number(9,6),	/*返回本金比例		intst_type为3时有效                             */
					/*			到期返回本金的比例，低于100时，表示损失         */
					/*			本金了，不再计算收益，只有在归还本金比例        */
					/*			等于100时才去计算收益。                         */
	chrg_rate	number(9,6),	/*手续费利率 */
	trust_rate	number(9,6),	/*托管费率		运营该产品需要给第三方的费用比例。              */
    manage_rate number(9,6),    /*管理费用比例*/
	profit_rate	number(9,6),	/*支行分红比例		分给支行的利润比例                              */
	min_amt		number(16,2),	/*产品最低募集金额		达到此金额才划款                        */
					/*			该金额不能大于发售金额                          */
	sale_amt	number(16,2),	/*发售金额		该产品的总额上限，应该是单笔金额的整数倍，      */
					/*			超过该上限，不允许继续购买。                    */
	unit_amt	number(16,2),	/*单笔金额		一份该产品的金额,购买金额必须为此               */
					/*			数值的整数倍            */
	add_amt	number(16,2),	/*递增金额               */
	per_min_amt	number(16,2),	/*个人起存金额		单笔金额的整数倍，个人购买时必须高于这个金额    */
	com_min_amt	number(16,2),	/*单位起存金额		单笔金额的整数倍，单位购买时必须高于这个金额    */
	cancel_flag	varchar2(1),	/*是否撤销		Y:允许                                          */
					/*			N:不允许                                        */
	redempt		varchar2(1),	/*是否赎回		Y:允许赎回；                                    */
					/*			N:不允许赎回。                                  */
	redempt_chrg	number(9,6),	/*赎回费率		赎回时支付的违约金率                            */
	quota_flag	varchar2(2),	/*配额参数		第一位：是否有配额                              */
					/*			Y:是；                                          */
					/*			N:否。                                          */
					/*			第二位：第一位为N时的处理模式：                 */
					/*			Y:可以发售，按全行总额。                        */
					/*			N:不能发售。                                    */
					/*			按配额募集时，有配额的机构必须按配额来认购，    */
					/*			没有配额的根据第二位标志处理。                  */
	per_ac_no	varchar2(24),	/*个人募集资金账户		                                        */
	com_ac_no	varchar2(24),	/*单位募集资金账户		                                        */
	stat		varchar2(1),	/*状态		0:募集开始；                                            */
					/*		1:募集结束(客户账已扣款,产品成立)                       */
					/*		2:已分配收益；                                          */
					/*		3:募集失败                                              */
					/*		*.取消                                                  */
	tel1           varchar2(6),     /*分润时录入柜员  add by lwb 晋中商行让分润时两个柜员做 */
	tel2           varchar2(6)      /*分润时审核柜员  add by lwb 晋中商行让分润时两个柜员做 */
)tablespace users;
create unique index fn_parm_1 on fn_parm(prdt_code) tablespace indx;
