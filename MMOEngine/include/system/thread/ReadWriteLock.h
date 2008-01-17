/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef READWRITELOCK_H_
#define READWRITELOCK_H_

#include "../platform.h"

#include <pthread.h>

#include "../lang/StackTrace.h"

namespace sys {
  namespace thread {

	class Mutex;
	
	class ReadWriteLock {
	protected:
		pthread_rwlock_t rwlock;
	
		string lockName;
		int lockCount;
	
		StackTrace* trace;
		Time lockTime;
	
	public:
		ReadWriteLock() {
			pthread_rwlock_init(&rwlock, NULL);
	
			lockCount = 0;
			
			trace = NULL;
		}

		ReadWriteLock(const string& s) {
			pthread_rwlock_init(&rwlock, NULL);
	
			lockName = s;
			lockCount = 0;
			
			trace = NULL;
		}

		~ReadWriteLock() {
			pthread_rwlock_destroy(&rwlock);
		}
		
		void setLockName(const string& s) {
			lockName = s;
		}
	
		inline void rlock(bool doLock = true) {
			if (!doLock)
				return;
	
			#ifdef LOG_LOCKS
				int cnt = ++lockCount;
	
				cout << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquiring rlock #" << cnt << "\n";
			#endif
	
			#if !defined(TRACE_LOCKS) || defined(__CYGWIN__)	
				int res = pthread_rwlock_rdlock(&rwlock);
				if (res != 0)
					cout << "(" << Time::currentNanoTime() << " nsec) rlock() failed on RWLock \'" << lockName << "\' (" << res << ")\n";
			#else
				#ifndef LOG_LOCKS
					int cnt = ++lockCount;
				#endif
	
	
				Time start;
				start.addMiliTime(2000);
				
		    	while (pthread_rwlock_timedrdlock(&rwlock, start.getTimeSpec())) {
		    		cout << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "] unable to access rlock #" << cnt << " at\n";
					StackTrace::printStackTrace();
		    		
					cout << "locked at " << lockTime.getMiliTime() << " by\n";
					trace->print();
					
					while(true);
					
					start.addMiliTime(1000);
		    	}
	
				lockTime.update();
	
				if (trace != NULL)
					delete trace;
			
				trace = new StackTrace();
			#endif
			
			#ifdef LOG_LOCKS
				cout << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquired rlock #" << cnt << "\n";
			#endif
		}
	
		inline void wlock(bool doLock = true) {
			if (!doLock)
				return;
	
			#ifdef LOG_LOCKS
				int cnt = ++lockCount;
	
				cout << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquiring wlock #" << cnt << "\n";
			#endif
	
			#if !defined(TRACE_LOCKS) || defined(__CYGWIN__)	
				int res = pthread_rwlock_wrlock(&rwlock);
				if (res != 0)
					cout << "(" << Time::currentNanoTime() << " nsec) wlock() failed on RWLock \'" << lockName << "\' (" << res << ")\n";
			#else
				#ifndef LOG_LOCKS
					int cnt = ++lockCount;
				#endif
	
				Time start;
				start.addMiliTime(2000);
				
		    	while (pthread_rwlock_timedwrlock(&rwlock, start.getTimeSpec())) {
		    		cout << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "] unable to access wlock #" << cnt << " at\n";
		    		StackTrace::printStackTrace();
		    		
					cout << "locked at " << lockTime.getMiliTime() << " by\n";
					trace->print();
					
					while (true);
	
					start.addMiliTime(1000);
		    	}
	
				lockTime.update();
	
				if (trace != NULL)
					delete trace;
			
				trace = new StackTrace();
			#endif
			
			#ifdef LOG_LOCKS
				cout << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquired wlock #" << cnt << "\n";
			#endif
		}
	
		void wlock(Mutex* lock);
	
		void wlock(ReadWriteLock* lock);
	
		inline bool tryWLock() {
			return pthread_rwlock_trywrlock(&rwlock) == 0;
		}

		inline void unlock(bool doLock = true) {
			if (!doLock)
				return;
			
			#ifdef TRACE_LOCKS
				delete trace;
				trace = NULL;
			#endif
			
			#ifdef LOG_LOCKS
				int cnt = lockCount;
				cout << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] releasing lock #" << cnt << "\n";
			#endif
	
			int res = pthread_rwlock_unlock(&rwlock);
			if (res != 0) {
				cout << "(" << Time::currentNanoTime() << " nsec) unlock() failed on RWLock \'" << lockName << "\' (" << res << ")\n";
				
				StackTrace::printStackTrace();
			}
	
			#ifdef LOG_LOCKS
				cout << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] released lock #" << cnt << "\n";
			#endif
		}
	
		inline bool destroy() {
			pthread_rwlock_wrlock(&rwlock);
			pthread_rwlock_unlock(&rwlock);
			
			return pthread_rwlock_destroy(&rwlock) == 0;
		}
		
		friend class Condition;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*READWRITELOCK_H_*/