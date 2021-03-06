
#pragma once

#include "mLibInclude.h"

#include <random>

struct SimpleMaterial : public BinaryDataSerialize<SimpleMaterial> {
	SimpleMaterial() {
		ambient = vec4f::origin;
		diffuse = vec4f::origin;
		specular = vec4f::origin;
		shiny = 0.0f;
	}

	void saveToFile(const std::string& filename) const {
		std::ofstream out(filename);

		out << ambient << std::endl;
		out << diffuse << std::endl;
		out << specular << std::endl;
		out << shiny << std::endl;

		out.close();
	}

	void loadFromFile(const std::string& filename) {
		std::ifstream in(filename);

		if (!in.is_open()) throw MLIB_EXCEPTION(__FUNCTION__);

		in >> ambient;
		in >> diffuse;
		in >> specular;
		in >> shiny;

		in.close();
	}


	// material parameters (standard phong...)
	vec4f ambient;
	vec4f diffuse;
	vec4f specular;
	float shiny;

	
	enum RANDOM_FLAGS {
		AMBIENT = (1 << 0),
		DIFFUSE = (1 << 1),
		SPECULAR = (1 << 2)
	};

	// random normal distribution and clamped
	static float rndNC(float mu, float sd, float minV = 0.0f, float maxV = 1.0f) {
		static std::default_random_engine generator;
		std::normal_distribution<float> distribution(mu, sd);

		float res = -1.0f;
		while (res < minV || res > maxV) {
			res = distribution(generator);
		}
		return res;
	}

	static SimpleMaterial randomMaterial(unsigned int flags = DIFFUSE | AMBIENT | SPECULAR) {

		RNG rng;		
		SimpleMaterial m;

		if (flags & AMBIENT) {
			float a = rndNC(0.1f, 0.1f);
			m.ambient = vec4f(a, a, a, 1.0f);
		}
		if (flags & DIFFUSE) {
			float mu = 0.5f;	float sd = 0.5f;
			m.diffuse = vec4f(rndNC(mu, sd), rndNC(mu, sd), rndNC(mu, sd), 1.0f);
		}
		if (flags & SPECULAR) {
			float mu = 0.5f;	float sd = 0.5f;
			float s = rndNC(mu, sd);
			m.specular = vec4f(s, s, s, 1.0f);
			m.shiny = rndNC(15.0f, 10.0f, 0.5f, 50.0f);
		}

		return m;
	}

	static SimpleMaterial default() {
		SimpleMaterial m;
		m.ambient = vec4f(0.1f, 0.1f, 0.1f, 1.0f);
		m.diffuse = vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		m.specular = vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		m.shiny = 30.0f;
		return m;
	}

	static SimpleMaterial redDiffuse() {
		SimpleMaterial m;
		m.ambient = vec4f(0.1f, 0.1f, 0.1f, 1.0f);
		m.diffuse = vec4f(1.0f, 0.0f, 0.0f, 1.0f);
		m.specular = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		m.shiny = 0.0f;
		return m;
	}
	static SimpleMaterial greenDiffuse() {
		SimpleMaterial m;
		m.ambient = vec4f(0.1f, 0.1f, 0.1f, 1.0f);
		m.diffuse = vec4f(0.0f, 1.0f, 0.0f, 1.0f);
		m.specular = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		m.shiny = 0.0f;
		return m;
	}

};

std::ostream& operator<<(std::ostream& s, const SimpleMaterial& m);