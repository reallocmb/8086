all:
	c89 src/main.c -o bin/8086

37: all
	bin/8086 listing_0037_single_register_mov

38: all
	bin/8086 listing_0038_many_register_mov
