drop index hv_uniontab_idx;
drop  table hv_uniontab;
create table hv_uniontab(
	or_br_no varchar2(12),         /*支付机构号    大额专用全国排，即支付系统行号*/
	br_sts varchar2(1),            /*状态标志 0－生效前 1－有效 2－注销 */
	br_categ varchar2(2),          /*类别01--直接参与人行02--直接参与国库 03--EIS转换中心04--直接参与商业银行05-开户特许直接参与者06-开户特许间接参与者07--间接参与者08-未开户特许直接参与者(债券)*/
	br_code varchar2(3),           /*行别代码    */
	sendco varchar2(4),            /*发报中心代码*/
	dir_br_code varchar2(12),      /*所属直接参与者行号   类别为'07'的必选 */
	node_code varchar2(5),         /*所在节点代码类别为'01','02','03','04','05','08'的必选 */ 
	superior varchar2(130),        /*本行上级参与者  类别为'01','02', '04'的必选  */
	pb_code varchar2(12),          /*所属人行代码类别为'02','03','04','05','06'的必选*/
	city_code varchar2(4),         /*所在城市类别为'01','02','03','04','05','07','08'的必选 */
	acct_sts varchar2(1),          /*清算帐户状态1-待开户2-开户0-借记控制3-待销户4-注销类别为'01','02','03','04','05','06'的必选*/
	acctsts_chg_dt varchar2(8),    /*清算帐户状态变更工作日类别为'01'，'02', '03'，'04'，'05', '06'的必选*/
	acctsts_chg_tm varchar2(14),   /*清算帐户状态变更日期和时间类别为'01'，'02', '03'，'04'，'05', '06'的必选*/
	br_name_f varchar2(60),        /*机构全称    */
	br_name_s varchar2(20),        /*机构简称    */
	addr varchar2(60),             /*地址        */
	phone varchar2(30),            /*电话/电挂   */
	email varchar2(30),            /*电子邮件地址*/
	postcode varchar2(6),          /*邮编        */
	start_date varchar2(8),        /*生效日期    */
	end_date varchar2(8),          /*失效日期    */
	chg_date varchar2(8),          /*更新日期    */
	fax varchar2(20),              /*传真        */
	telegraph varchar2(8),         /*电报        */
	qs_no varchar2(12),            /*清算行号    */
	acct_lvl varchar2(1),          /*帐户级别 1-直连2-间接连 */
	obr_no_attr varchar2(1),       /*对方行属性 1-参与现代支付系统2--参与电子联行系统3--参与其他支付系统4--注销*/
	upd_type varchar2(1),          /*最近更新操作 1:变更2：新增3：注销  */
	rec_upd_no varchar2(8),        /*记录更新期数  年份＋序号： YYYYXXXX */
	km_actno varchar2(16)          /*往来科目帐户帐号  */
)tablespace users;
create	 unique index hv_uniontab_idx on hv_uniontab( or_br_no)tablespace indx;
