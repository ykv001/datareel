// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// C++ Compiler Used: GNU, Intel
// Produced By: DataReel Software Development Team
// File Creation Date: 06/17/2016
// Date Last Modified: 07/16/2016 
// Copyright (c) 2016 DataReel Software Development
// ----------------------------------------------------------- // 
// ------------- Program Description and Details ------------- // 
// ----------------------------------------------------------- // 
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
 
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  
USA

Help functions for Datareel load balancer.

*/
// ----------------------------------------------------------- // 
#include "gxdlcode.h"

#include "drlb_server.h"
#include "m_help.h"

// Version number, program  and publisher info
const char *VersionString = "1.37";
const char *ProgramDescription = "Datareel Load Balancer";
const char *ProducedBy = "http://datareel.com";

const char *GetVersionString()
{
  return VersionString;
}

const char *GetProgramDescription()
{
  return ProgramDescription;
}

const char *GetProducedBy()
{
  return ProducedBy;
}

void HelpMessage(const char *ProgramName)
{
  VersionMessage();
  cout << "Usage: " << ProgramName << " [args]" << "\n" << flush;
  cout << "args: " << "\n" << flush;
  cout << "     --help                      Print help message and exit" << "\n" << flush;
  cout << "     --version                   Print version number and exit" << "\n" << flush;
  cout << "     --debug                     Enable debug mode default level --debug=1" << "\n" << flush;
  cout << "     --debug-level=1             Set debugging level, 1-5" << "\n" << flush;
  cout << "     --verbose                   Turn on verbose messaging to console" << "\n" << flush;
  cout << "     --verbose-level=1           Set verbose level, 0-5" << "\n" << flush;
  cout << "     --config-file=\"config.cfg\"  Specify configuration CFG file location" << "\n" << flush;
  cout << "     --log-file=\"logfile.log\"    Use log file to log program output" << "\n" << flush;
  cout << "     --log-level=1               Set logging level, 0-5" << "\n" << flush;
  cout << "     --log-file-clear            Clear the log file when program starts" << "\n" << flush;
  cout << "\n" << flush;
}

void VersionMessage()
{
  cout << "\n" << flush;
  cout << ProgramDescription << " version " << VersionString  << "\n" 
	      << flush;
  cout << "Produced by: " << ProducedBy << "\n" << flush;
  cout << "\n" << flush;
}

// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //