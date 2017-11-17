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
setupPointers(std::tuple<typename make_ptr_t<Types>::type...>& t, size_t allocated)
{ }

template<std::size_t I = 0, class... Types>
inline typename std::enable_if<I < sizeof...(Types), void>::type
	setupPointers(std::tuple<typename make_ptr_t<Types>::type...>& t, size_t allocated)
{
	std::get<I>(t) = reinterpret_cast<std::tuple_element<I, std::tuple<typename make_ptr_t<Types>::type...>>::type>(std::get<I -1>(t) + allocated);
	setupPointers<I + 1, Types...>(t, allocated);
}

template<std::size_t I = 0, typename... Types>
inline typename std::enable_if<I == sizeof...(Types), void>::type
setValue(std::tuple<typename make_ptr_t<Types>::type...>& tp, const std::tuple<Types...>& t, std::size_t index)
{ }

template<std::size_t I = 0, class... Types>
inline typename std::enable_if<I < sizeof...(Types), void>::type
	setValue(std::tuple<typename make_ptr_t<Types>::type...>& tp,const std::tuple<Types...>& t, std::size_t index)
{
	std::get<I>(tp)[index] = std::get<I>(t);
	setValue<I + 1, Types...>(tp, t, index);
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

	void clear()
	{
		used = 0;
	}
	inline std::optional<std::pair<Key, std::size_t>> find(const Key key)const
	{
		if (auto const find = map.find(key); find != map.end())
			return { { find->first, find->second } };
		else
			return std::nullopt;
	}

	inline std::size_t size()const { return used; };

	inline void add(const Key key, const Types... args)
	{
		if (used + 1 > allocated)
			Allocate(allocated * 2);
		auto index=  map[key] = used++;
		const auto tpl = std::make_tuple(key, args... );
		setValue<0, Key, Types...>(typePointers, tpl, index);
	}

	inline std::size_t add(const Key key)
	{
		if (used + 1 > allocated)
			Allocate(allocated * 2);
		return map[key] = used++;
	}

	template<std::size_t N, class type>
	inline void set(std::size_t index,const type&& t)
	{
		std::get<N>(typePointers)[index] = t;
		//((type*)aTypePointers[N])[index] = std::move(t);
	}

	template<std::size_t N>
	inline auto& get(std::size_t index)
	{
		return std::get<N>(typePointers)[index];
		//using type = typename std::tuple_element<N, std::tuple<Types...>>::type;
		//return ((type*)aTypePointers[N])[index];
	}

	void erase(const Key key)
	{
		if (const auto find = map.find(key); find != map.end())
		{
			
		}
	}
	typedef std::tuple<typename make_ptr_t<Key>::type, typename make_ptr_t<Types>::type...> type;
	type typePointers;
private:
	void *data;
	std::size_t used;
	std::size_t allocated;
	std::size_t byteWidth;

	const std::array<std::size_t, sizeof...(Types) + 1> typeSizes { sizeof(Key), sizeof(Types)...};
	std::array<void*, sizeof...(Types)+1> aTypePointers{ nullptr };

	
	static const std::tuple<Types...> members;

	std::unordered_map<Key, std::size_t, KeyHash> map;
	
	type asd;
	void Allocate(std::size_t newSize)
	{
		std::size_t newAllocated = newSize;
		void* newData = operator new(newAllocated*byteWidth);
		type newTypePointers;

	/*	std::array<void*, sizeof...(Types)+1> newatp;

		newatp[0] = newData;
		setupAP<1, Key, Types...>(newatp);

		memcpyAP<0, Key, Types...>(newatp, aTypePointers);

		aTypePointers = newatp;*/

		std::get<0>(newTypePointers) = (std::tuple_element<0, type>::type) newData;
		setupPointers<1, Key, Types...>(newTypePointers, newAllocated);

		memcpyTuple<0, Key, Types...>(newTypePointers, typePointers);

		operator delete(data);
		typePointers = newTypePointers;
		allocated = newAllocated;
		data = newData;
		
		int i = 0;
	}

	void Destroy(std::size_t at)
	{

	}


	template<std::size_t I = 0, typename... Types>
	inline typename std::enable_if<I == sizeof...(Types), void>::type
		memcpyTuple(std::tuple<typename make_ptr_t<Types>::type...>& t1, std::tuple<typename make_ptr_t<Types>::type...>& t2)
	{ }

	template<std::size_t I = 0, class... Types>
	inline typename std::enable_if<I < sizeof...(Types), void>::type
		memcpyTuple(std::tuple<typename make_ptr_t<Types>::type...>& t1, std::tuple<typename make_ptr_t<Types>::type...>& t2)
	{
		memcpy(std::get<I>(t1), std::get<I>(t2), typeSizes[I] * used);
		memcpyTuple<I + 1, Types...>(t1, t2);
	}

	template<std::size_t I = 0, typename... Types>
	inline typename std::enable_if<I == sizeof...(Types), void>::type
		memcpyAP(std::array<void*, sizeof...(Types)>&ap, std::array<void*, sizeof...(Types)>&ap2)
	{ }

	template<std::size_t I = 0, class... Types>
	inline typename std::enable_if<I < sizeof...(Types), void>::type
		memcpyAP(std::array<void*, sizeof...(Types)>&ap, std::array<void*, sizeof...(Types)>&ap2)
	{
		memcpy(ap[I], ap2[I], typeSizes[I] * used);
		memcpyAP<I + 1, Types...>(ap, ap2);
	}


	template<std::size_t I = 0, typename... Types>
	inline typename std::enable_if<I == sizeof...(Types), void>::type
		setupAP(std::array<void*, sizeof...(Types)>&ap)
	{ }

	template<std::size_t I = 0, class... Types>
	inline typename std::enable_if<I < sizeof...(Types), void>::type
		setupAP(std::array<void*, sizeof...(Types)>&ap)
	{
		ap[I] = (char*)ap[I - 1] + typeSizes[I - 1] * allocated;
		setupAP<I + 1, Types...>(ap);
	}

};