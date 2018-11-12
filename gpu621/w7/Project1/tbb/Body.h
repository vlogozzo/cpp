#pragma once	
#include "tbb/tbb.h"

template <typename T, typename C>
class Body {
private:
	T accumul;
	T* const out;
	const T* const in;
	const T identity;
	const C combine;
public:
	Body(T* out_, const T* in_, T i, C c) :
		accumul(i), out(out_), in(in_),
		identity(i), combine(c) {}

	Body(Body& b, tbb::split) :
		accumul(b.identity), out(b.out), in(b.in),
		identity(b.identity), combine(b.combine) {}

	template<typename Tag>
	void operator()(
		const tbb::blocked_range<int>& r, Tag) {
		T temp = accumul;
		for (int i = r.begin(); i != r.end();
			i++) {
			temp = combine(temp, in[i]);
			if (Tag::is_final_scan())
				out[i] = temp;
		}
		accumul = temp;
	}

	void reverse_join(Body& a) {
		accumul = combine(accumul, a.accumul);
	}
	void assign(Body& b) { accumul = b.accumul; }
	T get_accumul()
	{
		return this->accumul;
	}

};