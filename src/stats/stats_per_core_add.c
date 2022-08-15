static void
stats_add64(uint64_t *counter_value, uint64_t operand)
{
	uint64_t new_value;

	new_value = *counter_value + operand;

        __atomic_store_n(counter_value, new_value, __ATOMIC_RELAXED);
}

void
ep_stats_update(uint16_t session_id, uint64_t pkt_size)
{
	unsigned int lcore_id;
	struct ep_stats *stats;
	struct ep_session_stats *session_stats;

	lcore_id = rte_lcore_id();

	stats = &lcore_stats[lcore_id];
	stats_add64(&stats->total_pkts, 1);
	stats_add64(&stats->total_bytes, pkt_size);

	session_stats = &stats->sessions[session_id];
	stats_add64(&session_stats->pkts, 1);
	stats_add64(&session_stats->bytes, pkt_size);
}
