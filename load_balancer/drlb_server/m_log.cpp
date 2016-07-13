// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// C++ Compiler Used: GNU, Intel
// Produced By: DataReel Software Development Team
// File Creation Date: 06/17/2016
// Date Last Modified: 07/01/2016 
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

Log functions for Datareel load balancer.

*/
// ----------------------------------------------------------- // 
#include "gxdlcode.h"

#include "drlb_server.h"
#include "m_log.h"

int rotate_log_file();
void log_message(gxString &message, int is_proc_message, int is_debug);

int LogThread::flush_all_logs()
{
  int is_dirty = 0;
  size_t i;
  char sbuf[255];
  memset(sbuf, 0, 255);

  for(i = 0; i < servercfg->log_queue_size; i++) {
    if(servercfg->loq_queue_nodes[i].dirty[0] == 1) {
      servercfg->loq_queue_nodes[i].dirty[0] = 0;
      memmove(sbuf, servercfg->loq_queue_nodes[i].sbuf, 255);
      is_dirty++;
      write_log_entry(sbuf);
    }
  }
  for(i = 0; i < servercfg->log_queue_size; i++) {
    if(servercfg->loq_queue_proc_nodes[i].dirty[0] == 1) {
      servercfg->loq_queue_proc_nodes[i].dirty[0] = 0;
      memmove(sbuf, servercfg->loq_queue_proc_nodes[i].sbuf, 255);
      is_dirty++;
      write_log_entry(sbuf);
    }
  }
  for(i = 0; i < servercfg->log_queue_size; i++) {
    if(servercfg->loq_queue_debug_nodes[i].dirty[0] == 1) {
      servercfg->loq_queue_debug_nodes[i].dirty[0] = 0;
      memmove(sbuf, servercfg->loq_queue_debug_nodes[i].sbuf, 255);
      is_dirty++;
      write_log_entry(sbuf);
    }
  }
  return is_dirty;
}

void LogThread::ThreadExitRoutine(gxThread_t *thread)
{
  flush_all_logs();
}

void LogThread::ThreadCleanupHandler(gxThread_t *thread)
{
  flush_all_logs();
}

void *LogThread::ThreadEntryRoutine(gxThread_t *thread)
{
  int is_dirty = 0;

  while(servercfg->accept_clients) { 
    is_dirty = flush_all_logs();
    if(is_dirty == 0) sSleep(1); // All queue are empty
  }

  return (void *)0;
}

void LogThread::write_log_entry(char *sbuf)
{
  if(!sbuf) return;
  SysTime logtime;
  gxString output_message;

  output_message << clear << logtime.GetSyslogTime() << " " << servercfg->service_name << ": ";
  output_message << sbuf;
  output_message << "\n";
  
  if(servercfg->enable_logging == 1) {
    if(servercfg->logfile.Open(servercfg->logfile_name.c_str()) == 0) {
      servercfg->logfile << output_message.c_str();
      servercfg->logfile.df_Flush();
      servercfg->logfile.Close();
      rotate_log_file();  
  }
    else { // Could not write to log file
     if(servercfg->verbose) {
       cout << logtime.GetSyslogTime() << " " << servercfg->service_name.c_str() << ": " 
	    << "Error writing to log file " << servercfg->logfile_name.c_str()
	    << "\n";
       cout << output_message.c_str();
       cout.flush(); 
     }
    }
  }
  
  if(servercfg->verbose) {
    cout << output_message.c_str();
    cout.flush(); 
  }
}

void LogMessage(const char *mesg)
{
  gxString message = mesg;
  log_message(message, 0 , 0);
}

void LogDebugMessage(const char *mesg)
{
  gxString message = mesg;
  log_message(message, 0 , 1);
}

void LogProcMessage(const char *mesg)
{
  gxString message = mesg;
  log_message(message, 1 , 0);
}

