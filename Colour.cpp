#include "Colour.h"

Colour::Colour() {
	m_data[0] = 0.0;
	m_data[1] = 0.0;
	m_data[2] = 0.0;
}

Colour::Colour(double r, double g, double b) { 
	m_data[0] = r;
	m_data[1] = g;
	m_data[2] = b;
}

Colour::Colour(const Colour& other) {
	m_data[0] = other.m_data[0];
	m_data[1] = other.m_data[1];
	m_data[2] = other.m_data[2];
}

Colour& Colour::operator =(const Colour& other) {
	m_data[0] = other.m_data[0];
	m_data[1] = other.m_data[1];
	m_data[2] = other.m_data[2];
	return *this;
}

Colour Colour::operator *(const Colour& other) {
	return Colour(m_data[0]*other.m_data[0], 
		m_data[1]*other.m_data[1], 
		m_data[2]*other.m_data[2]);
}

double& Colour::operator[](int i) {
	return m_data[i];
}
double Colour::operator[](int i) const {
	return m_data[i];
}

void Colour::clamp() {
	for (int i = 0; i < 3; i++) {
		if (m_data[i] > 1.0) m_data[i] = 1.0; 
		if (m_data[i] < 0.0) m_data[i] = 0.0; 
	}
}

Colour operator *(double s, const Colour& c)
{
  return Colour(s*c[0], s*c[1], s*c[2]);
}

Colour operator +(const Colour& u, const Colour& v)
{
  return Colour(u[0]+v[0], u[1]+v[1], u[2]+v[2]);
}

std::ostream& operator <<(std::ostream& s, const Colour& c)
{
  return s << "c(" << c[0] << "," << c[1] << "," << c[2] << ")";
}
