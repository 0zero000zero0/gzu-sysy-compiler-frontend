int main()
{
	int i;
	int sum;

	i = 0;
	sum = 0;
	while (i < 10)
	{
		if (sum > 10)
			break;
		sum = sum + i;
		i = i + 1;
	}

	return sum;
}