void log_message(gxString &message, int is_proc_message, int is_debug)
{
  size_t slen = message.length();
  if(slen >= 255) slen = 254;

  if(is_proc_message == 1) {
    int queue_proc_count = servercfg->QUEUE_PROC_COUNT();
    memset(servercfg->loq_queue_proc_nodes[queue_proc_count].sbuf, 0, 255);
    memmove(servercfg->loq_queue_proc_nodes[queue_proc_count].sbuf, message.c_str(), slen);
    servercfg->loq_queue_proc_nodes[queue_proc_count].sbuf[254] = 0;
    servercfg->loq_queue_proc_nodes[queue_proc_count].dirty[0] = 1;
  }
  else if(is_debug == 1) {
    int queue_debug_count = servercfg->QUEUE_DEBUG_COUNT();
    memset(servercfg->loq_queue_debug_nodes[queue_debug_count].sbuf, 0, 255);
    memmove(servercfg->loq_queue_debug_nodes[queue_debug_count].sbuf, message.c_str(), slen);
    servercfg->loq_queue_debug_nodes[queue_debug_count].sbuf[254] = 0;
    servercfg->loq_queue_debug_nodes[queue_debug_count].dirty[0] = 1;
  }
  else {
    int queue_node_count = servercfg->QUEUE_NODE_COUNT();
    memset(servercfg->loq_queue_nodes[queue_node_count].sbuf, 0, 255);
    memmove(servercfg->loq_queue_nodes[queue_node_count].sbuf, message.c_str(), slen);
    servercfg->loq_queue_nodes[queue_node_count].sbuf[254] = 0;
    servercfg->loq_queue_nodes[queue_node_count].dirty[0] = 1;
  }
}

// Internel helper function to rotate an open log file
int rotate_log_file()
{
  if(!servercfg->enable_logging) return 0;

  gxString new_name = servercfg->logfile_name.c_str();
  FAU_t fsize = servercfg->logfile.df_FileSize(servercfg->logfile_name.c_str());
  SysTime logtime;

  if(!servercfg->logfile.Open(servercfg->logfile_name.c_str()) == 0) {
    if(servercfg->verbose) {
      cout << logtime.GetSyslogTime() << " " << servercfg->service_name.c_str() << ": " 
	   << "Error writing to log file " << servercfg->logfile_name.c_str()
	   << "\n";
      cout.flush(); 
    }
    return 0;
  }
  if(fsize >= (FAU_t)servercfg->max_log_size) {
    gxString output_message;
    output_message << clear << shn << logtime.GetSyslogTime() << " " << servercfg->service_name 
		<< ": Log file exceeded " << servercfg->max_log_size << " bytes" << "\n";
    servercfg->logfile << output_message.c_str();
    servercfg->logfile.df_Flush();
    if(servercfg->verbose) {
      cout << output_message.c_str();
      cout.flush(); 
    }
    int next_log = 1;
    if(servercfg->last_log == 0) {
      servercfg->last_log = 1;
    }
    else {
      servercfg->last_log++;
    }
    if(servercfg->last_log > servercfg->num_logs_to_keep) {
      servercfg->last_log = 0;  
      next_log = 1;
    }
    else {
      next_log = servercfg->last_log;
    }
    new_name << "." << next_log;
    output_message << clear << logtime.GetSyslogTime() << " " << servercfg->service_name 
		<< ": Rotating log, saving prev log file " << new_name.c_str() << "\n";
    servercfg->logfile << output_message.c_str();
    servercfg->logfile.df_Flush();
    servercfg->logfile.Close();
    if(servercfg->logfile.df_rename(servercfg->logfile_name.c_str(), new_name.c_str()) != DiskFileB::df_NO_ERROR) {
      
      servercfg->logfile.Open(servercfg->logfile_name.c_str());
			      
      output_message << clear << logtime.GetSyslogTime() << " " << servercfg->service_name 
		     << ": ERROR - could not rotate log, cannot create new log file " << new_name.c_str() << "\n";
      servercfg->logfile << output_message.c_str();
      servercfg->logfile.df_Flush();
      servercfg->logfile.Close();

      if(servercfg->verbose) {
	cout << output_message.c_str();
	cout.flush(); 
      }
      return 0;
    }
    
    if(servercfg->verbose) {
      cout << output_message.c_str();
      cout.flush(); 
    }
    if(servercfg->logfile.Open(servercfg->logfile_name.c_str()) == 0) {
      output_message << clear << logtime.GetSyslogTime() << " " << servercfg->service_name 
		  << ": New log file started " << "\n";
      servercfg->logfile << output_message.c_str();
      servercfg->logfile.df_Flush();
      servercfg->logfile.Close();
      if(servercfg->verbose) {
	cout << output_message.c_str();
	cout.flush(); 
      }
      else {
	if(servercfg->verbose) {
	  cout << logtime.GetSyslogTime() << " " << servercfg->service_name.c_str() << ": " 
	       << "Error writing to log file " << servercfg->logfile_name.c_str()
	       << "\n";
	  cout.flush(); 
	}
      }
    }
    return 1; // Log was rotated
  }

  return 0; // Did not rotate log
}

