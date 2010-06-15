// -*- C++ -*-
//
// $Id$

#ifndef CIAO_USESSM_SENDER_EXEC_H_
#define CIAO_USESSM_SENDER_EXEC_H_

#include "UsesSM_SenderEC.h"
#include "UsesSM_Sender_exec_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "tao/LocalObject.h"
#include "ace/Task.h"

namespace CIAO_UsesSM_Sender_Impl
{
  typedef ACE_Atomic_Op <TAO_SYNCH_MUTEX, CORBA::UShort > Atomic_UShort;
   
  /// Worker threads for asynchronous invocations
  class asynch_foo_generator : public virtual ACE_Task_Base
  {
  public:
    asynch_foo_generator (::UsesSM::Sender::sendc_run_my_um_oneConnections_var,
                          ::UsesSM::AMI4CCM_Two_ptr my_two_ami);
    virtual int svc (void);

  private:
    ::UsesSM::Sender::sendc_run_my_um_oneConnections_var my_one_ami_;
    ::UsesSM::AMI4CCM_Two_var my_two_ami_;
  };

  /// Worker threads for synchronous invocations
  class synch_foo_generator : public virtual ACE_Task_Base
  {
  public:
    synch_foo_generator (::UsesSM::Sender::run_my_um_oneConnections_var);
    virtual int svc (void);

  private:
    ::UsesSM::Sender::run_my_um_oneConnections_var my_one_ami_;
  };

  class One_callback_exec_i
    : public virtual ::UsesSM::CCM_AMI4CCM_OneReplyHandler,
      public virtual ::CORBA::LocalObject
  {
  public:
    One_callback_exec_i (void);
    virtual ~One_callback_exec_i (void);

    virtual void foo (::CORBA::Long ami_return_val, const char * answer);
    virtual void foo_excep (::CCM_AMI::ExceptionHolder_ptr excep_holder);
  };
  class Two_callback_exec_i
    : public virtual ::UsesSM::CCM_AMI4CCM_TwoReplyHandler,
      public virtual ::CORBA::LocalObject
  {
  public:
    Two_callback_exec_i (void);
    virtual ~Two_callback_exec_i (void);

    virtual void bar (const char * answer);
    virtual void bar_excep (::CCM_AMI::ExceptionHolder_ptr excep_holder);
  };
  class Sender_exec_i
    : public virtual Sender_Exec,
      public virtual ::CORBA::LocalObject
  {
  public:
    Sender_exec_i (void);
    virtual ~Sender_exec_i (void);

    virtual void
    set_session_context (
      ::Components::SessionContext_ptr ctx);

    virtual void configuration_complete (void);

    virtual void ccm_activate (void);
    virtual void ccm_passivate (void);
    virtual void ccm_remove (void);

  private:
    ::UsesSM::CCM_Sender_Context_var context_;
    One_callback_exec_i* global_foo_callback_;
    Two_callback_exec_i* global_two_callback_;

   };

  extern "C" USESSM_SENDER_EXEC_Export ::Components::EnterpriseComponent_ptr
  create_UsesSM_AMI_Sender_Impl (void);
}

#endif /* ifndef */
