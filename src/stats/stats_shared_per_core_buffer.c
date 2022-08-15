void
ep_stats_update(uint32_t session_id, uint16_t pkt_size)
{
	struct ep_session_stats *session_stats =
		&stats.sessions[session_id];

	stat_wb_schedule_add64(&stats.total_pkts, 1);
	stat_wb_schedule_add64(&stats.total_bytes, pkt_size);

	stat_wb_schedule_add64(&session_stats->pkts, 1);
	stat_wb_schedule_add64(&session_stats->bytes, pkt_size);
}
