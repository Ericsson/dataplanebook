void
ep_stats_session_read(uint32_t session_id,
		      struct ep_session_stats *result)
{
	struct ep_session_stats *session_stats =
		&stats.sessions[session_id];

	rte_spinlock_lock(&stats_lock);

	result->pkts = __atomic_load_n(&session_stats->pkts,
				       __ATOMIC_RELAXED);
	result->bytes = __atomic_load_n(&session_stats->bytes,
					__ATOMIC_RELAXED);

	rte_spinlock_unlock(&stats_lock);
}
