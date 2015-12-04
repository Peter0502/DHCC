drop table cif_ass_reg;
create table cif_ass_reg
(
	cif_no		Number(9),	/*�ͻ���						*/
	ass_date	Number(8),	/*��������		                                */
	end_date	Number(8),	/*��Ч��ֹ����		                                */
	ass_br_no	varchar2(5),	/*��������		                                */
	ass_tel		Varchar2(6),	/*��Ա		                                        */
	qt_code		Varchar2(3),	/*������		���ֿͻ�����������Ŀ            */
	answer		Varchar2(20),	/*�ͻ���		�ͻ��ύ�Ĵ�                  */
	score		number(4),	/*���������÷�		�ͻ��÷�                        */
	ass_lvl		Varchar2(1)	/*�����ȼ�	Ass_lvl	1��С�ڵ���20���������������   */
					/*			2��21��--45����������Ƚ���     */
					/*			3��46��--70���������ƽ����     */
					/*			4��71��--85����������ɳ���     */
					/*			5��86��--100�����������ȡ��    */
)tablespace users;
create unique index cif_ass_reg_1 on cif_ass_reg(cif_no) tablespace indx;


