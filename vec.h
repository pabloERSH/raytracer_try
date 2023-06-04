#pragma once
#include <cmath>
#include <vector>
#include <iostream>
#include<exception>

template <size_t dim, typename T> struct vec {
private:
	T data[dim];
public:

	T &operator [](const size_t i) {
		try {
			if (i > dim) {
				throw std::invalid_argument("Ошибка!!! Зашли за границу массива!");
			}
		}
		catch (std::exception const& ex) {
			std::cout << ex.what() << std::endl;
		}
		return data[i];
	}

	const T &operator [](const size_t i) const {
		try {
			if (i > dim) {
				throw std::invalid_argument("Ошибка!!! Зашли за границу массива!");
			}
		}
		catch (std::exception const& ex) {
			std::cout << ex.what() << std::endl;
		}
		return data[i];
	}

	static vec<dim, T> fill(const T& num = 0) {
		vec<dim, T> res;
		for (size_t i = dim; i--; res[i] = num);
		return res;
	}

	T norm() const {
		return std::sqrt((*this) * (*this));
	}

	vec<dim, T> normalize() const{
		return (*this) / norm();
	}
};

template<size_t dim, typename T> T operator *(const vec<dim, T> &lv, const vec<dim, T>&pv) {
	T res = 0;
	for (size_t i = dim; i--; res += lv[i] * pv[i]);
	return res;
}

template<size_t dim, typename T>vec<dim, T> operator +(vec<dim, T> lv, const vec<dim, T>&pv) {
	for (size_t i = dim; i--; lv[i] += pv[i]);
	return lv;
}

template<size_t dim, typename T>vec<dim, T> operator -(vec<dim, T> lv, const vec<dim, T>&pv) {
	for (size_t i = dim; i--; lv[i] -= pv[i]);
	return lv;
}

template<size_t dim, typename T>vec<dim, T> operator *(vec<dim, T> lv, const T num) {
	for (size_t i = dim; i--; lv[i] *= num);
	return lv;
}

template<size_t dim, typename T>vec<dim, T> operator /(vec<dim, T> lv, const T num) {
	for (size_t i = dim; i--; lv[i] /= num);
	return lv;
}

template<size_t len, size_t dim, typename T> vec<len, T> embed(const vec<dim, T> &v, const T &fill = 1 ) {
	vec<len, T> res = vec<len, T>::fill(fill);
	for (size_t i = dim; i--; res[i] = v[i]);
	return res;
}

template<size_t len, size_t dim, typename T> vec<len, T> proj(const vec<dim, T>& v, const T& fill = 1) {
	vec<len, T> res;
	for (size_t i = len; i--; res[i] = v[i]);
	return res;
}

template <size_t dim, typename T> std::ostream& operator<<(std::ostream& out, const vec<dim, T>& v) {
	out << "{";
	for (unsigned int i = 0; i < dim; i++) {
		out << v[i] << " ";
	}
	out << "}";
	return out;
}

typedef vec<2, float> vec2f;
typedef vec<3, float> vec3f;
typedef vec<3, int> vec3i;
typedef vec<4, float> vec4f;

//template <typename T> struct vec<2, T> {
//	vec() : x(T()), y(T()) {}
//	vec(T X, T Y) : x(X), y(Y) {}
//
//	T& operator [](const size_t i) {
//		assert(i < 2);
//		return i <= 0 ? x : y;
//	}
//
//	const T& operator [](const size_t i) const {
//		assert(i < 2);
//		return i <= 0 ? x : y;
//	}
//private:
//	T x, y;
//};
//
template <typename T> struct vec<3, T> {
	vec() : x(T()), y(T()), z(T()) {}
	vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}

	T& operator [](const size_t i) {
		try {
			if (i > 3) {
				throw std::invalid_argument("Ошибка!!! Зашли за границу массива!");
			}
		}
		catch (std::exception const& ex) {
			std::cout << ex.what() << std::endl;
		}
		return i <= 0 ? x : (1==i? y : z);
	}

	const T& operator [](const size_t i) const {
		try {
			if (i > 3) {
				throw std::invalid_argument("Ошибка!!! Зашли за границу массива!");
			}
		}
		catch (std::exception const& ex) {
			std::cout << ex.what() << std::endl;
		}
		return i <= 0 ? x : (1 == i ? y : z);
	}

private:
	T x, y, z;
};
//
//template <typename T> struct vec<4, T> {
//	vec() : x(T()), y(T()), z(T()), w(T()) {}
//	vec(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) {}
//
//	T& operator[](const size_t i) { 
//		assert(i < 4);
//		return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w)); 
//	}
//
//	const T& operator[](const size_t i) const { 
//		assert(i < 4); 
//		return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w)); 
//	}
//private:
//	T x, y, z, w;
//};
