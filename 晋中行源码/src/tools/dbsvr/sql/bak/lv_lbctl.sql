        /**	�������Ʊ� **/
drop index lv_lbctl_idx;
drop index lv_lbctl_idx2;
drop table lv_lbctl;
create table lv_lbctl(
	pack_date	number(8) ,/**	�������      	 **/
	packid		Varchar2(8)	,/**	��id          	 **/
	pkgno		Varchar2(3)	,/**	Pkg���       	 **/
	pay_qs_no	Varchar2(12),/**	���������к�   	 **/ 
	cash_qs_no	Varchar2(12),/**	���������к�   	 **/
	in_date		number(8)	,/**	��������   	 **/
	packmy		Varchar2(40),/**	����Ѻ      	 **/
	dtlcnt		Number(8)   ,/**	��ϸҵ���ܱ���   **/
	totamt		Number(24,2),/**	��ϸҵ���ܽ��   **/
	zcnode		Varchar2(1)	,/**	����ڵ�����   	 **/
	zc_date		number(8)	,/**	��������   		 **/
	zcid		Varchar2(2)	,/**	�����   		 **/
	hz_date		number(8)	,/**	��ִ���ޣ����ڣ� **/
	o_pack_date	number(8)	,/**	ԭ��������   	 **/
	o_packid	Varchar2(8)	,/**	ԭ����id   		 **/
	dtlsuccnt	Number(8)   ,/**	ϸҵ��ɹ�����   **/
	dtlsucamt	Number(24,2),/**	��ϸҵ��ɹ���� **/
	o_pkgno		Varchar2(3)	,/**	ԭ�����ͺ�   	 **/
	ornode		Varchar2(12),/**	����ڵ��   	 **/
	acnode		Varchar2(12),/**	���սڵ��   	 **/
	ffflag		Varchar2(1) ,/**	Ͻ�ڷַ���־   	 **/
	txnum		Varchar2(5) ,/**	ҵ�����ͺ�   	 **/
	bfflag		Varchar2(1) ,/**	������־   		 **/
	qs_date		number(8) ,/**	��������   		 **/
	packstat	Varchar2(2) ,/**	������״̬   	 **/
	sts		Varchar2(1) ,/**	��״̬   		 **/
	brprtflag	Varchar2(64) ,/**	֧�д�ӡ��־   	 **/
	qsprtflag	Varchar2(64) ,/**	�������Ĵ�ӡ��־ **/
	content		Varchar2(64),/**	����������   	 **/
	br_no		varchar2(7),  /** ������**/
	filler		Varchar2(60)  /**	��ע  			 **/
)tablespace users;
create unique index lv_lbctl_idx on lv_lbctl(pack_date,pay_qs_no,packid)tablespace indx;
create index lv_lbctl_idx2 on lv_lbctl(o_pack_date,o_packid,br_no,in_date)tablespace indx;

