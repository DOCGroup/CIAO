// -*- C++ -*-
// $Id$

/**
 * Code generated by the The ACE ORB (TAO) IDL Compiler v1.8.3
 * TAO and the TAO IDL Compiler have been developed by:
 *       Center for Distributed Object Computing
 *       Washington University
 *       St. Louis, MO
 *       USA
 *       http://www.cs.wustl.edu/~schmidt/doc-center.html
 * and
 *       Distributed Object Computing Laboratory
 *       University of California at Irvine
 *       Irvine, CA
 *       USA
 * and
 *       Institute for Software Integrated Systems
 *       Vanderbilt University
 *       Nashville, TN
 *       USA
 *       http://www.isis.vanderbilt.edu/
 *
 * Information about TAO is available at:
 *     http://www.cs.wustl.edu/~schmidt/TAO.html
 **/

#include "CFTLE_Test_Receiver_exec.h"

#include "dds4ccm/impl/dds4ccm_conf.h"

#define MIN_ITERATION 2
#define MAX_ITERATION 5

#define SAMPLES_PER_KEY 2

namespace CIAO_CFTLE_Test_Receiver_Impl
{

  /**
   * Facet Executor Implementation Class: info_listen_data_listener_exec_i
   */

  info_listen_data_listener_exec_i::info_listen_data_listener_exec_i (
        ::CFTLE_Test::CCM_Receiver_Context_ptr ctx,
        ACE_Thread_ID &thread_id,
        Atomic_Long &samples_received)
    : ciao_context_ (
        ::CFTLE_Test::CCM_Receiver_Context::_duplicate (ctx))
      , thread_id_ (thread_id)
      , samples_received_ (samples_received)
  {
  }

  info_listen_data_listener_exec_i::~info_listen_data_listener_exec_i (void)
  {
  }

  // Operations from ::CFTLE_Test::ContentFilteredTopicListenEventTestConnector::Listener

  void
  info_listen_data_listener_exec_i::on_one_data (const ::ContentFilteredTopicListenEventTest & datum,
  const ::CCM_DDS::ReadInfo & info)
  {
    ACE_Thread_ID t_id;
    this->thread_id_ = t_id;
    ++this->samples_received_;
    ACE_DEBUG ((LM_DEBUG, "ContentFilteredTopicListenEventTest_Listener::on_one_data: "
                          "key <%C> - iteration <%d>\n",
                          datum.key.in (),
                          datum.iteration));
    if (!DDS_INSTANCE_HANDLE_VALID(info.instance_handle))
      {
        ACE_ERROR ((LM_ERROR, "ERROR: ContentFilteredTopicListenEventTest_Listener::on_one_data: "
                              "instance handle seems to be invalid "
                              "key <%C> - iteration <%d>\n",
                              datum.key.in (),
                              datum.iteration));
      }
    if (info.source_timestamp.sec == 0 &&
        info.source_timestamp.nanosec == 0)
      {
        ACE_ERROR ((LM_ERROR, "ERROR: ContentFilteredTopicListenEventTest_Listener::on_one_data: "
                              "source timestamp seems to be invalid (nil) "
                              "key <%C> - iteration <%d>\n",
                              datum.key.in (),
                              datum.iteration));
      }
    if (datum.iteration <= MIN_ITERATION ||
        datum.iteration  > MAX_ITERATION )
      {
        ACE_ERROR ((LM_ERROR, "ContentFilteredTopicListenEventTest_Listener::on_one_data: "
                              "ERROR: Unexpected iteration received.\n"));
      }
  }

  void
  info_listen_data_listener_exec_i::on_many_data (const ::ContentFilteredTopicListenEventTestSeq & /* data */,
  const ::CCM_DDS::ReadInfoSeq & /* infos */)
  {
    /* Your code here. */
  }

  /**
   * Facet Executor Implementation Class: info_listen_status_exec_i
   */

