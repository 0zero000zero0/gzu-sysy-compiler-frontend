int main()
{
	int score1, score2;
	int i;
	int sum;

	score1 = 1;
	score2 = 2;

	if (score2 < score1 * 2 + 4)
	{
		i = 0;
		while (i < 10)
		{
			sum = sum + i * 2;
			i = i + 1;
		}
	}
	else
	{
		sum = 100;
	}

	return sum;
}