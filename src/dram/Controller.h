/*
 * Multi2Sim
 * Copyright (C) 2014 Agamemnon Despopoulos (agdespopoulos@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef DRAM_CONTROLLER_H
#define DRAM_CONTROLLER_H

#include <memory>
#include <vector>
#include <queue>

#include <lib/cpp/IniFile.h>


namespace dram
{

// Forward declarations
class Channel;
class Request;


// Definitions of lookup values for timing tables
// The timing table would be defined as having dimensions:
//	[TimingCommand][TimingCommand][TimingLocation][TimingLocation]
enum TimingCommand
{
	TimingPrecharge = 0,
	TimingActivate,
	TimingRead,
	TimingWrite
};

enum TimingLocation
{
	TimingSame = 0,
	TimingDifferent,
	TimingAny
};


struct TimingParameters
{
	int tRC = 0;
	int tRRD = 0;
	int tRP = 0;
	int tRFC = 0;
	int tCCD = 0;
	int tRTRS = 0;
	int tCWD = 0;
	int tWTR = 0;
	int tCAS = 0;
	int tRCD = 0;
	int tOST = 0;
	int tRAS = 0;
	int tWR = 0;
	int tRTP = 0;
	int tBURST = 0;
};


class Controller
{
	int id;

	std::string name;

	// Sizes of components under this controller
	int num_channels;
	int num_ranks;
	int num_banks;
	int num_rows;
	int num_columns;
	int num_bits;

	// Timing matrix
	int timings[4][4][3][3] = {};

	// List of physical channels contained in this controller
	std::vector<std::shared_ptr<Channel>> channels;

	// Incoming request queue
	std::queue<std::shared_ptr<Request>> incoming_requests;

public:

	Controller(int id);
	Controller(int id, const std::string &section,
			misc::IniFile &config);

	/// Parse a MemoryController section of a dram configuration file
	void ParseConfiguration(const std::string &section,
			misc::IniFile &config);
	void ParseTiming(const std::string &section,
			misc::IniFile &config);

	/// Set default timing parameters for DDR3 1066
	void DefaultDDR3_1066(TimingParameters &parameters);

	int getId() { return id; }

	int getNumChannels() { return num_channels; }
	int getNumRanks() { return num_ranks; }
	int getNumBanks() { return num_banks; }
	int getNumRows() { return num_rows; }
	int getNumColumns() { return num_columns; }

	Channel *getChannel(int id) { return channels[id].get(); }

	/// Add a request to the controller's incoming request queue.
	void AddRequest(std::shared_ptr<Request> request);

	/// Call the request processor for this controller.  This function will
	/// only invoke the request processor if it is not already scheduled to
	/// run.  The request processor will keep reinvoking itself while there
	/// are requests in the incoming request queue.
	void CallRequestProcessor();

	/// Event handler that runs the request processor.
	static void RequestProcessorHandler(esim::EventType *,
			esim::EventFrame *);

	/// Process requests in the incoming requests queue, breaking them
	/// down into their commands.
	void RequestProcessor();

	/// Dump the object to an output stream.
	void dump(std::ostream &os = std::cout) const;

	/// Dump object with the << operator
	friend std::ostream &operator<<(std::ostream &os,
			const Controller &object)
	{
		object.dump(os);
		return os;
	}
};


class RequestProcessorFrame : public esim::EventFrame
{

public:
	RequestProcessorFrame()
	{
	}

	Controller *controller;
};

}  // namespace dram

#endif