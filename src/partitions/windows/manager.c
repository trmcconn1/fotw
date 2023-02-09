#include "partition.h"

struct filter_entry my_filters[] =
{
	{
		"distinct",
		"no repeated parts",
		&distinct_parts
	},
	{
		"odd",
		"odd number parts",
		&odd_parts
	},
	{
		"superdistinct",
		"parts differ by at least 2",
		&super_distinct_parts
	},
	{
		"fewerthan(m)",
		"fewer than m parts",
		&fewerthan
	},
	{
		"lessthan(m)",
		"no parts m or larger",
		&lessthan
	},
	{
		"bressoud",
		"distinct parts, each even part > 2(#odd parts)",
		&bressoud
	},
	{
		"1or4mod5",
		"parts congruent to 1 or 4 modulo 5",
		&_1or4mod5
	},
};

int n_filters = sizeof(my_filters)/sizeof(struct filter_entry);

void dump_filter_names(void){
	int i;

	if(verbose)printf("\nAvailable filters [index]\tname (info)\n\n");
	for(i=0;i<n_filters;i++){
		printf("[%d]\t%s",i,my_filters[i].name);
		if(verbose)printf(" (%s)",my_filters[i].info);
		printf("\n");
	}
	printf("\n");
}
