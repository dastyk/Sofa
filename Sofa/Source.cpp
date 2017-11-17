#include "Sofa.h"

int main()
{



	struct asd
	{
		int a[4];
	};
	Sofa<int, std::hash<int>, int, bool, char> sofa;

	for(int i = 0; i < 2; i++)
	if (auto const find = sofa.find(123); find.has_value())
	{
		sofa.set<0>(find->second, 1);
	}
	else
	{
		sofa.add(123);		
	}
	

	auto find = sofa.find(123);
	if (find.has_value())
		printf("%d", sofa.get<0>(find->second));



	return 0;
}