  info_listen_status_exec_i::info_listen_status_exec_i (
        ::CFTLE_Test::CCM_Receiver_Context_ptr ctx)
    : ciao_context_ (
        ::CFTLE_Test::CCM_Receiver_Context::_duplicate (ctx))
  {
  }

  info_listen_status_exec_i::~info_listen_status_exec_i (void)
  {
  }

  // Operations from ::CCM_DDS::PortStatusListener

  void
  info_listen_status_exec_i::on_requested_deadline_missed (::DDS::DataReader_ptr /* the_reader */,
  const ::DDS::RequestedDeadlineMissedStatus & /* status */)
  {
    /* Your code here. */
  }

  void
  info_listen_status_exec_i::on_sample_lost (::DDS::DataReader_ptr /* the_reader */,
  const ::DDS::SampleLostStatus & /* status */)
  {
    /* Your code here. */
  }

  /**
   * Component Executor Implementation Class: Receiver_exec_i
   */

  Receiver_exec_i::Receiver_exec_i (void)
    : keys_ (5)
      , iterations_ (10)
      , thread_id_listener_ (0, 0)
      , samples_expected_ (keys_ * SAMPLES_PER_KEY)
      , samples_received_ (0)
  {
  }

  Receiver_exec_i::~Receiver_exec_i (void)
  {
  }

  // Supported operations and attributes.
  void
  Receiver_exec_i::start (void)
  {
    ::CCM_DDS::DataListenerControl_var dlc =
        this->ciao_context_->get_connection_info_listen_data_control ();
    dlc->mode (::CCM_DDS::ONE_BY_ONE);
  }

  // Component attributes and port operations.

  ::CFTLE_Test::ContentFilteredTopicListenEventTestConnector::CCM_Listener_ptr
  Receiver_exec_i::get_info_listen_data_listener (void)
  {
    if ( ::CORBA::is_nil (this->ciao_info_listen_data_listener_.in ()))
      {
        info_listen_data_listener_exec_i *tmp = 0;
        ACE_NEW_RETURN (
          tmp,
          info_listen_data_listener_exec_i (
            this->ciao_context_.in (),
            this->thread_id_listener_,
            this->samples_received_),
            ::CFTLE_Test::ContentFilteredTopicListenEventTestConnector::CCM_Listener::_nil ());

          this->ciao_info_listen_data_listener_ = tmp;
      }

    return
      ::CFTLE_Test::ContentFilteredTopicListenEventTestConnector::CCM_Listener::_duplicate (
        this->ciao_info_listen_data_listener_.in ());
  }

  ::CCM_DDS::CCM_PortStatusListener_ptr
  Receiver_exec_i::get_info_listen_status (void)
  {
    if ( ::CORBA::is_nil (this->ciao_info_listen_status_.in ()))
      {
        info_listen_status_exec_i *tmp = 0;
        ACE_NEW_RETURN (
          tmp,
          info_listen_status_exec_i (
            this->ciao_context_.in ()),
            ::CCM_DDS::CCM_PortStatusListener::_nil ());

          this->ciao_info_listen_status_ = tmp;
      }

    return
      ::CCM_DDS::CCM_PortStatusListener::_duplicate (
        this->ciao_info_listen_status_.in ());
  }

  ::CORBA::UShort
  Receiver_exec_i::keys (void)
  {
    return this->keys_;
  }

  void
  Receiver_exec_i::keys (
    const ::CORBA::UShort keys)
  {
    this->keys_ = keys;

    this->samples_expected_ = SAMPLES_PER_KEY * this->keys_;
  }

  ::CORBA::UShort
  Receiver_exec_i::iterations (void)
  {
    return this->iterations_;
  }

  void
  Receiver_exec_i::iterations (
    const ::CORBA::UShort iterations)
  {
    this->iterations_ = iterations;
  }

  // Operations from Components::SessionComponent.

