#include "monitor/breakpoint.h"
#include "monitor/expr.h"


static BP *head, *free_;
static BP BP_pool[NR_BP];

void init_bp_pool()
{
	int i;
	for (i = 0; i < NR_BP; i++)
	{
		BP_pool[i].cnt = 0;
		BP_pool[i].NO = i;
		BP_pool[i].enable = true;
		BP_pool[i].next = &BP_pool[i + 1];
	}
	BP_pool[NR_BP - 1].next = NULL;

	head = NULL;
	free_ = BP_pool;
}

/* TODO: Implement the functionality of watchpoint */

BP *new_bp()
{
	if (free_ == NULL)
	{
		printf("\33[1;34mError: No new watchpoints available\33[0m\n");
		return NULL;
	}
	BP *temp = head;
	if(temp==NULL){
		// Log("TEMP==NULL");
		BP *ret = free_;
		free_ = free_->next;
		ret->next = NULL;
		head = ret;
		return ret;
	}
	while (temp->next != NULL)
		temp = temp->next;
	BP *ret = free_;
	free_ = (*free_).next;
	ret->next = NULL;
	temp->next = ret;
	return ret;
}

void free_bp(BP * bp)
{
	BP *temp = head;
	if (temp == NULL)
	{
		printf("\33[1;34mError: No watch points left to free\33[0m\n");
		return;
	}
	BP *f;
	for(f=free_;f!=NULL;f=f->next){
		if(f==bp){
			printf("\33[1;34mError: Watchpoint \33[0m\33[1;36m%d \33[0m\33[1;34mis not used\33[0m\n" ,f->NO);
			return;
		}
	}
	if(head==bp){
		head = head->next;
		bp->next = free_;
		free_ = bp;
	}else{
		while (temp->next != bp)
			temp = temp->next;
		temp->next = bp->next;
		bp->next = free_;
		free_ = bp;
	}
	bp->cnt = 0;
	printf("\33[1;37mFree breakpoint\33[0m \33[1;36m%d\33[0m\n" ,bp->NO);
	return;
}

BP *get_breakpoint_head()
{
	return head;
}

void delete_bp(int number){
	if(number >= 32){
		printf("\33[1;34mError: No breakpoint \33[0m\33[1;36m%d\33[0m\n" ,number);
		return;
	}
	free_bp(&BP_pool[number]);
	return;
}
