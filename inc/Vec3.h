#pragma once

#include <memory>
#include <type_traits>
#include <cmath>
#include <iostream>

#define PI 3.14159265

enum AngleType{ 
	Radians,
	Degrees
};

enum SetType{
	X,
	Y
};

template<typename T = double>
class Vec3{
private:
	T x, y, z;

public:
	Vec3() : x(0), y(0), z(0){
		static_assert(std::is_arithmetic<T>::value, "Values must be numeric");
	}

	Vec3(const T X, const T Y, const T Z) : x(X), y(Y), z(Z){
		static_assert(std::is_arithmetic<T>::value, "Values must be numeric");
	}
	~Vec3(){}

	// declarations

	template<typename N = double> Vec3<T>  operator+ (const Vec3<N> &U);
	template<typename N = double> Vec3<T>& operator+=(const Vec3<N> &U);
	template<typename N = double> Vec3<T>  operator- (const Vec3<N> &U);
	template<typename N = double> Vec3<T>& operator-=(const Vec3<N> &U);
	template<typename N = double> Vec3<T>  operator* (const N &R);
	template<typename N = double> Vec3<T>  operator* (const Vec3<N> &R);
	template<typename N = double> Vec3<T>& operator*=(const N &R);
	template<typename N = double> Vec3<T>& operator*=(const Vec3<N> &R);
	template<typename N = double> Vec3<T>& operator/=(const N &R);
	template<typename N = double> Vec3<T>& operator/=(const Vec3<N> &R);

	template<typename N = double> bool operator==(const Vec3<N> &U) const;
	template<typename N = double> bool operator>=(const Vec3<N> &U) const;
	template<typename N = double> bool operator<=(const Vec3<N> &U) const;
	template<typename N = double> bool operator> (const Vec3<N> &U) const;
	template<typename N = double> bool operator< (const Vec3<N> &U) const;

	void operator() (const T &X, const T &Y, const T &Z);

	Vec3<double> inverse() const;
	Vec3<T> negative() const;
	double length() const;
	double lengthsquared() const;
	template<typename N = double> double distance(const Vec3<N> &U) const;
	template<typename N = double> double dotproduct(const Vec3<N> &U) const;
	template<AngleType degrees = Degrees, typename N = double> double angle(const Vec3<N> &U) const;

	T X() const{
		return x;
	}

	void X(T val){
		x = val;
	}

	T Y() const{
		return y;
	}

	void Y(T val){
		y = val;
	}

    T Z() const{
		return z;
	}

	void Z(T val){
		z = val;
	}


	void print(){
		std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl;
	}

	template<typename Cast> operator Vec3<Cast> () {
		return Vec3<Cast>((Cast)this->x, (Cast)this->y, (Cast)this->z);
	}
};

// definitions

// addition
template<typename T>
template<typename N>
Vec3<T> Vec3<T>::operator+(const Vec3<N> &U){
	return Vec3<T>(this->x + U.X(), this->y + U.Y(), this->z + U.Z());
}

template<typename T>
template<typename N>
Vec3<T>& Vec3<T>::operator+=(const Vec3<N> &U){
	this->x += U.X();
	this->y += U.Y();
    this->z += U.Z();
	return *this;
}

// subtraction
template<typename T>
template<typename N>
Vec3<T> Vec3<T>::operator-(const Vec3<N> &U){
	return Vec3<T>(this->x - U.X(), this->y - U.Y(), this->z - U.Z());
}

template<typename T>
template<typename N>
Vec3<T>& Vec3<T>::operator-=(const Vec3<N> &U){
	this->x -= U.X();
	this->y -= U.Y();
    this->z -= U.Z();
	return *this;
}

// multiplication
template<typename T>
template<typename N>
Vec3<T> Vec3<T>::operator*(const N &R){
	static_assert(std::is_arithmetic<N>::value, "Type must be numeric or Vector<T>");
	return Vec3<T>(this->x * R, this->y * R, this->z * R);
}

template<typename T>
template<typename N>
Vec3<T> Vec3<T>::operator*(const Vec3<N> &R){
	return Vec3<T>(this->x * R.X(), this->y * R.Y(), this->z * R.Z());
}

