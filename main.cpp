#include <future>
#include <new>
#include <thread>
#include <iostream>
#include <chrono>
#include <array>


template<class T = void>
struct Allocator_with_buffer
{	using value_type = T;

	void* const buf;

	Allocator_with_buffer(void* const buffer) noexcept : buf(buffer){}

	template<class U>
	Allocator_with_buffer(Allocator_with_buffer<U> const& rhs) noexcept : buf(rhs.buf){}

	T* allocate(std::size_t const, void const* const = 0) const noexcept
	{	return reinterpret_cast<T*>(buf);  
	}

	static void deallocate(T* const, std::size_t const) noexcept{}

	template<class U, class...Args>
	static void construct(U* const ptr, Args&&...args) 
	noexcept(  noexcept( U{ decltype(args)(args)... } )  )
	{	new(ptr) U{ decltype(args)(args)... };  
	}

	template<class U>
	static void destroy(U* const ptr) noexcept
	{	if(ptr)
			ptr->~U();  
	}
};
//========//========//========//========//=======#//========//========//========//========//=======#


static void Example01()
{	std::byte buffer[1024];

	Allocator_with_buffer const alloc(buffer);
	std::promise< std::array<char, 8> > prom(std::allocator_arg, alloc);
	auto fut = prom.get_future();

	std::thread th
	(	[&prom]
		{	std::this_thread::sleep_for(std::chrono::milliseconds{100});

			prom.set_value({'L', 'O', 'V', 'E'});

			std::this_thread::sleep_for(std::chrono::milliseconds{100});

			std::cerr << "thread ends." << std::endl;
		}
	);

	for(auto const t : fut.get())
		std::cout << t;

	std::cout << std::endl;

	th.join();
}


int main( int const, char const*(&)[] )
{	Example01();
	
	return 0;
}