#include "Sofa.h"
#include <chrono>




struct RenderableObjectData
{
	static const size_t size = sizeof(int) + sizeof(int) + sizeof(bool) + sizeof(char);
	size_t allocated = 0;
	size_t used = 0;
	void* data = nullptr;
	int* key;
	int* v1;
	bool* v2;
	char* v3;
};

RenderableObjectData renderableObjectInfo;
std::unordered_map<int, size_t, std::hash<int>> entityToRenderableObjectInfoIndex;






void Allocate(size_t size)
{
	_ASSERT(size > renderableObjectInfo.allocated);

	// Allocate new memory
	RenderableObjectData newData;
	newData.allocated = size;
	newData.data = operator new(size * RenderableObjectData::size);
	newData.used = renderableObjectInfo.used;

	// Setup the new pointers
	newData.key = (int*)newData.data;
	newData.v1 = (int*)(newData.key + newData.allocated);
	newData.v2 = (bool*)(newData.v1 + newData.allocated);
	newData.v3 = (char*)(newData.v2 + newData.allocated);

	// Copy data
	memcpy(newData.key, renderableObjectInfo.key, renderableObjectInfo.used * sizeof(int));
	memcpy(newData.v1, renderableObjectInfo.v1, renderableObjectInfo.used * sizeof(int));
	memcpy(newData.v2, renderableObjectInfo.v2, renderableObjectInfo.used * sizeof(bool));
	memcpy(newData.v3, renderableObjectInfo.v3, renderableObjectInfo.used * sizeof(char));

	// Delete old data;
	operator delete(renderableObjectInfo.data);
	renderableObjectInfo = newData;
}






int main()
{
	Allocate(64);


	struct asd
	{
		int a[4];
	};
	Sofa<int, std::hash<int>, int, bool, char> sofa;
	Sofa<int, std::hash<int>, int, bool, char> sofa2;
	Sofa<int, std::hash<int>, int, bool, char> sofa3;
	static const std::size_t count = 10000000;
	//{
	//	auto s = std::chrono::high_resolution_clock::now();
	//	for (int i = 0; i < count; i++)
	//	{
	//		sofa2.add(i, i * 10, i % 2, 'c');
	//	}
	//	auto e = std::chrono::high_resolution_clock::now();
	//	auto diff = std::chrono::duration<float, std::milli>(e - s).count();
	//	printf("Add Sofa T: %f\n", diff);
	//}
	/*{
		auto s = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < count; i++)
		{
			auto index = sofa3.add(i);
			std::get<0>(sofa3.typePointers)[index] = i * 10;
			std::get<1>(sofa3.typePointers)[index] = i % 2;
			std::get<2>(sofa3.typePointers)[index] = 'c';
		}
		auto e = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration<float, std::milli>(e - s).count();
		printf("Add Sofa P: %f\n", diff);
	}*/

	{
		auto s = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < count; i++)
		{
			auto index = sofa.add(i);
			sofa.set<0>(index, i * 10);
			sofa.set<1>(index, i % 2);
			sofa.set<2>(index, 'c');
		}
		auto e = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration<float, std::milli>(e - s).count();
		printf("Add Sofa: %f\n", diff);
	}
	
	
	{
		auto s = std::chrono::high_resolution_clock::now();
		auto sum = 0;
		for (size_t i = 0; i < sofa.size(); i++)
		{
		//	printf("i: %d ", sofa.get<0>(i));
			//printf("b: %s\n", sofa.get<1>(i) ? "true" : "false");
			sum += sofa.get<2>(i) ? sofa.get<1>(i) : 1337;
		}
		auto e = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration<float, std::milli>(e - s).count();
		printf("Iter Sofa: %f, %d\n", diff, sum);
	}
	{
		auto s = std::chrono::high_resolution_clock::now();
		auto sum = 0;
		for (size_t i = 0; i < sofa.size(); i++)
		{
			if (auto const find = sofa.find(i); find.has_value())
				sum += sofa.get<2>(find->second) ? sofa.get<1>(find->second) : 1337;
		}
		auto e = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration<float, std::milli>(e - s).count();
		printf("Find sofa: %f, %d\n", diff, sum);
	}
	{
		auto s = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < count; i++)
		{
			if (renderableObjectInfo.used + 1 > renderableObjectInfo.allocated)
				Allocate(renderableObjectInfo.allocated * 2);
			auto index = entityToRenderableObjectInfoIndex[i] = renderableObjectInfo.used++;
			renderableObjectInfo.key[index] = i;
			renderableObjectInfo.v1[index] = i*10;
			renderableObjectInfo.v2[index] = i % 2;
			renderableObjectInfo.v3[index] = 'c';
		}
		auto e = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration<float, std::milli>(e - s).count();
		printf("Add: %f\n", diff);
	}
	

	{
		auto s = std::chrono::high_resolution_clock::now();
		auto sum = 0;
		for (size_t i = 0; i < renderableObjectInfo.used; i++)
		{
	
			sum += renderableObjectInfo.v2[i] ? renderableObjectInfo.v1[i] : 1337;
		}
		auto e = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration<float, std::milli>(e - s).count();
		printf("Iter: %f, %d\n", diff, sum);
	}

	{
		auto s = std::chrono::high_resolution_clock::now();
		auto sum = 0;
		for (size_t i = 0; i < renderableObjectInfo.used; i++)
		{
			if (auto const find = entityToRenderableObjectInfoIndex.find(i); find != entityToRenderableObjectInfoIndex.end())
				sum += renderableObjectInfo.v2[find->second] ? renderableObjectInfo.v1[find->second] : 1337;
		}
		auto e = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration<float, std::milli>(e - s).count();
		printf("Find: %f, %d\n", diff, sum);
	}
	getchar();
	return 0;
}