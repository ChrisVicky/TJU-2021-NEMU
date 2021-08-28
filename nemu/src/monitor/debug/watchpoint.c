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
		Log("Error: No new watchpoints available");
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
		Log("No watchpoint %d\n" ,number);
		return ;
	}
	for(i=0;i<NR_WP;i++){
		if(number==wp_pool[i].NO){
			wp = wp_pool + i;
		}
	}
	Log("Free watchpoint %d %s (0x%08x)\n" ,number ,wp->expressions ,wp->old_value);
	WP *temp = head;
	if (temp == NULL)
	{
		Log("Error: No watch points left to free");
		return;
	}
	WP *f;
	for(f=free_;f!=NULL;f=f->next){
		if(f==wp){
			Log("Watchpoint %d is not used\n" ,f->NO);
			return;
		}
	}
	if(head==wp){
		head = head->next;
		wp->next = free_;
		free_ = wp;
		return;
	}
	while (temp->next != wp)
		temp = temp->next;
	temp->next = wp->next;
	wp->next = free_;
	free_ = wp;
}

WP *get_head()
{
	return head;
}
