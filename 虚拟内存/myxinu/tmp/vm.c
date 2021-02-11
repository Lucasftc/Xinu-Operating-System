#include <xinu.h>
extern uint32 framelist[];
extern uint32 framenum;
extern uint32 usedframe;
syscall init_pd(struct pd_t *pd)
{
    memset(pd, 0, 4 * 1024);
    return OK;
}
syscall init_pt(struct pt_t* pt){
    memset(pt, 0, 4 * 1024);
    return OK;
}
syscall init_core_pt(struct pt_t *pt,int n){
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < NPTENTRIES;j++){
            pt->pt_base = i * NPTENTRIES*PGSIZE+j*PGSIZE;
            pt->pt_pres = 1;
            pt++;
        }
    }
    return OK;
}
syscall add_table_to_dir(struct pd_t * pd,struct pt_t* pt,int offset){
    (pd + offset)->pd_pres = 1;
    (pd + offset)->pd_write = 1;
    (pd + offset)->pd_user = 0;
    (pd + offset)->pd_pwt = 0;
    (pd + offset)->pd_pcd = 0;
    (pd + offset)->pd_acc = 0;
    (pd + offset)->pd_mbz = 0;
    (pd + offset)->pd_fmb = 0;
    (pd + offset)->pd_global = 0;
    (pd + offset)->pd_avail = 1;
    (pd + offset)->pd_base = pt;
}
syscall add_page_to_table(struct pt_t* pt,uint32* addr,int offset){
    (pt + offset)->pt_pres = 1;
    (pt + offset)->pt_write = 1;
    (pt + offset)->pt_user = 1;
    (pt + offset)->pt_pwt = 1;
    (pt + offset)->pt_pcd = 1;
    (pt + offset)->pt_acc = 1;
    (pt + offset)->pt_dirty = 1;
    (pt + offset)->pt_mbz = 1;
    (pt + offset)->pt_global = 0;
    (pt + offset)->pt_avail = 1;
    (pt + offset)->pt_base = addr;
    return OK;
}

uint32 get_free_frame(){
    static int last = 0;
    if(usedframe==framenum)
        panic("free frame ran out\n");
    while(1){
        if(!(framelist[last]&1)){
            framelist[last] |= 1;
            usedframe++;
            return framelist[last] |= -2;
        }
        last++;
        if(last>=framenum)
            last %= framenum;
    }
}