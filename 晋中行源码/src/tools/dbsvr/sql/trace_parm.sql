/***************************************************
	<<	��ˮ�ʲ�ѯ���ñ�	>>

			create by  gyf	
			2006��9��22��
***************************************************/
drop		table	trace_parm;
create	table	trace_parm(
	tx_code		varchar2(4),	/** �������� **/
	seq_no          number(3),	/** ������� **/
	field           varchar2(20),	/** �ֶ����� **/
	field_type      varchar2(10),	/** �ֶ�����:number varchar2 **/
	field_name      varchar2(50),	/** �ֶ����� **/
	list            varchar2(50)	/** �ֶ���ֵ���� **/
)tablespace	users;
create   unique index trace_parm_index on trace_parm( tx_code,seq_no)tablespace indx;
