
#include <iostream>
#include <assert.h>

#include "Timeline.h"

int main()
{
	{
		Timeline timeline = Timeline();

		{
			TimelineEvent e1(etype_Free, 0, 0, 101);
			TimelineEvent e2(etype_Free, 1, 0, 102);
			TimelineEvent e3(etype_Free, 1, 1, 103);
			TimelineEvent e4(etype_Free, 2, 1, 104);
			TimelineEvent e5(etype_Free, 3, 0, 105);
			TimelineEvent e6(etype_Free, 3, 1, 106);

			for (int i = 0; i < 1000; ++i)
			{
				timeline.put(e6);
			}
			timeline.put(e3);
			timeline.put(e5);
			timeline.put(e1);
			timeline.put(e4);
			timeline.put(e2);
		}

		TimelineEvent e;

		e = timeline.get(); // Should be e1
		assert(e.value == 101);
		std::cout << e.value << std::endl;

		e = timeline.get(); // Should be e2
		assert(e.value == 102);
		std::cout << e.value << std::endl;

		e = timeline.get(); // Should be e3
		assert(e.value == 103);
		std::cout << e.value << std::endl;

		e = timeline.get(); // Should be e4
		assert(e.value == 104);
		std::cout << e.value << std::endl;

		e = timeline.get(); // Should be e5
		assert(e.value == 105);
		std::cout << e.value << std::endl;
	}

	std::cout << "Timeline module test passed" << std::endl;

	return 0;
}
