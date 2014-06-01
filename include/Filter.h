#ifndef _Filter_h_
#define _Filter_h_

#include "FusionBone.h"

namespace jester {
	class Filter {
	public:
		virtual jester::FusionBone update(jester::FusionBone bone) = 0;

		virtual ~Filter() = 0;
	};
};

#endif
