void
ep_stats_update(uint32_t session_id, uint16_t pkt_size)
{
	struct ep_session_stats *session_stats =
		&stats.sessions[session_id];

	stat_wb_schedule_add64_m(4,
			&stats.total_pkts, (uint64_t)1,
			&stats.total_bytes, (uint64_t)pkt_size,
			&session_stats->pkts, (uint64_t)1,
			&session_stats->bytes, (uint64_t)pkt_size);
}
