int main()
{
	int a, b, c;
	int max;

	a = 3;
	b = 5;
	c = 7;

	if (a > b)
	{
		max = a;
		if (c > max)
			max = c;
	}
	if (a <= b)
	{
		max = b;
		if (c > max)
			max = c;
	}

	return max;
}