template<typename T>
template<typename N>
Vec3<T>& Vec3<T>::operator*=(const N &R){
	static_assert(std::is_arithmetic<N>::value, "Type must be numeric or Vector<T>");
	this->x *= R;
	this->y *= R;
    this->z *= R;
	return *this;
}

template<typename T>
template<typename N>
Vec3<T>& Vec3<T>::operator*=(const Vec3<N> &R){
	this->x *= R.X();
	this->y *= R.Y();
    this->z *= R.Z();
	return *this;
}

// division
template<typename T, typename N>
Vec3<T> operator/(const Vec3<T>& V, const N &R){
	static_assert(std::is_arithmetic<N>::value, "Type must be numeric or Vector<T>");
	return Vec3<T>(V.X() / R, V.Y() / R, V.Z() / R);
}

template<typename T, typename N>
Vec3<T> operator/(const N &R, const Vec3<T>& V){
	static_assert(std::is_arithmetic<N>::value, "Type must be numeric or Vector<T>");
	return Vec3<T>(R / V.X(), R / V.Y());
}

template<typename T, typename N>
Vec3<T> operator/(const Vec3<T>& V, const Vec3<N> &R){
	return Vec3<T>(V.X() / R.X(), V.Y() / R.Y(), V.Z() / R.Z());
}

template<typename T>
template<typename N>
Vec3<T>& Vec3<T>::operator/=(const N &R){
	static_assert(std::is_arithmetic<N>::value, "Type must be numeric or Vector<T>");
	this->x /= R;
	this->y /= R;
    this->z /= R;
	return *this;
}

template<typename T>
template<typename N>
Vec3<T>& Vec3<T>::operator/=(const Vec3<N> &R){
	// V /= U
	this->x /= R.X();
	this->y /= R.Y();
    this->z /= R.Z();
	return *this;
}

// logical
template<typename T>
template<typename N>
bool Vec3<T>::operator==(const Vec3<N> &U) const{
	return (this->x == U.X()) && (this->y == U.Y()) && (this->z == U.Z());
}
template<typename T>
template<typename N>
bool Vec3<T>::operator>=(const Vec3<N> &U) const{
	return (this->x >= U.X()) && (this->y >= U.Y()) && (this->z >= U.Z());
}
template<typename T>
template<typename N>
bool Vec3<T>::operator<=(const Vec3<N> &U) const{
	return (this->x <= U.X()) && (this->y <= U.Y()) && (this->z <= U.Z());
}
template<typename T>
template<typename N>
bool Vec3<T>::operator>(const Vec3<N> &U) const{
	return (this->x > U.X()) && (this->y > U.Y()) && (this->z > U.Z());
}
template<typename T>
template<typename N>
bool Vec3<T>::operator<(const Vec3<N> &U) const{
	return (this->x < U.X()) && (this->y < U.Y()) && (this->z < U.Z());
}

template<typename T>
Vec3<double> Vec3<T>::inverse() const{
	return Vec3<double>(1/(double)this->x, 1/(double)this->y, 1/(double)this->z);
}

template<typename T>
Vec3<T> Vec3<T>::negative() const{
	return Vec3<T>(-this->x, -this->y, -this->z);
}

template<typename T>
template<typename N>
double Vec3<T>::dotproduct(const Vec3<N> &U) const{
	return (this->x * U.X()) + (this->y * U.Y()) + (this->z * U.Z());
}

template<typename T>
double Vec3<T>::length() const{
	return std::sqrt(std::pow((double)this->x, 2.0) + std::pow((double)this->y, 2.0) + std::pow((double)this->z, 2.0));
}

template<typename T>
double Vec3<T>::lengthsquared() const{
	return std::pow((double)this->x, 2.0) + std::pow((double)this->y, 2.0) + std::pow((double)this->z, 2.0);
}

template<typename T>
template<AngleType degrees, typename N>
double Vec3<T>::angle(const Vec3<N> &U) const{
	return std::acos(
		this->dotproduct(U) / std::sqrt(this->lengthsquared() * U.lengthsquared())
	) * ((bool)degrees ? (180.0 / PI) : 1);
}

template<typename T>
template<typename N>
double Vec3<T>::distance(const Vec3<N> &U) const{
	return (*this - U).length();
}

template<typename T>
void Vec3<T>::operator() (const T &X, const T &Y, const T &Z){
	x = X;
	y = Y;
    z = Z;
}