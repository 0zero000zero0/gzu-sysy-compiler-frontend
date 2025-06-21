int main()
{
	int a[10][2] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int sum;
	int i, j;

	i = 0;
	sum = 0;
	while (i < 10)
	{
		j = 0;
		while (j < 2)
		{
			sum = sum + a[i][j];
			j = j + 1;
		}
		i = i + 2;
	}
	return sum;
}
