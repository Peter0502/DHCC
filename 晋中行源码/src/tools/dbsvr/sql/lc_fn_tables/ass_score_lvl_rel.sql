drop table ass_score_lvl_rel;
create table ass_score_lvl_rel
(
qt_code		varchar2(3),	/*������						*/	
min_score	number(4),	/*С��ֵ		                                */
max_score	number(4),	/*���ֵ		                                */
ass_lvl		varchar2(1)	/*���ճ��ܵȼ�		1��С�ڵ���20���������������   */
				/*			2��21��--45����������Ƚ���     */
				/*			3��46��--70���������ƽ����     */
				/*			4��71��--85����������ɳ���     */
				/*			5��86��--100�����������ȡ��    */
)tablespace users;
create unique index ass_score_lvl_rel_1 on ass_score_lvl_rel(qt_code,min_score,max_score) tablespace indx;
