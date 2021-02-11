#include<xinu.h>
const uint32 *kpgdir;

extern uint32 framelist[];
extern uint32 framenum;
extern uint32 usedframe;
extern syscall init_pd(struct pd_t *pd);
extern syscall init_pt(struct pt_t* pt);
extern syscall init_core_pt(struct pt_t* pt,int n);
extern syscall add_table_to_dir(struct pd_t* pd, struct pt_t* pt,int offset);
extern syscall add_page_to_table(struct pt_t *pt, uint32 *addr, int offset);

void vminit(){
    int i;
    uint32 *page_dir = (uint32 *)PGROUNDUP((uint32)&end);
    kpgdir = page_dir;
    init_pd(page_dir);
    uint32 *page_tab = (uint32 *)((uint32)page_dir + PGSIZE);
    init_core_pt((struct pt_t*)page_tab, 8);
    add_table_to_dir(page_dir, page_tab,0);
    page_tab = (uint32 *)((uint32)page_dir + 36 * 1024);
    init_pt(page_tab);
    add_page_to_table(page_tab, (uint32 *)0x6000, 1023);
    add_table_to_dir(page_dir, page_tab, 1021);
    //for (i = 0; i < 10;i++)
    //    framelist[i] |= 1;
    //usedframe += 10;

}
