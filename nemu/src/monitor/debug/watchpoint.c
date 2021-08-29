#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool()
{
	int i;
	for (i = 0; i < NR_WP; i++)
	{
		wp_pool[i].NO = i;
		wp_pool[i].enable = true;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp()
{
	if (free_ == NULL)
	{
		printf("\33[1;34mError: No new watchpoints available.\33[0m\n");
		return NULL;
	}
	WP *temp = head;
	if(temp==NULL){
		// Log("TEMP==NULL");
		WP *ret = free_;
		free_ = free_->next;
		ret->next = NULL;
		head = ret;
		return ret;
	}
	while (temp->next != NULL)
		temp = temp->next;
	WP *ret = free_;
	free_ = (*free_).next;
	ret->next = NULL;
	temp->next = ret;
	return ret;
}

void free_wp(int number)
{
	int i;
	static WP *wp = NULL;
	if(number >= NR_WP){
		printf("\33[1;34mError: No watchpoint %d\33[0m\n" ,number);
		return ;
	}
	for(i=0;i<NR_WP;i++){
		if(number==wp_pool[i].NO){
			wp = wp_pool + i;
		}
	}
	
	WP *temp = head;
	if (temp == NULL)
	{
		printf("\33[1;34mError: No watch points left to free.\33[0m\n");
		return;
	}
	WP *f;
	for(f=free_;f!=NULL;f=f->next){
		if(f==wp){
			printf("\33[1;34mError: Watchpoint %d is not used.\33[0m\n" ,f->NO);
			return;
		}
	}
	if(head==wp){
		head = head->next;
		wp->next = free_;
		free_ = wp;
	}else{
		while (temp->next != wp)
			temp = temp->next;
		temp->next = wp->next;
		wp->next = free_;
		free_ = wp;
	}
	printf("\33[1;37mFree watchpoint\33[0m \33[1;36m%d\33[0m \33[40;36m%s\33[0m \33[40;33m(0x%08x).\33[0m\n" ,number ,wp->expressions ,wp->old_value);
	return;
}

WP *get_head()
{
	return head;
}
