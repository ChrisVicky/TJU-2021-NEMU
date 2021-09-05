#include "monitor/watchpoint.h"
#include "monitor/expr.h"


static WP *head, *free_;
static WP wp_pool[NR_WP];

void init_wp_pool()
{
	int i;
	for (i = 0; i < NR_WP; i++)
	{
		wp_pool[i].cnt = 0;
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
		printf("\33[1;34mError: No new watchpoints available\33[0m\n");
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

void free_wp(WP * wp)
{
	WP *temp = head;
	if (temp == NULL)
	{
		printf("\33[1;34mError: No watch points left to free\33[0m\n");
		return;
	}
	WP *f;
	for(f=free_;f!=NULL;f=f->next){
		if(f==wp){
			printf("\33[1;34mError: Watchpoint \33[0m\33[1;36m%d \33[0m\33[1;34mis not used\33[0m\n" ,f->NO);
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
	wp->cnt = 0;
	printf("\33[1;37mFree watchpoint\33[0m \33[1;36m%d\33[0m : \33[40;36m%s\33[0m (0x%08x)\n" ,wp->NO ,wp->expressions ,wp->old_value);
	return;
}

WP *get_head()
{
	return head;
}

void delete_wp(int number){
	if(number >= 32){
		printf("\33[1;34mError: No watchpoint \33[0m\33[1;36m%d\33[0m\n" ,number);
		return;
	}
	free_wp(&wp_pool[number]);
	return;
}
