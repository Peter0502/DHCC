drop  table com_parm_holiday;
create table com_parm_holiday(
        date_code varchar2(3), /**�ڼ��մ���**/
        date_name varchar2(10),/**�ڼ�������**/
	beg_date number(8),    /**��ʼ����**/
	end_date number(8),    /**��ֹ����**/
	days number(6),        /**�ܹ�����**/
	type varchar2(1),      /**�Ƿ���Ч0:δ��Ч 1:����Ч**/
	filler varchar2(60)    /**��    ע**/
)tablespace users;
