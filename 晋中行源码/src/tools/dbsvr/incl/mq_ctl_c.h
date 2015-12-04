#ifndef __mq_ctl_CH__
#define __mq_ctl_CH__
struct mq_ctl_c{
        char            src[13];
        char            des[13];
        long            workdate;
        char            login_sts[2];
        char            work_sts[2];
        long            stop_date;
        long            stop_time;
        long            back_date;
        long            back_time;
        char            stoprun_reason[201];
};
#endif 
