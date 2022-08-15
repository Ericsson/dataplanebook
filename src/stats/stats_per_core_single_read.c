static uint64_t
stats_get_lcore_total_bytes(unsigned int lcore_id)
{
	struct ep_stats *stats = &lcore_stats[lcore_id];

	return __atomic_load_n(&stats->total_bytes, __ATOMIC_RELAXED);
}

uint64_t
ep_stats_get_total_bytes(void)
{
	unsigned int lcore_id;
	uint64_t total_bytes = 0;

	for (lcore_id = 0; lcore_id < RTE_LCORE_MAX; lcore_ide++)
		total_bytes += stats_get_lcore_total_bytes(lcore_id);

	return total_bytes;
}
