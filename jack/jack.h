/*
    Copyright (C) 2001 Paul Davis
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    
    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Id$
*/

#ifndef __jack_h__
#define __jack_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <jack/types.h>
#include <jack/error.h>

/**
 * Attemps to become a client of the Jack server.
 */

jack_client_t *jack_client_new (const char *client_name);

/**
 * Disconnects from Jack server.
 */

int            jack_client_close (jack_client_t *client);

/** 
 * @param client The Jack client structure.
 * @param function The jack_shutdown function pointer.
 * @param arg The arguments for the jack_shutdown function.
 *
 * Register a function (and argument) to be called if and when the
 * JACK server shuts down the client thread.  The function must
 * be written as if it were an asynchonrous POSIX signal
 * handler --- use only async-safe functions, and remember that it
 * is executed from another thread.  A typical function might
 * set a flag or write to a pipe so that the rest of the
 * application knows that the JACK client thread has shut
 * down.
 *
 * NOTE: clients do not need to call this.  It exists only
 * to help more complex clients understand what is going
 * on.  If called, it must be called before jack_client_activate().
*/

void jack_on_shutdown (jack_client_t *client, void (*function)(void *arg), void *arg);

/**
 * Tell the Jack serve to call 'process()' whenever there is work
 * be done.
 */
int jack_set_process_callback (jack_client_t *, JackProcessCallback, void *arg);

/**
 * Tell the Jack server to call 'bufsize()' whenever the
 * maximum number of frames that will be passed to 'process()'
 * changes.
 */
int jack_set_buffer_size_callback (jack_client_t *, JackBufferSizeCallback, void *arg);

/**
 * Tell the Jack server to call 'srate()' whenver the sample rate of
 * the system changes.
 */
int jack_set_sample_rate_callback (jack_client_t *, JackSampleRateCallback, void *arg);

int jack_set_port_registration_callback (jack_client_t *, JackPortRegistrationCallback, void *);
int jack_set_graph_order_callback (jack_client_t *, JackGraphOrderCallback, void *);

/**
 * Tell the Jack server that the program is ready to start processing
 * audio.
 */

int jack_activate (jack_client_t *client);

/**
 * Tells the Jack server that the program should be removed from the 
 * processing graph.
 */

int jack_deactivate (jack_client_t *client);


/**
   A port has a set of flags, enumerated below and passed as the third
   argument in the form of a bitmask created by AND-ing together the
   desired flags. The flags "IsInput" and "IsOutput" are mutually
   exclusive and it is an error to use them both.  
*/

enum JackPortFlags {

     JackPortIsInput = 0x1,
     JackPortIsOutput = 0x2,
     JackPortIsPhysical = 0x4, /* refers to a physical connection */

    /**
	 * if JackPortCanMonitor is set, then a call to
	jack_port_request_monitor() makes sense.
	
	Precisely what this means is dependent on the client. A typical
	result of it being called with TRUE as the second argument is
	that data that would be available from an output port (with
	JackPortIsPhysical set) is sent to a physical output connector
	as well, so that it can be heard/seen/whatever.
	
	Clients that do not control physical interfaces
	should never create ports with this bit set.
    */

     JackPortCanMonitor = 0x8,

     /**
	  * JackPortIsTerminal means:

		for an input port: the data received by the port
	                   will not be passed on or made
			   available at any other port

        for an output port: the data available at the port
	                   does not originate from any other port

        Audio synthesizers, i/o h/w interface clients, HDR
        systems are examples of things that would set this
        flag for their ports.  
     */

     JackPortIsTerminal = 0x10
     
};	    

/**
 * Used for the flag argument of jack_port_register().
 */

#define JACK_DEFAULT_AUDIO_TYPE "32 bit float mono audio"

/**
 * This creates a new port for the client. 

   A port is an object used for moving data in or out of the client.
   the data may be of any type. Ports may be connected to each other
   in various ways.

   A port has a short name, which may be any non-NULL and non-zero
   length string, and is passed as the first argument. A port's full
   name is the name of the client concatenated with a colon (:) and
   then its short name.

   A port has a type, which may be any non-NULL and non-zero length
   string, and is passed as the second argument. For types that are
   not built into the jack API (currently just
   JACK_DEFAULT_AUDIO_TYPE) the client MUST supply a non-zero size
   for the buffer as the fourth argument. For builtin types, the
   fourth argument is ignored.
*/

jack_port_t *
jack_port_register (jack_client_t *,
		    const char *port_name,
		    const char *port_type,
		    unsigned long flags,
		    unsigned long buffer_size);

