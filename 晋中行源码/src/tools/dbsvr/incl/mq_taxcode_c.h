#ifndef __mq_taxcode_CH__
#define __mq_taxcode_CH__
struct mq_taxcode_c{
        char            taxorg_code[13];
        char            taxorg_name[61];
        char            taxorg_type[2];
        char            ofnode_code[13];
        long            effect_date;
};
#endif 
