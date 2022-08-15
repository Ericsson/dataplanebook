void
ep_stats_update(uint32_t session_id, uint16_t pkt_size)
{
	struct ep_session_stats *session_stats =
	    &stats.sessions[session_id];

	stats.total_pkts++;
	stats.total_bytes += pkt_size;

	session_stats->pkts++;
	session_stats->bytes += pkt_size;
}
