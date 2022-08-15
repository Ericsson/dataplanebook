void
stat_wb_schedule_add64_m(uint16_t num_ops, ...)
{
	struct stat_wb *wb = lcore_wb();
	va_list ap;
	uint16_t i;

	if ((STAT_WB_MAX_SIZE - wb->num_add64_ops) < num_ops)
		force_flush(wb);
	else if (wb->num_add64_ops > STAT_WB_SOFT_THRESHOLD)
		try_flush(wb);

	va_start(ap, num_ops);

	for (i = 0; i < num_ops; i++) {
		uint64_t *counter;
		uint64_t operand;

		counter = va_arg(ap, uint64_t *);
		operand = va_arg(ap, uint64_t);

		schedule_add64(wb, counter, operand);
	}

	va_end(ap);
}
