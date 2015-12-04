#ifndef __com_parm_holiday_CH__
#define __com_parm_holiday_CH__
struct com_parm_holiday_c{
	char      rowid[20];
	char      date_code[4];
	char      date_name[11];
	long      beg_date;
	long      end_date;
	long      days;
	char      type[2];
	char      filler[61];
};
#endif 
