/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_HAZARDPOINTER_H_
#define ENGINE_STM_HAZARDPOINTER_H_

namespace engine {
  namespace stm {

	class HazardPointer {
		Vector<Object*> retireList;

		static const int RETIRED_TRESHOLD = 100;

	public:
		void retireObject(Object* object) {
			retireList.add(object);

			if (retireList.size() > RETIRED_TRESHOLD)
				scan();
		}

		void scan() {

		}

	};

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_HAZARDPOINTER_H_ */