/** 
 * This removes the port from the client, disconnecting
 * any existing connections at the same time.
*/

int jack_port_unregister (jack_client_t *, jack_port_t *);

/**
 * Returns the name of the jack_port_t.
 */
const char * jack_port_name (const jack_port_t *port);

/**
 * Returns the short name of the jack_port_t.
 */
const char * jack_port_short_name (const jack_port_t *port);

/**
 * Returns the flags of the jack_port_t.
 */
int          jack_port_flags (const jack_port_t *port);

/**
 * Returns the type of the jack_port_t.
 */
const char * jack_port_type (const jack_port_t *port);

/** 
 * Returns 1 if the jack_port_t belongs to the jack_client_t.
 */
int          jack_port_is_mine (const jack_client_t *, const jack_port_t *port);

/** 
 * This returns TRUE or FALSE to indicate if there are
 * any connections to/from the port argument.
*/

int jack_port_connected (const jack_port_t *port);

/**
 * This returns TRUE or FALSE if the port argument is
 * DIRECTLY connected to the port with the name given in 'portname' 
*/

int jack_port_connected_to (const jack_port_t *port, const char *portname);

/**
 * This returns TRUE or FALSE if the two ports are
 * directly connected to each other.
*/

int jack_port_connected_to_port (const jack_port_t *port, const jack_port_t *other_port);

/**
 * This returns a null-terminated array of port names to which 
 * the argument port is connected. if there are no connections, it 
 * returns NULL.

 * The caller is responsible for calling free(3) on any
 * non-NULL returned value.
*/   

const char ** jack_port_get_connections (const jack_port_t *port);

/**
 * This modifies a port's name, and may be called at any time.
*/

int jack_port_set_name (jack_port_t *port, const char *name);

/**
 * This returns a pointer to the memory area associated with the
   specified port. For an output port, it will be a memory area
   that can be written to; for an input port, it will be an area
   containing the data from the port's connection(s), or
   zero-filled. if there are multiple inbound connections, the data
   will be mixed appropriately.  

   You may cache the value returned, but only between calls to
   your "blocksize" callback. For this reason alone, you should
   either never cache the return value or ensure you have
   a "blocksize" callback and be sure to invalidate the cached
   address from there.
*/

void *jack_port_get_buffer (jack_port_t *, nframes_t);

/**
 * Establishes a connection between two ports.
 *
 * When a connection exists, data written to the source port will
 * be available to be read at the destination port.
 *
 * The types of both ports must be identical to establish a connection.
 * The flags of the source port must include PortIsOutput.
 * The flags of the destination port must include PortIsInput.
 */
int jack_connect (jack_client_t *,
		  const char *source_port,
		  const char *destination_port);

/**
 * Removes a connection between two ports.
 *
 * The types of both ports must be identical to establish a connection.
 * The flags of the source port must include PortIsOutput.
 * The flags of the destination port must include PortIsInput.
 */
int jack_disconnect (jack_client_t *,
		     const char *source_port,
		     const char *destination_port);

/**
 * Performs the exact same function as jack_connect(), but it uses
 * port handles rather than names, which avoids the name lookup inherent
 * in the name-based version.
 *
 * It is envisaged that clients connecting their own ports will use these
 * two, whereas generic connection clients (e.g. patchbays) will use the
 * name-based versions.
 */

int jack_port_connect (jack_client_t *, jack_port_t *src, jack_port_t *dst);

/**
 * Performs the exact same function as jack_disconnect(), but it uses
 * port handles rather than names, which avoids the name lookup inherent
 * in the name-based version.
 *
 * It is envisaged that clients disconnecting their own ports will use these
 * two, whereas generic connection clients (e.g. patchbays) will use the
 * name-based versions.
 */

int jack_port_disconnect (jack_client_t *, jack_port_t *);

/**
 * A client may call this on a pair of its own ports to 
 * semi-permanently wire them together. This means that
 * a client that wants to direct-wire an input port to
 * an output port can call this and then no longer
 * have to worry about moving data between them. Any data
 * arriving at the input port will appear automatically
 * at the output port.
 *
 * The 'destination' port must be an output port. The 'source'
 * port must be an input port. Both ports must belong to
 * the same client. You cannot use this to tie ports between
 * clients. That is what a connection is for.
*/

int  jack_port_tie (jack_port_t *src, jack_port_t *dst);

/**
 * This undoes the effect of jack_port_tie(). The port
 * should be same as the 'destination' port passed to
 * jack_port_tie().
*/

