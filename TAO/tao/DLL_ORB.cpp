// -*- C++ -*-

#include "tao/DLL_ORB.h"
#include "tao/TAO_Singleton_Manager.h"
#include "tao/Environment.h"
#include "tao/debug.h"
#include "tao/CORBA_methods.h"

#include "ace/ARGV.h"
#include "ace/Argv_Type_Converter.h"

ACE_RCSID (tao,
           DLL_ORB,
           "$Id$")

#if !defined (__ACE_INLINE__)
# include "tao/DLL_ORB.inl"
#endif  /* ! __ACE_INLINE__ */

TAO_DLL_ORB::TAO_DLL_ORB (void)
  : orb_ ()
{
  // Nothing
}

TAO_DLL_ORB::~TAO_DLL_ORB (void)
{
  // Nothing
}

int
TAO_DLL_ORB::init (int /*argc*/, ACE_TCHAR *argv[])
{
  // This class is deprecated.  See the class documentation in
  // DLL_ORB.h for details explaining why this is so.
  if (TAO_debug_level > 0)
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("TAO (%P|%t) - The TAO_DLL_ORB class is ")
                  ACE_TEXT ("deprecated.  See the class documentation\n")
                  ACE_TEXT ("TAO (%P|%t) - `tao/DLL_ORB.h' for details ")
                  ACE_TEXT ("explaining why this is so.\n")));
    }

  // Make sure TAO's singleton manager is initialized, and set to not
  // register itself with the ACE_Object_Manager since it is under the
  // control of the Service Configurator.
  int register_with_object_manager = 0;

  if (TAO_Singleton_Manager::instance ()->init (register_with_object_manager)
        == -1)
    {
      return -1;  // No exceptions yet.
    }

  ACE_TRY_NEW_ENV
    {
      ACE_ARGV new_argv;

      if (new_argv.add (ACE_TEXT ("dummy")) == -1
          || new_argv.add (argv) == -1)
        return -1;

      int new_argc = new_argv.argc ();

#if defined (ACE_HAS_WINCE) || defined (ACE_USES_WCHAR)
      {
          // Copy command line parameter from new_argv.
          ACE_Argv_Type_Converter command_line(new_argc, new_argv.argv());

          // Initialize the ORB.
          this->orb_ = CORBA::ORB_init (command_line.get_argc(),
                                        command_line.get_ASCII_argv(),
                                        ""
                                         ACE_ENV_ARG_PARAMETER);
          // command_line will be destroyed here and new_argv will be updated if necessary
      }
#else
      // Initialize the ORB.
      this->orb_ = CORBA::ORB_init (new_argc,
                                    new_argv.argv (),
                                    ""
                                     ACE_ENV_ARG_PARAMETER);
#endif  // ACE_HAS_WINCE

      ACE_TRY_CHECK;

      if (CORBA::is_nil (this->orb_.in ()))
        return -1;
    }
  ACE_CATCHANY
    {
      if (TAO_debug_level >= 4)
        ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                             "TAO_DLL_ORB::init");

      return -1;
    }
  ACE_ENDTRY;

#if defined (ACE_HAS_THREADS)
      // Become an Active Object so that the ORB
      // will execute in a separate thread.
      return this->activate ();
#else
      return 0;
#endif /* ACE_HAS_THREADS */
}

int
TAO_DLL_ORB::fini (void)
{
  ACE_TRY_NEW_ENV
    {
      this->orb_->destroy (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      if (TAO_Singleton_Manager::instance ()->fini () == -1)
        return -1;
    }
  ACE_CATCHANY
    {
      if (TAO_debug_level >= 4)
        ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                             "TAO_DLL_ORB::fini");
      return -1;
    }
  ACE_ENDTRY;

  return 0;
}

int
TAO_DLL_ORB::svc (void)
{
  ACE_TRY_NEW_ENV
    {
      // Run the ORB event loop in its own thread.
      this->orb_->run (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      if (TAO_debug_level >= 4)
        ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                             "TAO_DLL_ORB::svc");
      return -1;
    }
  ACE_ENDTRY;

  return 0;
}


ACE_STATIC_SVC_DEFINE (TAO_DLL_ORB,
                       ACE_TEXT ("DLL_ORB"),
                       ACE_SVC_OBJ_T,
                       &ACE_SVC_NAME (TAO_DLL_ORB),
                       ACE_Service_Type::DELETE_THIS
                       | ACE_Service_Type::DELETE_OBJ,
                       0)

ACE_FACTORY_DEFINE (TAO, TAO_DLL_ORB)



// Template instantiations necessary for use when dynamically load the
// TAO_DLL_ORB.

#if 0
#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Dynamic_Service<TAO_DLL_ORB>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Dynamic_Service<TAO_DLL_ORB>

#endif

#endif /*if 0*/