  void
  Receiver_exec_i::set_session_context (
    ::Components::SessionContext_ptr ctx)
  {
    this->ciao_context_ =
      ::CFTLE_Test::CCM_Receiver_Context::_narrow (ctx);

    if ( ::CORBA::is_nil (this->ciao_context_.in ()))
      {
        throw ::CORBA::INTERNAL ();
      }
  }

  void
  Receiver_exec_i::configuration_complete (void)
  {
    ::CFTLE_Test::ContentFilteredTopicListenEventTestConnector::Reader_var reader =
      this->ciao_context_->get_connection_info_listen_data ();
    if (CORBA::is_nil (reader.in ()))
      {
        ACE_ERROR ((LM_ERROR, "Receiver_exec_i::configuration_complete - "
                              "Error get reader connection.\n"));
      }
  }

  void
  Receiver_exec_i::ccm_activate (void)
  {
    ACE_DEBUG ((LM_DEBUG, "Receiver_exec_i::ccm_activate - "
                          "Start listening.\n"));
    start ();
  }

  void
  Receiver_exec_i::ccm_passivate (void)
  {
    /* Your code here. */
  }

  void
  Receiver_exec_i::ccm_remove (void)
  {
    char ccm_buf [65];
    ACE_Thread_ID ccm_thread_id;
    ccm_thread_id.to_string (ccm_buf);

    char list_buf [65];
    this->thread_id_listener_.to_string (list_buf);


    if (this->thread_id_listener_.id () == 0)
      {
        ACE_ERROR ((LM_ERROR, "ERROR: "
                              "Thread ID for ReaderListener not set!\n"));
      }
    #if (CIAO_DDS4CCM_CONTEXT_SWITCH == 1)
    else if (this->thread_id_listener_ == ccm_thread_id)
      {
        ACE_DEBUG ((LM_DEBUG, "CFT_EVENT: "
                              "Thread switch for ReaderListener seems OK. "
                              "(DDS uses the CCM thread for its callback) "
                              "listener <%C> - component <%C>\n",
                              list_buf,
                              ccm_buf));
      }
    else
      {
        ACE_ERROR ((LM_ERROR, "ERROR: CFT_EVENT: "
                              "Thread switch for ReaderListener "
                              "doesn't seem to work! "
                              "listener <%C> - component <%C>\n",
                              list_buf,
                              ccm_buf));
      }
    #else
    else if (this->thread_id_listener_ == ccm_thread_id)
      {
        ACE_ERROR ((LM_ERROR, "ERROR: CFT_EVENT: ReaderListener: "
                              "DDS seems to use a CCM thread for its callback: "
                              "listener <%C> - component <%C>\n",
                              list_buf,
                              ccm_buf));
      }
    else
      {
        ACE_DEBUG ((LM_DEBUG, "CFT_EVENT: ReaderListener: "
                              "DDS seems to use its own thread for its callback: "
                              "listener <%C> - component <%C>\n",
                              list_buf,
                              ccm_buf));
      }
    #endif
    if (this->samples_received_ != this->samples_expected_)
      {
        ACE_ERROR ((LM_ERROR, "ERROR: CFT_EVENT: ReaderListener: "
                              "Unexpected number of samples received: "
                              "expected <%d> - received <%d>\n",
                              this->samples_expected_,
                              this->samples_received_.value ()));
      }
    else
      {
        ACE_DEBUG ((LM_DEBUG, "CFT_EVENT: ReaderListener: "
                              "Expected number of samples received: "
                              "expected <%d> - received <%d>\n",
                              this->samples_expected_,
                              this->samples_received_.value ()));
      }
  }

  extern "C" RECEIVER_EXEC_Export ::Components::EnterpriseComponent_ptr
  create_CFTLE_Test_Receiver_Impl (void)
  {
    ::Components::EnterpriseComponent_ptr retval =
      ::Components::EnterpriseComponent::_nil ();

    ACE_NEW_NORETURN (
      retval,
      Receiver_exec_i);

    return retval;
  }
}
