// $Id$
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
//    http://www.dre.vanderbilt.edu/CIAO

#ifndef CIAO_PUBLICATION_EXEC_H
#define CIAO_PUBLICATION_EXEC_H

#include /**/ "ace/pre.h"

#include "MessengerEC.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "Messenger_exec_export.h"
#include "tao/LocalObject.h"

// MY CODE
#include <string>
#include "ace/Thread_Mutex.h"

namespace CIAO_Messenger_Impl
{
  class MESSENGER_EXEC_Export Publication_exec_i
  : public virtual ::CCM_Publication,
    public virtual ::CORBA::LocalObject
  {
  public:
    Publication_exec_i (const char* text, CORBA::UShort period);
    virtual ~Publication_exec_i (void);

    // Operations from ::Publication

    virtual char* text ();

    virtual void text ( const char* text);

    virtual CORBA::UShort period ();

    virtual void period ( CORBA::UShort period);

  private:
    std::string text_;
    CORBA::UShort period_;

    TAO_SYNCH_MUTEX lock_;
  };
}

#include /**/ "ace/post.h"

#endif /* CIAO_PUBLICATION_EXEC_H */
