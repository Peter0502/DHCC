      /**С��ķ��͵Ǽǲ� **/
drop index lv_sendlist_idx;
drop index lv_sendlist_idx2;
drop table lv_sendlist;
create table lv_sendlist(
   	tx_date		number(8)	, /** ��������		**/
   	br_no		varchar2(7)	, /** ���׻���		**/
	tel		varchar2(6)	, /** ����Ա		**/
	trace_no	number(9)	, /** ��ˮ��		**/
	refno		varchar2(20), /** ���Ĳο���	**/
	flagno		varchar2(8), /** ���ı�ʶ��   	**/
	packtype	varchar2(3)	, /** ������		**/
	packno		varchar2(3)	, /** �����		**/
	orderno		varchar2(8)	, /** ֧���������	**/
	or_br_no	varchar2(12), /** �������к�	**/
	pay_qs_no	varchar2(12), /** ����������	**/
	ac_br_no	varchar2(12), /** �������к�	**/
	cash_qs_no	varchar2(12), /** ����������	**/
	tx_amt		number(17,2), /** ���׽��		**/
	respflag	varchar2(1)	, /** �Ƿ��յ�911����**/
	respno		varchar2(8)	, /** Ӧ����		**/
	respdesc	varchar2(60), /** Ӧ������		**/
	filler		varchar2(60)  /** ��ע			**/
)tablespace users;
create unique index lv_sendlist_idx on lv_sendlist(tx_date,refno)tablespace indx;
create index lv_sendlist_idx2 on lv_sendlist(br_no,tel,respflag)tablespace indx;

