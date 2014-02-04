#ifndef _JesterTransfrom_h_
#define _JesterTransfrom_h_

#include <cstdlib>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace jester {
	typedef struct JesterTransform {
	public:
		static const int ROWS = 4;
		static const int COLS = 4;

		JesterTransform(glm::mat4x4 source) {
			glm::vec4 col0 = source[0];
			glm::vec4 col1 = source[1];
			glm::vec4 col2 = source[2];
			glm::vec4 col3 = source[3];

			transform[0][0] = col0[0];
			transform[0][1] = col0[1];
			transform[0][2] = col0[2];
			transform[0][3] = col0[3];

			transform[1][0] = col1[0];
			transform[1][1] = col1[1];
			transform[1][2] = col1[2];
			transform[1][3] = col1[3];

			transform[2][0] = col2[0];
			transform[2][1] = col2[1];
			transform[2][2] = col2[2];
			transform[2][3] = col2[3];

			transform[3][0] = col3[0];
			transform[3][1] = col3[1];
			transform[3][2] = col3[2];
			transform[3][3] = col3[3];
		};

		JesterTransform& operator= (JesterTransform &rhs) {
		  memcpy(&transform, &(rhs.transform), sizeof(transform));
		  return *this;
		}

		glm::mat4x4 toGLMMatrix() {
			return glm::mat4x4(transform[0][0], transform[1][0], transform[2][0], transform[3][0],
							   transform[0][1], transform[1][1], transform[2][1], transform[3][1],
							   transform[0][2], transform[1][2], transform[2][2], transform[3][2],
							   transform[0][3], transform[1][3], transform[2][3], transform[3][3]);
		}

		float transform[COLS][ROWS];
	} JesterTransform;

	inline bool operator== (JesterTransform &lhs, JesterTransform &rhs) {
	  for (int i = 0; i < JesterTransform::ROWS; i++)
	  	for (int j = 0; j < JesterTransform::COLS; j++)
	  		if (lhs.transform[j][i] != rhs.transform[j][i])
	  			return false;
	  return true;
	}
};

#endif