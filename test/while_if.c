int main()
{
int count, num, i;
count = 0;
num = 1;
while (num < 9527){
	i = num;
	while (i > 0){
		i = i - 2;
	}
	
	if (i == 0)
		count = count + 1;
	num = num + 1;
}
return count;
}
