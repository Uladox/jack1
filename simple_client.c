#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <jack/jack.h>

jack_port_t *input_port;
jack_port_t *output_port;

int
process (nframes_t nframes, void *arg)

{
	sample_t *out = (sample_t *) jack_port_get_buffer (output_port, nframes);
	sample_t *in = (sample_t *) jack_port_get_buffer (input_port, nframes);

	memcpy (out, in, sizeof (sample_t) * nframes);

	return 0;      
}

int
bufsize (nframes_t nframes, void *arg)

{
	printf ("the maximum buffer size is now %lu\n", nframes);
	return 0;
}

int
srate (nframes_t nframes, void *arg)

{
	printf ("the sample rate is now %lu/sec\n", nframes);
	return 0;
}

int
main (int argc, char *argv[])

{
	jack_client_t *client;

	if (argc < 2) {
		fprintf (stderr, "usage: aeclient <name>\n");
		return 1;
	}

	if ((client = jack_client_new (argv[1])) == 0) {
		fprintf (stderr, "jack server not running?\n");
		return 1;
	}

	jack_set_process_callback (client, process, 0);
	jack_set_buffer_size_callback (client, bufsize, 0);
	jack_set_sample_rate_callback (client, srate, 0);

	printf ("engine sample rate: %lu\n", jack_get_sample_rate (client));

	input_port = jack_port_register (client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	output_port = jack_port_register (client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
	}

	printf ("client activated\n");

	if (jack_port_connect (client, "ALSA I/O:Input 1", jack_port_name (input_port))) {
		fprintf (stderr, "cannot connect input ports\n");
	} 
	
	if (jack_port_connect (client, jack_port_name (output_port), "ALSA I/O:Output 1")) {
		fprintf (stderr, "cannot connect output ports\n");
	} 

	sleep (5);

	printf ("done sleeping, now closing...\n");
	jack_client_close (client);
	exit (0);
}

