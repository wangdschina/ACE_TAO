// This may look like C, but it's really -*- C++ -*-

//=============================================================================
/**
 *  @file Thread_Per_Connection_Handler.h
 *
 *  $Id$
 *
 *  Definition of a connection handler for the thread-per-connection
 *  strategy.
 *
 *  @author  Balachandran Natarajan <bala@dre.vanderbilt.edu>
 */
//=============================================================================

#ifndef TAO_THREAD_PER_CONNECTION_HANDLER_H
#define TAO_THREAD_PER_CONNECTION_HANDLER_H

#include /**/ "ace/pre.h"
#include "ace/Task_T.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "tao/TAO_Export.h"

class TAO_Connection_Handler;
class TAO_ORB_Core;

typedef ACE_Task<ACE_NULL_SYNCH> TAO_TPC_BASE;

/**
 * @class TAO_Thread_Per_Connection_Handler
 *
 * @brief Definition for the thread-per-connection strategy.
 *
 * This object acts as an active object, encapsulating the protocol
 * specific handler which the active thread uses to process incoming
 * messages.
 *
 */
class TAO_Export TAO_Thread_Per_Connection_Handler : public TAO_TPC_BASE
{
public:
  TAO_Thread_Per_Connection_Handler (TAO_Connection_Handler *ch,
                                     TAO_ORB_Core *oc);

  ~TAO_Thread_Per_Connection_Handler (void);

  /// = Active object activation method.
  /**
   * @todo: This probably needs to go after x.4.1
   */
  virtual int activate (long flags = THR_NEW_LWP,
                        int n_threads = 1,
                        int force_active = 0,
                        long priority = ACE_DEFAULT_THREAD_PRIORITY,
                        int grp_id = -1,
                        ACE_Task_Base *task = 0,
                        ACE_hthread_t thread_handles[] = 0,
                        void *stack[] = 0,
                        size_t stack_size[] = 0,
                        ACE_thread_t  thread_names[] = 0);

  /// Template hook method that the thread uses...
  /**
   * Please see the documentation in ace/Task.h for details.
   */
  virtual int svc (void);
  virtual int open (void *);
  virtual int close (u_long);

  /// Helper method invoked by TAO's acceptors.
  /**
   * This method checks the priority of the invoking thread and sets
   * the priority of thread that will be created by activate ().
   */
  int activate_threads (long flags,
                        int n_threads);

private:
  /// Pointer to protocsol specific code that does the bunch of the
  /// job.
  TAO_Connection_Handler *ch_;
};

#include /**/ "ace/post.h"

#endif /*TAO_THREAD_PER_CONNECTION_HANDLER_H*/
