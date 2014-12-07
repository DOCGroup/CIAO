// -*- C++ -*-
// $Id$

/**
 * Code generated by the The ACE ORB (TAO) IDL Compiler v1.8.2
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
 *     http://www.dre.vanderbilt.edu/~schmidt/TAO.html
 **/
#ifndef CIAO_MULTITOPIC_SENDER_EXEC_DNHPAQ_H_
#define CIAO_MULTITOPIC_SENDER_EXEC_DNHPAQ_H_


#include "MultiTopic_SenderEC.h"
#include "Sender_exec_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "tao/LocalObject.h"

namespace CIAO_Shapes_Sender_Impl
{
  class Sender_exec_i;
  //============================================================
  // pulse_Generator
  //============================================================
  class pulse_Generator : public ACE_Event_Handler
  {
  public:
    pulse_Generator (Sender_exec_i &callback);

    virtual ~pulse_Generator ();

    virtual int handle_timeout (const ACE_Time_Value &tv,
                                   const void *arg);
  private:
    Sender_exec_i &pulse_callback_;
  };

  /**
   * Component Executor Implementation Class: Sender_exec_i
   */

  class  SENDER_EXEC_Export Sender_exec_i
    : public virtual Sender_Exec,
      public virtual ::CORBA::LocalObject
  {
  public:
    Sender_exec_i (void);
    virtual ~Sender_exec_i (void);

    //@{
    /** Supported operations and attributes. */
    enum TYPE_OF_SHAPE {
      SHAPE_TYPE_SQUARE,
      SHAPE_TYPE_TRIANGLE,
      SHAPE_TYPE_CIRCLE
    };

    //@}

    //@{
    /** Component attributes and port operations. */

    virtual ::CORBA::ULong rate (void);

    virtual void rate (::CORBA::ULong rate);

    virtual ::CORBA::UShort max_x (void);

    virtual void max_x (::CORBA::UShort max_x);

    virtual ::CORBA::UShort max_y (void);

    virtual void max_y (::CORBA::UShort max_y);

    virtual ::CORBA::UShort max_size (void);

    virtual void max_size (::CORBA::UShort max_size);

    virtual ::CORBA::Boolean resize_shape (void);

    virtual void resize_shape (::CORBA::Boolean resize_shape);
    //@}

    //@{
    /** Operations from Components::SessionComponent. */
    virtual void set_session_context (::Components::SessionContext_ptr ctx);
    virtual void configuration_complete (void);
    virtual void ccm_activate (void);
    virtual void ccm_passivate (void);
    virtual void ccm_remove (void);
    //@}

    //@{
    /** User defined public operations. */
    void tick ();
    void start ();
    void stop ();
    //@}

  private:
    ::Shapes::CCM_Sender_Context_var ciao_context_;

    ShapeType* square_;
    ShapeType* triangle_;
    ShapeType* circle_;
    pulse_Generator * ticker_;

    //@{
    /** Component attributes. */

    ::CORBA::ULong rate_;

    ::CORBA::UShort max_x_;

    ::CORBA::UShort max_y_;

    ::CORBA::UShort max_size_;

    ::CORBA::Boolean resize_shape_;
    //@}

    //@{
    /** User defined members. */
    TYPE_OF_SHAPE TypeOfShape;
    //@}

    //@{
    /** User defined private operations. */

    //@}

    /// Get the ACE_Reactor
    ACE_Reactor* reactor (void);

  };

  extern "C" SENDER_EXEC_Export ::Components::EnterpriseComponent_ptr
  create_Shape_Sender_Impl (void);
}

#endif /* ifndef */
