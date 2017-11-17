#pragma once
#include <cstddef>
#include <tuple>
#include <array>
#include <unordered_map>
#include <optional>
#include <list>
//types
template<class T> struct make_ptr_t { typedef T* type; };
template<class T> struct make_ptr_t<T*> { typedef T* type; };

template<std::size_t I = 0, typename... Types>
inline typename std::enable_if<I == sizeof...(Types), void>::type
writePtr_Tuple(std::tuple<typename make_ptr_t<Types>::type...>& t, size_t allocated)
{ }

template<std::size_t I = 0, class... Types>
inline typename std::enable_if<I < sizeof...(Types), void>::type
	writePtr_Tuple(std::tuple<typename make_ptr_t<Types>::type...>& t, size_t allocated)
{
	std::get<I>(t) = reinterpret_cast<std::tuple_element<I, std::tuple<typename make_ptr_t<Types>::type...>>::type>(std::get<I -1>(t) + allocated);
	writePtr_Tuple<I + 1, Types...>(t, allocated);
}




template<class Key, class KeyHash, class... Types>
class Sofa
{
public:
	Sofa(std::size_t size = 64) : used(0), allocated(size), byteWidth(0)
	{
		for (auto&& v : typeSizes)
			byteWidth += v;
		Allocate(allocated);
	}


	std::optional<std::pair<Key, std::size_t>> find(const Key key)const
	{
		if (auto const find = map.find(key); find != map.end())
			return { { find->first, find->second } };
		else
			return std::nullopt;
	}

	inline std::size_t size()const { return used; };

	void add(const Key key)
	{
		if (used + 1 > allocated)
			Allocate(allocated * 2);

		auto index = map[key] = used++;

	}

	template<std::size_t N, class type>
	inline void set(std::size_t index,const type& t)
	{
		std::get<N>(typePointers)[index] = t;
	}

	template<std::size_t N>
	inline auto& get(std::size_t index)
	{
		return std::get<N>(typePointers)[index];
	}

	void destroy(const Key key)
	{
		if (const auto find = map.find(key); find != map.end())
		{
			
		}
	}

private:
	void *data;
	std::size_t used;
	std::size_t allocated;
	std::size_t byteWidth;

	const std::array<std::size_t, sizeof...(Types) + 1> typeSizes { sizeof(Key), sizeof(Types)...};
	typedef std::tuple<typename make_ptr_t<Key>::type, typename make_ptr_t<Types>::type...> type;
	type typePointers;
	static const std::tuple<Types...> members;

	std::unordered_map<Key, std::size_t, KeyHash> map;
	
	type asd;
	void Allocate(std::size_t newSize)
	{
		std::size_t newAllocated = newSize;
		void* newData = operator new(newAllocated*byteWidth);
		type newTypePointers;

		std::get<0>(newTypePointers) = (std::tuple_element<0, type>::type) newData;
		writePtr_Tuple<1, Key, Types...>(newTypePointers, newAllocated);



		//newTypePointers[0] = newData;

		//for (std::size_t i = 1; i < newTypePointers.size(); i++)
		//	newTypePointers[i] =( void*)((std::size_t)newTypePointers[i - 1] + typeSizes[i-1]*newAllocated);
		//
		//for (std::size_t i = 0; i < newTypePointers.size(); i++)
		//	memcpy(newTypePointers[i], typePointers[i], typeSizes[i] * used);


		operator delete(data);
		typePointers = newTypePointers;
		allocated = newAllocated;
		data = newData;
		
		int i = 0;
	}

	void Destroy(std::size_t at)
	{

	}
};
