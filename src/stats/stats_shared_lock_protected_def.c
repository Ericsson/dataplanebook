static struct ep_stats stats[RTE_MAX_LCORE];

static rte_spinlock stats_lock = RTE_SPINLOCK_INITIALIZER;
