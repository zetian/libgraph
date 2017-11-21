/* 
 * state_example.h
 * 
 * Created on: Apr 15, 2016
 * Description: 
 * 
 * Copyright (c) 2017 Ruixiang Du (rdu)
 */ 

#ifndef BDS_EXAMPLE_H
#define BDS_EXAMPLE_H

#include <cstdint>

namespace librav {

struct StateExample
{
	StateExample(uint64_t id):state_id_(id){};

	uint64_t state_id_;
};

}

#endif /* BDS_EXAMPLE_H */