drop table fn_flt_radio_parm;
create table fn_flt_radio_parm
(
	Prdt_code	varchar2(20),	/*产品代码		Fn_parm中相同			*/
	Ctl_flag	Varchar2(1),	/*控制标志		0：取最大值；                   */
					/*			1：累加；                       */
	Cif_lvl		Varchar2(1),	/*客户级别		大于等于该客户级别              */
	Flt_radio1	Number(9,6),	/*上浮比例		满足客户级别上浮的比例          */
	Buy_amt		Number(16,2),	/*单次认购金额		大于等于该认购金额              */
	Flt_radio2	Number(9,6),	/*上浮比例		满足认购笔数上浮的比例          */
	Buy_amt1		Number(16,2),	/*单次认购金额		大于等于该认购金额              */
	Flt_radio21	Number(9,6),	/*上浮比例		满足认购笔数上浮的比例          */
	Buy_amt2		Number(16,2),	/*单次认购金额		大于等于该认购金额              */
	Flt_radio22	Number(9,6),	/*上浮比例		满足认购笔数上浮的比例          */
	Buy_amt3		Number(16,2),	/*单次认购金额		大于等于该认购金额              */
	Flt_radio23	Number(9,6),	/*上浮比例		满足认购笔数上浮的比例          */	
	Buy_amt4		Number(16,2),	/*单次认购金额		大于等于该认购金额              */
	Flt_radio24	Number(9,6),	/*上浮比例		满足认购笔数上浮的比例          */
	
	Total_amt	Number(16,2),	/*认购总金额		大于等于该总认购金额            */
	Flt_radio3	Number(9,6),	/*上浮比例		满足总认购金额上浮的比例        */
	reg_date	number(8),	/*登记日期		                                */
	Reg_tel		Varchar2(6),	/*登记柜员		                                */
	Upd_date	Number(8),	/*修改日期		                                */
	Upd_tel		Varchar2(6)	/*修改柜员	                                        */
)tablespace users;
create unique index fn_flt_radio_parm_1 on fn_flt_radio_parm(prdt_code) tablespace indx; 