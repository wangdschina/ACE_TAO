//
// $Id$
//
#include <iostream.h>
#include "CosNaming_i.h"
#include "svr.h"

// This is a startup for the naming server.
// This is used for testing of the Naming Service.

ACE_HANDLE
IOR_Multicast::get_handle (void) const
{
  return this->mcast_dgram_.get_handle ();
}

IOR_Multicast::IOR_Multicast (char * ior,
			      u_short port, 
			      const char *mcast_addr,
			      u_short response_port)
  : ior_ (ior),
    mcast_addr_ (port, mcast_addr),
    SERVICE_RESPONSE_UDP_PORT_ (response_port),
    response_ (response_addr_)
{
  // Use ACE_SOCK_Dgram_Mcast factory to subscribe to multicast group.
  if (this->mcast_dgram_.subscribe (this->mcast_addr_) == -1)
    ACE_ERROR ((LM_ERROR, "%p\n", "subscribe"));
}

// destructor 

IOR_Multicast::~IOR_Multicast (void)
{
  this->mcast_dgram_.unsubscribe ();    
}

int
IOR_Multicast::handle_timeout (const ACE_Time_Value &,
                               const void *arg)
{
  return 0;
}

int
IOR_Multicast::handle_input (ACE_HANDLE)
{
  ssize_t retcode = 
    this->mcast_dgram_.recv (this->buf_, 
			     BUFSIZ, 
			     this->remote_addr_);
  
  if (retcode == -1)
    return -1;

  ACE_DEBUG ((LM_DEBUG, "Received multicast.\n"));
  
  // @@ validate data string received is from a valid client here
  // @@ Probably not needed

  this->remote_addr_.set_port_number (this->SERVICE_RESPONSE_UDP_PORT_);
  retcode = response_.send (this->ior_, 
			    ACE_OS::strlen (this->ior_) + 1, 
			    this->remote_addr_, 
			    0);

  ACE_DEBUG ((LM_ERROR, "ior_ '%s' sent.\nretcode=%d\n", this->ior_, retcode));

  if (retcode == -1)
    return -1;  

  return 0;
}


int
main (int argc, char ** argv)
{
  CORBA::Environment env;
  char *orb_name = "internet";

  CORBA::ORB_ptr orb_ptr = CORBA::ORB_init (argc, 
					    argv, 
					    orb_name, 
					    env);

  if (env.exception () != 0)
    {
      env.print_exception ("ORB init");
      return 1;
    }

  // Initialize the Object Adapter
  CORBA::POA_ptr oa_ptr = orb_ptr->POA_init (argc, argv, "POA");

  if (oa_ptr == 0)
    ACE_ERROR_RETURN ((LM_ERROR,
		       " (%P|%t) Unable to initialize the POA.\n"),
		      1);

  // Create a naming context object.
  NS_NamingContext *naming_context = new NS_NamingContext;
  
  // Stringify the objref we'll be implementing, and print it to
  // stdout.  Someone will take that string and give it to a
  // client.  Then release the object.
  CORBA::String str;
  str = orb_ptr->object_to_string (naming_context, env);

  if (env.exception () != 0)
    {
      env.print_exception ("object2string");
      return 1;
    }

  ACE_DEBUG ((LM_DEBUG, "listening as object '%s'\n", str));

#if defined (ACE_HAS_IP_MULTICAST)
  // get reactor instance from TAO
  ACE_Reactor *reactor = TAO_ORB_Core_instance()->reactor();
  
  // Instantiate a server which will receive requests for an ior
  IOR_Multicast ior_multicast (str,
			       TAO_DEFAULT_NAME_SERVER_REQUEST_PORT, 
			       ACE_DEFAULT_MULTICAST_ADDR,
			       TAO_DEFAULT_NAME_SERVER_REPLY_PORT); 

  // register event handler for the ior multicast.
  if (reactor->register_handler (&ior_multicast,
				 ACE_Event_Handler::READ_MASK) == -1)
    ACE_ERROR ((LM_ERROR, "%p\n%a", "register_handler", 1));
    
  ACE_DEBUG ((LM_DEBUG, "The multicast server setup is done.\n"));
#endif /* ACE_HAS_IP_MULTICAST */

  // Handle requests for this object until we're killed, or one of the
  // methods asks us to exit.
  if (orb_ptr->run () == -1)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "run"), -1);

  return 0;
}
