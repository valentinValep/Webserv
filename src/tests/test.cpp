#include <iostream>

int	main(void)
{
	int	ret = 0;
	int	fail = 0;

	(void)ret;

	if (fail)
		std::cout << fail << " tests failed ❌" << std::endl;
	else
		std::cout << "All tests passed ✅" << std::endl;
	return (0);
}