int  jack_port_untie (jack_port_t *port);

/**
 * A client may call this function to prevent other objects
   from changing the connection status of a port. The port
   must be owned by the calling client.
*/

int jack_port_lock (jack_client_t *, jack_port_t *);

/**
 * This allows other objects to change the connection status of a port.
 */

int jack_port_unlock (jack_client_t *, jack_port_t *);

/**
 * Returns the time (in frames) between data being available
 * or delivered at/to a port, and the time at which it
 * arrived at or is delivered to the "other side" of the port.
 * E.g. for a physical audio output port, this is the time between
 * writing to the port and when the audio will be audible.
 * For a physical audio input port, this is the time between the sound
 * being audible and the corresponding frames being readable from the
 * port.  
*/

nframes_t jack_port_get_latency (jack_port_t *port);

/**
 * The maximum of the sum of the latencies in every
 * connection path that can be drawn between the port and other
 * ports with the JackPortIsTerminal flag set.
*/

nframes_t jack_port_get_total_latency (jack_client_t *, jack_port_t *port);

/**
 * The port latency is zero by default. Clients that control
   physical hardware with non-zero latency should call this
   to set the latency to its correct value. Note that the value
   should include any systemic latency present "outside" the
   physical hardware controlled by the client. For example,
   for a client controlling a digital audio interface connected
   to an external digital converter, the latency setting should
   include both buffering by the audio interface *and* the converter. 
*/

void jack_port_set_latency (jack_port_t *, nframes_t);

/**
 * If JackPortCanMonitor is set for a port, then these 2 functions will
   turn on/off input monitoring for the port. If JackPortCanMonitor
   is not set, then these functions will have no effect.
*/
int jack_port_request_monitor (jack_port_t *port, int onoff);

/**
 * If JackPortCanMonitor is set for a port, then these 2 functions will
   turn on/off input monitoring for the port. If JackPortCanMonitor
   is not set, then these functions will have no effect.
*/
int jack_port_request_monitor_by_name (jack_client_t *client, const char *port_name, int onoff);

/**
 * If JackPortCanMonitor is set for a port, then this function will
 * turn on input monitoring if it was off, and will turn it off it
 * only one request has been made to turn it on.  If JackPortCanMonitor
 * is not set, then this function will do nothing.
*/

int jack_port_ensure_monitor (jack_port_t *port, int onoff);

/**
 * Returns a true or false value depending on whether or not 
 * input monitoring has been requested for 'port'.
*/

int jack_port_monitoring_input (jack_port_t *port);

/**
 * This returns the sample rate of the jack system 
*/

unsigned long jack_get_sample_rate (jack_client_t *);

/**
 * This returns the current maximum size that will
 * ever be passed to the "process" callback.  It should only
 * be used *before* the client has been activated.  After activation,
 * the client will be notified of buffer size changes if it
 * registers a buffer_size callback.
*/

nframes_t jack_get_buffer_size (jack_client_t *);

/**
 * @param port_name_pattern A regular expression used to select 
 * ports by name.  If NULL or of zero length, no selection based 
 * on name will be carried out.
 * @param type_name_pattern A regular expression used to select 
 * ports by type.  If NULL or of zero length, no selection based 
 * on type will be carried out.
 * @param flags A value used to select ports by their flags.  
 * If zero, no selection based on flags will be carried out.

 * This function returns a NULL-terminated array of ports that match 
 * the specified arguments.
 * The caller is responsible for calling free(3) any non-NULL returned value.
*/

const char ** jack_get_ports (jack_client_t *, 
							const char *port_name_pattern, 
							const char *type_name_pattern, 
							unsigned long flags);

/**
 * Searchs for and returns the jack_port_t with the name value
 * from portname.
 */
jack_port_t *jack_port_by_name (jack_client_t *, const char *portname);

/**
 * If a client is told (by the user) to become the timebase
   for the entire system, it calls this function. If it
   returns zero, then the client has the responsibility to
   call jack_set_transport_info()) at the end of its process()
   callback. 
*/

int  jack_engine_takeover_timebase (jack_client_t *);

/**
 * undocumented
 */
void jack_update_time (jack_client_t *, nframes_t);

/**
 * This estimates the time that has passed since the
 * start jack server started calling the process()
 * callbacks of all its clients.
*/

nframes_t jack_frames_since_cycle_start (jack_client_t *);

#ifdef __cplusplus
}
#endif

#endif /* __jack_h__ */

