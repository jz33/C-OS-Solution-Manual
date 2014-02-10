#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
 
#define LICENSE "GPL"
#define DESCRIPTION "Ch2_LinkedList"
#define AUTHOR "jz33"

MODULE_LICENSE(LICENSE);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_AUTHOR(AUTHOR);

/*
Operating System Concepts Edition 9th Project 2.1
Test on Debian 7
Auther: junzhengrice@gmail.com
*/

//Simplified birthday struct
struct birthday{
    int day;
    struct list_head list;
};
typedef struct birthday bday;

//Initialize bday list
inline bday* bday_init(int d){
    bday* person = kmalloc(sizeof(bday), GFP_KERNEL);
    person->day = d;
    INIT_LIST_HEAD(&person->list);
    return person;
}

//Tranverse macro
#define list_for_each_entry_safe_tranverse(pos, n, head, member) \
        for(pos = list_entry((head)->next,typeof(*pos),member), \
               n = list_entry(pos->member.next,typeof(*pos),member); \
            &pos->member != (head); \
            pos = n, \
               n = list_entry(n->member.next,typeof(*n),member)) \
        printk(KERN_INFO "Tranverse: %d\n",pos->day);
 
//Delete macro        
#define list_for_each_entry_safe_remove(pos, n, head, member) \
        for(pos = list_entry((head)->next,typeof(*pos),member), \
               n = list_entry(pos->member.next,typeof(*pos),member); \
            &pos->member != (head); \
            pos = n, \
               n = list_entry(n->member.next,typeof(*n),member)){ \
        printk(KERN_INFO "Remove: %d\n",pos->day); \
        list_del(&(pos->member)); \
        kfree(pos); \
        }

static LIST_HEAD(head);

int linkedlist_init(void){
    
    bday* e0 = bday_init(0);
    bday* e1 = bday_init(1);
    bday* e2 = bday_init(2);
    bday* e3 = bday_init(3);
    bday* e4 = bday_init(4);
    
    bday* pos;
    bday* n;
    
    /*
      head->e0->e1->e2->e3->e4->(head)
    */
    list_add_tail(&e4->list, &head);
    list_add_tail(&e3->list, &e4->list);
    list_add_tail(&e2->list, &e3->list);
    list_add_tail(&e1->list, &e2->list);
    list_add_tail(&e0->list, &e1->list);
        
    list_for_each_entry_safe_tranverse(pos, n, &head, list);
    
    return 0;
}

void linkedlist_exit(void){  
    bday* pos;
    bday* n;
    list_for_each_entry_safe_remove(pos, n, &head, list);
    printk(KERN_INFO "Removing Module Done\n");
}

module_init(linkedlist_init);
module_exit(linkedlist_exit);
