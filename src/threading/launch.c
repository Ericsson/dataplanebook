#include <rte_eal.h>
#include <rte_lcore.h>
#include <rte_debug.h>

static int
do_work(void *arg)
{
	for (;;)
		; /* perform fast path work here */
}

int
main(int argc, char **argv)
{
	int rc;
	unsigned int lcore_id;

	rc = rte_eal_init(argc, argv);
	if (rc < 0)
		rte_exit(1, "Invalid EAL arguments\n");

	RTE_LCORE_FOREACH_WORKER(lcore_id)
		if (rte_eal_remote_launch(do_work, NULL, lcore_id) != 0)
			rte_panic("Failed to launch lcore thread\n");

	do_work(NULL);
}
