/*
 *  Multi2Sim
 *  Copyright (C) 2014  Yifan Sun (yifansun@coe.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef ARCH_HSA_EMU_COMPONENT_H
#define ARCH_HSA_EMU_COMPONENT_H

#include <string>
#include <list>
#include <memory>

#include "Emu.h"
#include "AQLQueue.h"
#include "Grid.h"


namespace HSA
{

class Grid;

/// An HSA component is an HSA agent that support HSAIL virtual ISAs
class Component
{
protected:

	// A data structure holds the information of the agent
	struct AgentInfo{
		// The global unique 64-bit device handler
		unsigned long long handler;

		// Determine if the device is a GPU device
		bool is_GPU;

		// Name of the device
		std::string name;

		// Name of the vendor
		std::string vendor_name;

		// Number of work items in a wavefront
		unsigned int wavesize;
	};

	// Information of current device
	AgentInfo agent_info;

	// List of work groups
	std::list<std::unique_ptr<Grid>> grids;

	// List of queues associated with this component
	std::list<std::unique_ptr<AQLQueue>> queues;

public:

	/// Constructor
	Component(unsigned long long handler)
	{
		this->agent_info.handler = handler;
	};

	/// Create and return a virtual CPU device
	static Component *getDefaultCPUComponent(unsigned long long handler);

	/// Create and return a virtual GPU device
	static Component *getDefaultGPUComponent(unsigned long long handler);

	/// Insert a queue into the queue list
	void addQueue(AQLQueue *queue);

	/// Execute instructions on this components
	///
	/// \return
	///	Returns false if the component have no ongoing tasks and
	/// 	no more tasks to be processed. When all components finish
	///	their tasks, the emulation finishes.
	bool Execute();

	/// Create a grid from a dispatch packet
	void LaunchGrid(AQLDispatchPacket *packet);

	/// Dump the information about the agent
	void Dump(std::ostream &os) const;

	/// Operator \c << invoking the function Dump) on an output stream
	friend std::ostream &operator<<(std::ostream &os,
			const Component &component)
	{
		component.Dump(os);
		return os;
	}





	//
	// Setters and getters for agent_info
	//

	/// Get handler
	unsigned long long getHandler() { return agent_info.handler; }

	/// Set is_GPU field in agent_info
	void setIsGPU(bool is_GPU){ agent_info.is_GPU = is_GPU; }

	/// Get is_GPU field of agent_info
	bool IsGPU(){ return agent_info.is_GPU; }

	/// Set name field in agent_info
	void setName(const std::string &name){ agent_info.name = name; }

	/// Get name field of agent_info
	std::string getName(){ return agent_info.name; }

	/// Set vendor_name field of agent_info
	void setVendorName(const std::string &vendor_name)
	{
		agent_info.vendor_name = vendor_name;
	}

	/// Get vendor_name of the device
	std::string getVendorName(){ return agent_info.vendor_name; }

	/// Set the number of work items in a wavefront
	void setWavesize(unsigned int wavesize)
	{
		agent_info.wavesize = wavesize;
	}

	/// Get the number of work items in a wavefront
	unsigned int getWavesize(){ return agent_info.wavesize; }


};

}

#endif