// Non-thread safe log rotate
int NT_log_rotate()
{
  return rotate_log_file();
}

int NT_print(const char *mesg1, const char *mesg2, const char *mesg3)
// Non-Thread safe print function used to print and log a console message
{
  return NT_printblock(mesg1, mesg2, mesg3, 0);
}

int NT_printlines(const char *mesg1, const char *mesg2, const char *mesg3)
// Non-Thread safe print function used to print and log a console message
{
  return NT_printblock(mesg1, mesg2, mesg3, 1);
}

int NT_printblock(const char *mesg1, const char *mesg2, const char *mesg3, 
		  int lines, int blob)
  // Non-Thread safe print function used to print and log a console message
{
  if(!mesg1) return 0;

  SysTime logtime;
  gxString output_message;
  if(lines) {
    if(mesg1) output_message << logtime.GetSyslogTime() 
			     << " " << servercfg->service_name << ": " << mesg1 << "\n";
    if(mesg2) output_message << logtime.GetSyslogTime() 
			     << " " << servercfg->service_name << ": " << mesg2 << "\n";
    if(mesg3) output_message << logtime.GetSyslogTime() 
			     << " " << servercfg->service_name << ": " << mesg3 << "\n";
  }
  else if(blob) {
    output_message << logtime.GetSyslogTime() 
		   << " " << servercfg->service_name << ": " << "Message Block <<" 
		   << "\n";
    if(mesg1) output_message << mesg1;
    if(mesg2) output_message << mesg2; 
    if(mesg3) output_message << mesg3; 
    output_message << ">>" << "\n";
  }
  else {
    output_message << logtime.GetSyslogTime() << " " << servercfg->service_name << ": ";
    if(mesg1) output_message << mesg1;
    if(mesg2) output_message << " " << mesg2;
    if(mesg3) output_message << " " << mesg3;
    output_message << "\n";
  }

  if(servercfg->enable_logging == 1) {
    if(servercfg->logfile.Open(servercfg->logfile_name.c_str()) == 0) {
      servercfg->logfile << output_message.c_str();
      servercfg->logfile.df_Flush();
      servercfg->logfile.Close();
    }
    else { // Could not write to log file
     if(servercfg->verbose) {
       cout << logtime.GetSyslogTime() << " " << servercfg->service_name.c_str() << ": " 
	    << "Error writing to log file " << servercfg->logfile_name.c_str()
	    << "\n";
       cout << output_message.c_str();
       cout.flush(); 
     }
    }
  }
  
  if(servercfg->verbose) {
    cout << output_message.c_str();
    cout.flush(); 
  }
  
  return 1;
}

int CheckThreadError(gxThread_t *thread, int seq_num)
{
  if(!thread) return 0;
  if(thread->GetThreadError() == gxTHREAD_NO_ERROR) return 0;
  gxString message;
  if(seq_num > 0) {
    message << clear << "[" << seq_num << "]: " << thread->ThreadExceptionMessage();
  }
  else {
    message << clear << thread->ThreadExceptionMessage();
  }
  if(servercfg->debug) {
    LogDebugMessage(message.c_str());
  }
  else {
    LogMessage(message.c_str());
  }
  return 1;
}

int CheckSocketError(gxSocket *s, int seq_num)
{
  if(!s) return 0;
  if(s->GetSocketError() == gxSOCKET_NO_ERROR) return 0;

  gxString message;
  if(seq_num > 0) {
    message << clear << "[" << seq_num << "]: " << s->SocketExceptionMessage();
  }
  else {
    message << clear << s->SocketExceptionMessage();
  }
  if(servercfg->debug) {
    LogDebugMessage(message.c_str());
  }
  else {
    LogMessage(message.c_str());
  }
  return 1;
}
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
