#include "endian.h"

namespace endian {
	ess host_;

	ess check()
	{
		assert(sizeof(short) == 2);

		union {
			short s;
			char c[sizeof(short)];
		} un;

		un.s = 0x0102;

		if (un.c[0] == 1 && un.c[1] == 2)
			return ess::big;
		else if (un.c[0] == 2 && un.c[1] == 1)
			return ess::little;
		else
			assert(0);
	}
}
#ifdef COMPTIME_ENDIAN
constexpr endian::ess endian::host()
{
	return ess::host;
}
#else
void endian::init()
{
	host_ = check();
}

endian::ess endian::host()
{
	return host_;
}
#endif
