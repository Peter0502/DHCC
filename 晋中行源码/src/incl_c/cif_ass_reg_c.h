#ifndef __cif_ass_reg_CH__
#define __cif_ass_reg_CH__
struct cif_ass_reg_c{
	char      rowid[20];
	long      cif_no;
	long      ass_date;
	long      end_date;
	char      ass_br_no[6];
	char      ass_tel[7];
	char      qt_code[4];
	char      answer[21];
	long      score;
	char      ass_lvl[2];
};
#endif 
