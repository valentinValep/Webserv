#include "utils.hpp"
#include <cstdlib>
#include <cstring>

bool containsEmptyLine(const std::string& data) {
	return (data.find("\r\n\r\n") != std::string::npos);
}

int	detect_int_overflow(const char *input)
{
	size_t	sign = 0;
	int		i;

	if (input[0] == '+' || input[0] == '-')
		sign++;
	if (strlen(input) > 10 + sign)
		return (1);
	if (strlen(input) == 10 + sign)
	{
		if (input[sign] > '2')
			return (1);
		else if (input[sign] == '2')
		{
			i = atoi(input + sign + 1);
			if (i > 147483647 + (input[0] == '-'))
				return (1);
			return (0);
		}
	}
	return (0);
}

int	is_number(const char *input)
{
	int	i;

	i = 0;
	if (input[i] == '+' || input[i] == '-')
		i++;
	while (input[i])
	{
		if (input[i] < '0' || input[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

// INT_MAX = 2147483647
// INT_MIN = -2147483648
int	anti_overflow_atoi(const char *input, int *result)
{
	if (!input || !result)
		return (1);
	if (strlen(input) == 0)
		return (1);
	if (is_number(input) == 0)
		return (1);
	if (detect_int_overflow(input))
		return (1);
	*result = atoi(input);
	return (0);
}

std::string		trimTrailingSlashes(std::string path)
{
	size_t	index;

	if (!(path.empty()) && ((index = path.find_last_not_of("/")) != path.npos))
			path.erase(index + 1);
	return (path);
}
