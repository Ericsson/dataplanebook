void
ep_stats_session_reset(uint32_t session_id,
		       struct ep_session_stats *result)
{
	struct ep_session_stats *session_stats =
		&stats.sessions[session_id];

	result->pkts = __atomic_exchange_n(&session_stats->pkts, 0,
					   __ATOMIC_RELAXED);
	result->bytes = __atomic_exchange_n(&session_stats->bytes, 0,
					    __ATOMIC_RELAXED);
}
