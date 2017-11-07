/**
 * @project identt
 * @file src/hrpc/RemoteKeeper.cc
 * @author  S Roychowdhury <sroycode AT gmail DOT com>
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2017 S Roychowdhury.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 *  RemoteKeeper.cc  :  Remotes list handler implementation
 *
 */

#include <hrpc/RemoteKeeper.hpp>
#include <thread>

/**
* constructor
*
*/
identt::hrpc::RemoteKeeper::RemoteKeeper(identt::utils::SharedTable::pointer s) : stptr(s) {}

/**
* destructor
*/
identt::hrpc::RemoteKeeper::~RemoteKeeper() {}

/**
* SetHosts : set list of hosts
*
*/
void identt::hrpc::RemoteKeeper::SetHosts(identt::hrpc::StateT* state)
{
	if (state->is_master()) {
		identt::utils::SharedTable::RemoteMapT rmap;
		for (auto i=0; i<state->remotes_size(); ++i) {
			identt::hrpc::RemoteT* rms = state->mutable_remotes(i);
			rmap[rms->address()]= {rms->logcounter(), rms->updated_at()};
		}
		stptr->remotes.AddMany(std::move(rmap));
	} else {
		stptr->remotes.AddOne( state->thisurl() , { state->logcounter(), state->ts() } );
	}
	LOG(INFO) << "Added Remote from: " << state->thisurl() ;
}

/**
* AddHost : add a single host
*
*/
void identt::hrpc::RemoteKeeper::AddHost(identt::hrpc::RemoteT* rms)
{
	stptr->remotes.AddOne( rms->address() , { rms->logcounter(), rms->updated_at() } );
}

/**
* AddHost : add a single host
*
*/
void identt::hrpc::RemoteKeeper::AddHost(std::string name, uint64_t logcounter, uint64_t updated_at)
{
	stptr->remotes.AddOne( std::move(name), {logcounter, updated_at});
}

/**
* GetHosts : get params and list of hosts
*
*/
void identt::hrpc::RemoteKeeper::GetHosts(identt::hrpc::StateT* state)
{
	// get params
	state->set_hostname( stptr->hostname.Get() );
	state->set_master( stptr->master.Get() );
	state->set_logcounter( stptr->logcounter.Get() );
	state->set_is_master( stptr->is_master.Get() );
	state->set_ts( IDENTT_CURRTIME_MS );
	state->set_thisurl( stptr->thisurl.Get() );
	if ( !state->is_master() ) return;

	for (const auto& p : stptr->remotes.GetCopy() ) {
		identt::hrpc::RemoteT* rem = state->add_remotes();
		rem->set_address( p.first );
		rem->set_logcounter( p.second.first );
		rem->set_updated_at( p.second.second );
	}
}

/**
* HostUpdate : update hosts with latest
*
*/
void identt::hrpc::RemoteKeeper::HostUpdate()
{
	// get params
	identt::hrpc::StateT state;
	GetHosts(&state);
	identt::hrpc::HrpcClient hclient;
	for (const auto& p : stptr->remotes.GetCopy() ) {
		LOG(INFO) << "Intimate Remote: " << p.first ;
		bool status = hclient.SendToRemote(stptr,p.first,::identt::hrpc::R_SETINFO,&state,true);
	}
}

/**
* MasterUpdate: elect the master based on info received after cutoff
*
*/
void identt::hrpc::RemoteKeeper::MasterUpdate(uint64_t cutoff)
{
	uint64_t max_logcounter = stptr->logcounter.Get();
	std::string newmaster = stptr->thisurl.Get();
	DLOG(INFO) << "Cand 0: " << newmaster << " " << max_logcounter;
	for (const auto& p : stptr->remotes.GetCopy() ) {
		DLOG(INFO) << "Cand 1: " << p.first << " " << p.second.first;
		// eliminate too low , lower counter , me
		if ( (p.second.second < cutoff) || (p.second.first < max_logcounter) || (p.first == newmaster) )
			continue;
		// we are now left with eligible entries only
		if ( (p.second.first > max_logcounter)  || ((p.second.first == max_logcounter) && (p.first > newmaster)) ) {
			newmaster = p.first;
			max_logcounter = p.second.first;
		}
		DLOG(INFO) << "Cand 2: " << newmaster << " " << max_logcounter;
	}
	
	if (newmaster == stptr->thisurl.Get()) {
		// lastslave initialised
		stptr->lastslave.Set("");
		stptr->is_master.Set(true);
		LOG(INFO) << "New Master elected: ME !";
	} else {
		identt::hrpc::StateT state;
		GetHosts(&state);
		identt::hrpc::HrpcClient hclient;
		int retries = 0;
		do {
			bool status = hclient.SendToRemote(stptr,newmaster,::identt::hrpc::R_REGISTER,&state,true);
			if (status) {
				stptr->master.Set(newmaster);
				LOG(INFO) << "New Master elected: " << newmaster;
				break;
			} else {
				LOG(INFO) << "New Master is not ready: " << newmaster;
				std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
			}
		} while(++retries<10);
	}
}
