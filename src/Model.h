/*
 * Model.h
 *
 *  Created on: Dec 18, 2013
 *      Author: azmy
 */
#include "include/Angel.h"
#ifndef MODEL_H_
#define MODEL_H_

namespace std {

class Model {
public:
	vec3 pos;
	vec3 scale;
	vec3 angles;
	Model();
	virtual ~Model();
};

} /* namespace std */
#endif /* MODEL_H_ */
