/*
 *  Multi2Sim
 *  Copyright (C) 2014  Sida Gu (gu.sid@husky.neu.edu)
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

#include <iostream>
#include <memory>
#include <vector>

#include <arch/mips/asm/Inst.h>
#include <lib/cpp/ELFReader.h>
#include <mem-system/Memory.h>
#include <mem-system/SpecMem.h>

#include "FileTable.h"
#include "Regs.h"
#include "Signal.h"


namespace MIPS
{
class Emu;

/// Context states
enum ContextState
{
	ContextInvalid      = 0x00000,
	ContextRunning      = 0x00001,  // it is able to run instructions
	ContextSpecMode     = 0x00002,  // executing in speculative mode
};

/// MIPS Context
class Context
{
	// Emulator it belongs to
	Emu *emu;

	// Virtual memory address space index
	int address_space_index;

	// Context memory. This object can be shared by multiple contexts, so it
	// is declared as a shared pointer. The last freed context pointing to
	// this memory object will be the one automatically freeing it.
	std::shared_ptr<mem::Memory> memory;

	// Speculative memory. Its initialization is deferred to be able to link
	// it with the actual memory, known only at context creation.
	std::unique_ptr<mem::SpecMem> spec_mem;

	// Register file. Each context has its own copy always.
	Regs regs;

	// Last emulated instruction
	//Inst inst;

	// File descriptor table, shared by contexts
	std::shared_ptr<FileTable> file_table;

	///////////////////////////////////////////////////////////////////////
	//
	// Functions implemented in ContextLoader.cc. These are the functions
	// related with the program loading process.
	//
	///////////////////////////////////////////////////////////////////////

	/// Structure containing information initialized by the program loader,
	/// associated with a context. When a context is created from a program
	/// executable, a Loader object is associated to it. All child contexts
	/// spawned by it will share the same Loader object.
	struct Loader
	{
		// Program executable
		std::unique_ptr<ELFReader::File> binary;

		// Command-line arguments
		std::vector<std::string> args;

		// Environment variables
		std::vector<std::string> env;

		// Executable interpreter
		std::string interp;

		// Executable file name
		std::string exe;

		// Current working directory
		std::string cwd;

		// File name for standard input and output
		std::string stdin_file_name;
		std::string stdout_file_name;

		// Stack
		unsigned stack_base;
		unsigned stack_top;
		unsigned stack_size;
		unsigned environ_base;

		// Lowest address initialized
		unsigned bottom;

		// Program entries
		unsigned prog_entry;
		unsigned interp_prog_entry;

		// Program headers
		unsigned phdt_base;
		unsigned phdr_count;

		// Random bytes
		unsigned at_random_addr;
		unsigned at_random_addr_holder;

	};

	// Loader information. This information can be shared among multiple
	// contexts. For this reason, it is declared as a shared pointer. The
	// last destructed context sharing this variable will automatically free
	// it.
	std::shared_ptr<Loader> loader;

	// Load ELF sections from binary
	void LoadELFSections(ELFReader::File *binary);

	// Load ELF binary, as already decoded in 'loader.binary'
	void LoadBinary();

	///////////////////////////////////////////////////////////////////////
	//
	// Fields and functions related with signal handling. The functions are
	// implemented in ContextSignal.cc.
	//
	///////////////////////////////////////////////////////////////////////

	// Table of signal handlers, possibly shared by multiple contexts
	std::shared_ptr<SignalHandlerTable> signal_handler_table;

public:
	Context();

	/// Destructor
	~Context();

	void Load(const std::vector<std::string> &args,
			const std::vector<std::string> &env,
			const std::string &cwd,
			const std::string &stdin_file_name,
			const std::string &stdout_file_name);

	/// Run one instruction for the context at the position pointed to by
	/// register program counter.
	void Execute();

	/// Given a file name, return its full path based on the current working
	/// directory for the context.
	std::string getFullPath(const std::string &path);
};
}