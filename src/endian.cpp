#include "endian.h"

endianness host_endianness;

endianness check_endianness()
{
	assert(sizeof(short) == 2);

    union {
        short s;
        char c[sizeof(short)];
    } un;

    un.s = 0x0102;

	if (un.c[0] == 1 && un.c[1] == 2)
		return endianness::big_endian;
	else if (un.c[0] == 2 && un.c[1] == 1)
		return endianness::little_endian;
	else
		assert(0);
}

endianness get_host_endianness()
{
	return host_endianness;
}

void init_host_endianness()
{
	host_endianness = check_endianness();
}
