/*
 * IPC_IF.hpp
 *
 *  Created on: 2021/04/14
 *      Author: yuma
 */

#ifndef IPC_IF_HPP_
#define IPC_IF_HPP_

template <class T>
class IPC_IF {
public:
	IPC_IF(){}
	virtual ~IPC_IF(){}

	virtual bool read(T*) = 0;
	virtual bool write(T) = 0;
};



#endif /* IPC_IF_HPP_ */
