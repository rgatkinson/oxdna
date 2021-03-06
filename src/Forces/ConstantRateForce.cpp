/*
 * ExternalForce.cpp
 *
 *  Created on: 18/oct/2011
 *      Author: Flavio 
 */

#include "ConstantRateForce.h"
#include "../Particles/BaseParticle.h"

template<typename number>
ConstantRateForce<number>::ConstantRateForce() : BaseForce<number>() {
	_particle = 0;
}

template<typename number>
ConstantRateForce<number>::~ConstantRateForce() {

}

template<typename number>
void ConstantRateForce<number>::get_settings(input_file &inp) {
	getInputInt (&inp, "particle", &this->_particle, 1);

	getInputNumber (&inp, "F0", &this->_F0, 1);
	getInputNumber (&inp, "rate", &this->_rate, 1);

	std::string strdir;
	getInputString (&inp, "dir", strdir, 1);
	double x, y, z;
	int tmpi = sscanf(strdir.c_str(), "%lf,%lf,%lf", &x, &y, &z);
	if (tmpi != 3) throw oxDNAException("could not parse direction in external_forces_file. Dying badly");
	this->_direction = LR_vector<number> ((number) x, (number) y, number (z));
	this->_direction.normalize();
}

template<typename number>
void ConstantRateForce<number>::init(BaseParticle<number> **particles, int N, number *box_side) {
	if (_particle >= N || N < -1) throw oxDNAException ("Trying to add a ConstantRateForce on non-existent particle %d. Aborting", _particle);
	if (_particle != -1) {
		OX_LOG (Logger::LOG_INFO, "Adding ConstantRateForce (F==%g, rate=%g, dir=%g,%g,%g on particle %d", this->_F0, this->_rate, this->_direction.x, this->_direction.y, this->_direction.z, _particle);
		particles[_particle]->add_ext_force(this);
	}
	else { // force affects all particles
		OX_LOG (Logger::LOG_INFO, "Adding ConstantRateForce (F==%g, rate=%g, dir=%g,%g,%g on ALL particles", this->_F0, this->_rate, this->_direction.x, this->_direction.y, this->_direction.z);
		for (int i = 0; i < N; i ++) particles[i]->add_ext_force(this);
	}
}

template<typename number>
LR_vector<number> ConstantRateForce<number>::value(llint step, LR_vector<number> &pos) {
	number x = (this->_F0 + this->_rate * step) * this->_direction.x;
	number y = (this->_F0 + this->_rate * step) * this->_direction.y;
	number z = (this->_F0 + this->_rate * step) * this->_direction.z;
	return LR_vector<number>(x, y, z);
}

template<typename number>
number ConstantRateForce<number>::potential(llint step, LR_vector<number> &pos) {
	return (number) -(this->_F0 + this->_rate * step) * (pos * this->_direction);
}

template class ConstantRateForce<double>;
template class ConstantRateForce<float>;

