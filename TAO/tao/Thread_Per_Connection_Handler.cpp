//$Id$
#include "Thread_Per_Connection_Handler.h"
#include "Connection_Handler.h"
#include "debug.h"
#include "Transport.h"
#include "ORB_Core.h"
#include "ace/Flag_Manip.h"

ACE_RCSID (tao,
           Thread_Per_Connection_Handler,
           "$Id$")

TAO_Thread_Per_Connection_Handler::TAO_Thread_Per_Connection_Handler (
    TAO_Connection_Handler *ch,
    TAO_ORB_Core *oc)
  : TAO_TPC_BASE (oc->thr_mgr ())
  , ch_ (ch)
{
  this->ch_->transport ()->add_reference ();
}

TAO_Thread_Per_Connection_Handler::~TAO_Thread_Per_Connection_Handler (void)
{
  this->ch_->close_connection ();
  this->ch_->transport ()->remove_reference ();
}

int
TAO_Thread_Per_Connection_Handler::activate (long flags,
                                             int n_threads,
                                             int force_active,
                                             long priority,
                                             int grp_id,
                                             ACE_Task_Base *task,
                                             ACE_hthread_t thread_handles[],
                                             void *stack[],
                                             size_t stack_size[],
                                             ACE_thread_t  thread_names[])
{
  if (TAO_debug_level)
    {
      ACE_DEBUG  ((LM_DEBUG,
                   ACE_LIB_TEXT ("TAO (%P|%t) - IIOP_Connection_Handler::")
                   ACE_LIB_TEXT ("activate %d threads, flags = %d\n"),
                   n_threads,
                   flags));
    }

  return TAO_TPC_BASE::activate (flags,
                                 n_threads,
                                 force_active,
                                 priority,
                                 grp_id,
                                 task,
                                 thread_handles,
                                 stack,
                                 stack_size,
                                 thread_names);
}

int
TAO_Thread_Per_Connection_Handler::activate_threads (long flags,
                                                     int n_threads)
{
  int priority = ACE_DEFAULT_THREAD_PRIORITY;
  int sched_policy = ACE_SCHED_OTHER;

  // Get the system policy and priority
  if (ACE_Thread::getprio (ACE_Thread::self (),
                           priority,
                           sched_policy) == -1)
    {
      priority = ACE_DEFAULT_THREAD_PRIORITY;
    }

  if (sched_policy ==  ACE_SCHED_FIFO)
    ACE_SET_BITS (flags, THR_SCHED_FIFO);
  else if (sched_policy == ACE_SCHED_RR)
    ACE_SET_BITS (flags, THR_SCHED_FIFO);
  else
    ACE_SET_BITS (flags, THR_SCHED_DEFAULT);

  return this->activate (flags,
                         n_threads,
                         0,
                         priority);
}

int
TAO_Thread_Per_Connection_Handler::svc (void)
{
  ACE_Flag_Manip::clr_flags (
      this->ch_->transport ()->event_handler_i ()->get_handle (),
      ACE_NONBLOCK);

  // Call the implementation here
  return this->ch_->svc_i ();
}

int
TAO_Thread_Per_Connection_Handler::open (void*v)
{
  return this->ch_->open_handler (v);
}

int
TAO_Thread_Per_Connection_Handler::close (u_long)
{
  delete this;

  return 0;
}
