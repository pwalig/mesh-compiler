#include "endian.h"

namespace endian {
	ness host_;

	ness check()
	{
		assert(sizeof(short) == 2);

		union {
			short s;
			char c[sizeof(short)];
		} un;

		un.s = 0x0102;

		if (un.c[0] == 1 && un.c[1] == 2)
			return ness::big;
		else if (un.c[0] == 2 && un.c[1] == 1)
			return ness::little;
		else
			assert(0);
	}
}
#ifdef COMPTIME_ENDIAN
constexpr endian::ness endian::host()
{
	return ness::host;
}
#else
void endian::init()
{
	host_ = check();
}

endian::ness endian::host()
{
	return host_;
}
#endif
