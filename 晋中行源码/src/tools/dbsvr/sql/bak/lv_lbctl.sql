        /**	来包控制表 **/
drop index lv_lbctl_idx;
drop index lv_lbctl_idx2;
drop table lv_lbctl;
create table lv_lbctl(
	pack_date	number(8) ,/**	打包日期      	 **/
	packid		Varchar2(8)	,/**	包id          	 **/
	pkgno		Varchar2(3)	,/**	Pkg编号       	 **/
	pay_qs_no	Varchar2(12),/**	发起清算行号   	 **/ 
	cash_qs_no	Varchar2(12),/**	接收清算行号   	 **/
	in_date		number(8)	,/**	进入日期   	 **/
	packmy		Varchar2(40),/**	包密押      	 **/
	dtlcnt		Number(8)   ,/**	明细业务总笔数   **/
	totamt		Number(24,2),/**	明细业务总金额   **/
	zcnode		Varchar2(1)	,/**	轧差节点类型   	 **/
	zc_date		number(8)	,/**	轧差日期   		 **/
	zcid		Varchar2(2)	,/**	轧差场次   		 **/
	hz_date		number(8)	,/**	回执期限（日期） **/
	o_pack_date	number(8)	,/**	原往包日期   	 **/
	o_packid	Varchar2(8)	,/**	原往包id   		 **/
	dtlsuccnt	Number(8)   ,/**	细业务成功笔数   **/
	dtlsucamt	Number(24,2),/**	明细业务成功金额 **/
	o_pkgno		Varchar2(3)	,/**	原包类型号   	 **/
	ornode		Varchar2(12),/**	发起节点号   	 **/
	acnode		Varchar2(12),/**	接收节点号   	 **/
	ffflag		Varchar2(1) ,/**	辖内分发标志   	 **/
	txnum		Varchar2(5) ,/**	业务类型号   	 **/
	bfflag		Varchar2(1) ,/**	补发标志   		 **/
	qs_date		number(8) ,/**	清算日期   		 **/
	packstat	Varchar2(2) ,/**	包处理状态   	 **/
	sts		Varchar2(1) ,/**	包状态   		 **/
	brprtflag	Varchar2(64) ,/**	支行打印标志   	 **/
	qsprtflag	Varchar2(64) ,/**	清算中心打印标志 **/
	content		Varchar2(64),/**	包附加数据   	 **/
	br_no		varchar2(7),  /** 机构号**/
	filler		Varchar2(60)  /**	备注  			 **/
)tablespace users;
create unique index lv_lbctl_idx on lv_lbctl(pack_date,pay_qs_no,packid)tablespace indx;
create index lv_lbctl_idx2 on lv_lbctl(o_pack_date,o_packid,br_no,in_date)tablespace indx;

