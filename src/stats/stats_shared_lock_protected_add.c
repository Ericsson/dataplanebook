void
ep_stats_update(uint32_t session_id, uint16_t pkt_size)
{
	struct ep_session_stats *session_stats =
	    &stats.sessions[session_id];

	rte_spinlock_lock(&stats_lock);

	stats.total_pkts++;
	stats.total_bytes += pkt_size;

	session_stats->pkts++;
	session_stats->bytes += pkt_size;

	rte_spinlock_unlock(&stats_lock);
}
