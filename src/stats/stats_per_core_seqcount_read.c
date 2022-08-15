uint64_t
ep_stats_get_session_stats(uint32_t session_id, struct ep_session_stats *stats)
{
	struct ep_session_stats *session_stats =
		&stats.sessions[session_id];
	uint32_t sn;

	do {
		sn = rte_seqcount_read_begin(&config->sc);

		result->pkts = session_stats->pkts;
		result->bytes = session_stats->bytes;

	} while (rte_seqcount_read_retry(&config->sc, sn));

	return total_bytes;
}
