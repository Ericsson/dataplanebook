
void
ep_stats_lock(void)
{
	rte_spinlock_lock(&stats_lock);
}

void
ep_stats_unlock(void)
{
	rte_spinlock_unlock(&stats_lock);
}

void
ep_stats_reset(uint32_t session_id, uint16_t pkt_size)
{
	struct ep_session_stats *session_stats =
	    &stats.sessions[session_id];

	session_stats->pkts = 0;
	session_stats->bytes = 0;
}
