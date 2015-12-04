drop table cif_ass_reg;
create table cif_ass_reg
(
	cif_no		Number(9),	/*客户号						*/
	ass_date	Number(8),	/*评估日期		                                */
	end_date	Number(8),	/*有效截止日期		                                */
	ass_br_no	varchar2(5),	/*评估机构		                                */
	ass_tel		Varchar2(6),	/*柜员		                                        */
	qt_code		Varchar2(3),	/*试题编号		区分客户做的哪套题目            */
	answer		Varchar2(20),	/*客户答案		客户提交的答案                  */
	score		number(4),	/*风险评估得分		客户得分                        */
	ass_lvl		Varchar2(1)	/*评估等级	Ass_lvl	1：小于等于20分评级结果保守型   */
					/*			2：21分--45分评级结果稳健型     */
					/*			3：46分--70分评级结果平衡型     */
					/*			4：71分--85分评级结果成长型     */
					/*			5：86分--100分评级结果进取型    */
)tablespace users;
create unique index cif_ass_reg_1 on cif_ass_reg(cif_no) tablespace indx;


