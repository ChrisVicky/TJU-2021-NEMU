#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *HEAD;
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

	head = HEAD;
	head->next = NULL;
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
	Log("TEMP!=NULL");
	while (temp->next != NULL)
		temp = temp->next;
	Log("Successfully Enter a new wp");
	WP *ret = free_;
	Log("2");
	free_ = (*free_).next;
	Log("3");
	ret->next = NULL;
	Log("4");
	temp->next = ret;
	return ret;
}

void free_wp(WP *wp)
{
	WP *temp = head;
	if (temp->next == NULL)
	{
		Log("Error: No watch points left to free out!");
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
