void
ep_stats_update(uint32_t session_id, uint16_t pkt_size)
{
	struct ep_session_stats *session_stats =
	    &stats.sessions[session_id];

	__atomic_fetch_add(&stats.total_pkts, 1,
			   __ATOMIC_RELAXED);
	__atomic_fetch_add(&stats.total_bytes, pkt_size,
			   __ATOMIC_RELAXED);

	__atomic_fetch_add(&session_stats->pkts, 1,
			   __ATOMIC_RELAXED);
	__atomic_fetch_add(&session_stats->bytes, pkt_size,
			   __ATOMIC_RELAXED);
}
