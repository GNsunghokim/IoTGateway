static Map* condition;

bool condition_init() {
	condition = map_create(16, map_string_hash, map_string_equals, NULL);
	if(!condition)
		return false;

	return true;
}
