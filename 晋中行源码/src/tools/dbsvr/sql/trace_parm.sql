/***************************************************
	<<	流水帐查询配置表	>>

			create by  gyf	
			2006年9月22日
***************************************************/
drop		table	trace_parm;
create	table	trace_parm(
	tx_code		varchar2(4),	/** 交易名称 **/
	seq_no          number(3),	/** 排列序号 **/
	field           varchar2(20),	/** 字段名称 **/
	field_type      varchar2(10),	/** 字段类型:number varchar2 **/
	field_name      varchar2(50),	/** 字段名称 **/
	list            varchar2(50)	/** 字段数值出处 **/
)tablespace	users;
create   unique index trace_parm_index on trace_parm( tx_code,seq_no)tablespace indx;
