#include <stdio.h>
#include <glib.h>
#include <ocstack.h>

static gboolean
runOCProcess( gpointer user_data ) {
	OCStackResult result = OCProcess();
	GMainLoop *mainLoop = ( GMainLoop * )user_data;

	if ( result == OC_STACK_OK ) {
		g_message( "client: OCProcess() succeeded" );
		return G_SOURCE_CONTINUE;
	} else {
		g_warning( "client: OCProcess() did not return OC_STACK_OK - quitting main loop" );
		g_main_loop_quit( mainLoop );
		return G_SOURCE_REMOVE;
	}
}

static OCStackApplicationResult
defaultCb( void *context, OCDoHandle handle, OCClientResponse *response ) {
	g_message( "client: defaultCb() has received a message" );

	return OC_STACK_DELETE_TRANSACTION;
}

static void
addCallback() {
	OCDoHandle handle;
	OCCallbackData cbData = {
		NULL,
		defaultCb,
		NULL
	};

	if ( OC_STACK_OK != OCDoResource(
			&handle,
			OC_REST_GET,
			"light",
			NULL,
			NULL,
			OC_ALL,
			OC_HIGH_QOS,
			&cbData,
			NULL,
			0 ) ) {

		g_warning( "client: OCDoResource() failed" );
	} else {
		g_message( "client: OCDoResource() succeeded" );
	}
}

int
main( int argc, char **argv ) {
	GMainLoop *mainLoop = g_main_loop_new( NULL, FALSE );

	if ( OC_STACK_OK == OCInit( NULL, 0, OC_CLIENT ) ) {
		g_message( "client: OCInit() succeeded" );

		addCallback();
		g_timeout_add( 100, runOCProcess, mainLoop );
		g_main_loop_run( mainLoop );
		OCStop();
		return 0;
	}

	g_warning( "client: OCInit() failed" );
	return 1;
}