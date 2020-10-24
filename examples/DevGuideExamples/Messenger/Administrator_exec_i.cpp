//
// ****              Code generated by the                 ****
// ****  Component Integrated ACE ORB (CIAO) CIDL Compiler ****
// CIAO has been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// CIDL Compiler has been developed by:
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about CIAO is available at:
//    https://www.dre.vanderbilt.edu/CIAO

#include "Administrator_exec_i.h"

// MY CODE
#include <iostream>
#include <string>

namespace CIAO_Administrator_Impl
{
  //==================================================================
  // Component Executor Implementation Class:   Administrator_exec_i
  //==================================================================

  Administrator_exec_i::Administrator_exec_i (void)
  {
  }

  Administrator_exec_i::~Administrator_exec_i (void)
  {
  }

  // Supported or inherited operations.

  // Attribute operations.

  // Port operations.

  // Operations from Components::SessionComponent

  void
  Administrator_exec_i::set_session_context (
    ::Components::SessionContext_ptr ctx)
  {
    this->context_ =
      ::CCM_Administrator_Context::_narrow (ctx);

    if (CORBA::is_nil (this->context_.in ()))
    {
      throw ::CORBA::INTERNAL ();
    }
  }

  void
  Administrator_exec_i::ccm_activate ()
  {
    // Your code here.

    // MY CODE
    ACE_DEBUG((LM_INFO, ACE_TEXT("ccm_activate\n" )));

    // Activate the Task
    this->activate();
  }

  void
  Administrator_exec_i::ccm_passivate ()
  {
    // Your code here.

    // MY CODE
    ACE_DEBUG((LM_INFO, ACE_TEXT("ccm_passivate\n" )));
  }

  void
  Administrator_exec_i::ccm_remove ()
  {
    // Your code here.
    ACE_DEBUG((LM_INFO, ACE_TEXT("ccm_remove\n")));
  }

  void
  Administrator_exec_i::configuration_complete (void)
  {
    ACE_DEBUG ((LM_INFO, ACE_TEXT("configuration_complete\n")));
  }

  // Overridden from ACE_Task_Base
  int
  Administrator_exec_i::svc()
  {
    enum SelectionType { START=1, STOP, CHANGE_PERIOD, CHANGE_TEXT,EXIT };

    bool done = false;
    do {
      std::cout << "\nWhat do you want to do to the Messenger(s)?" << std::endl;
      std::cout << START << ".  Start" << std::endl;
      std::cout << STOP << ".  Stop" << std::endl;
      std::cout << CHANGE_PERIOD << ".  Change Publication Period" << std::endl;
      std::cout << CHANGE_TEXT << ".  Change Publication Text" << std::endl;
      std::cout << EXIT << ".  Exit" << std::endl;

      char selection_text[10];
      std::cout << "Please enter a selection: ";
      std::cin.getline( selection_text, sizeof(selection_text) );
      int selection = ACE_OS::atoi(selection_text);

      switch (selection) {
        case START:
          startPublishing();
          break;
        case STOP:
          stopPublishing();
          break;
        case CHANGE_PERIOD:
          changePublicationPeriod();
          break;
        case CHANGE_TEXT:
          changePublicationText();
          break;
        case EXIT: {
          done = true;
          break;
        }
        default:
          std::cout << "Please enter a valid option" << std::endl;
      }
    } while ( !done );

    return 0;
  }

  void Administrator_exec_i::startPublishing()
  {
    // Get the attached Runnable facet(s)
    ::Administrator::runnablesConnections_var connections =
      this->context_->get_connections_runnables();

    std::cout << "Starting Publication" << std::endl;
    for ( CORBA::ULong i = 0; i < connections->length(); ++i ) {
      Runnable_var runnable = (*connections)[i].objref;
      runnable->start();
    }
  }

  void Administrator_exec_i::stopPublishing()
  {
    // Get the attached Runnable facet(s)
    ::Administrator::runnablesConnections_var connections =
      this->context_->get_connections_runnables();

    std::cout << "Stopping Publication" << std::endl;
    for ( CORBA::ULong i = 0; i < connections->length(); ++i ) {
      Runnable_var runnable = (*connections)[i].objref;
      runnable->stop();
    }
  }

  void Administrator_exec_i::changePublicationPeriod()
  {
    // Get the attached Publication facet(s)
    ::Administrator::contentConnections_var contents =
      this->context_->get_connections_content();

    char period[10];
    std::cout << "Please enter a new period in seconds: ";
    std::cin.getline( period, sizeof( period ) );
    for ( CORBA::ULong i = 0; i < contents->length(); ++i ) {
      Publication_var publication = (*contents)[i].objref;
      publication->period( ACE_OS::atoi(period) );
    }
  }

  void Administrator_exec_i::changePublicationText()
  {
    // Get the attached Publication facet(s)
    ::Administrator::contentConnections_var contents =
      this->context_->get_connections_content();

    char buffer[1024];
    std::cout << "Please enter new text: ";
    std::cin.getline( buffer, sizeof(buffer) );
    for ( CORBA::ULong i = 0; i < contents->length(); ++i ) {
      Publication_var publication = (*contents)[i].objref;
      publication->text( buffer );
    }
  }

  extern "C" ADMINISTRATOR_EXEC_Export ::Components::EnterpriseComponent_ptr
  create_Administrator_Impl (void)
  {
    ::Components::EnterpriseComponent_ptr retval =
      ::Components::EnterpriseComponent::_nil ();

    ACE_NEW_RETURN (retval,
                    Administrator_exec_i,
                    ::Components::EnterpriseComponent::_nil ());

    return retval;
  }

  //==================================================================
  // Home Executor Implementation Class:   AdministratorHome_exec_i
  //==================================================================

  AdministratorHome_exec_i::AdministratorHome_exec_i (void)
  {
  }

  AdministratorHome_exec_i::~AdministratorHome_exec_i (void)
  {
  }

  // Supported or inherited operations.

  // Home operations.

  // Factory and finder operations.

  // Attribute operations.

  // Implicit operations.

  ::Components::EnterpriseComponent_ptr
  AdministratorHome_exec_i::create ()
  {
    ::Components::EnterpriseComponent_ptr retval =
      ::Components::EnterpriseComponent::_nil ();

    ACE_NEW_THROW_EX (
      retval,
      Administrator_exec_i,
      ::CORBA::NO_MEMORY ());

    return retval;
  }

  extern "C" ADMINISTRATOR_EXEC_Export ::Components::HomeExecutorBase_ptr
  create_AdministratorHome_Impl (void)
  {
    ::Components::HomeExecutorBase_ptr retval =
      ::Components::HomeExecutorBase::_nil ();

    ACE_NEW_RETURN (
      retval,
      AdministratorHome_exec_i,
      ::Components::HomeExecutorBase::_nil ());

    return retval;
  }
}
