drop  index chnl_premis_idx;
drop  table chnl_premise;
create table chnl_premise(
	pt_date       number(9),	 /* 平台日期与卡号,平台流水唯一确定一笔业务*/
	pt_trace_no   number(6),	/* 平台流水号与卡号,平台日期唯一确定一笔业务*/
	ac_no         varchar2(19), 	/* 卡号与平台日期,平台流水号唯一确定一笔业务*/
	l_premise_no  number(6),    	/* 上笔预授权号*/
	premise_no    number(6),    	/* 预授权号*/
	premise_amt   number(16,2), 	/* 预授权金额*/
	finish_amt    number(16,2), 	/* 完成金额*/
	sts           varchar2(1),  	/* 预授权状态 0 正常预授权 1预授权撤销 2预授权完成 3预授权失效 */
	filler        varchar2(30)  	/* 备注*/
)tablespace users;
create unique index chnl_premis_idx on chnl_premise(pt_date,pt_trace_no,ac_no)tablespace indx;