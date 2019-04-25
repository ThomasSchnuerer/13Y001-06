/****************************************************************************
 ************                                                    ************
 ************                   LM63_TEST                        ************
 ************                                                    ************
 ****************************************************************************/
/*!
 *         \file lm63_test.c
 *       \author michael.roth@men.de
 *        $Date: 2009/10/02 17:38:34 $
 *    $Revision: 1.2 $
 *
 *        \brief Test program for the LM63 driver.
 *
 *               Reads from all channels and compares the values
 *
 *     Required: libraries: mdis_api, usr_oss, usr_utl
 *     \switches (none)
 *
 *--------------------------------[ History ]--------------------------------
 *
 * $Log: lm63_test.c,v $
 * Revision 1.2  2009/10/02 17:38:34  MRoth
 * R: syntax fault caused Compiler Error
 * M: changed calculation of temperature difference
 *
 * Revision 1.1  2009/10/02 13:15:10  MRoth
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2009 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
static char *RCSid="$Id: lm63_test.c,v 1.2 2009/10/02 17:38:34 MRoth Exp $\n";

/*-------------------------------------+
|   DEFINES                            |
+-------------------------------------*/
#define MAX_TEMP		60
#define DEFAULT_DIFF	10

/*-------------------------------------+
|   INCLUDES                           |
+-------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/lm63_drv.h>

/*-------------------------------------+
|   PROTOTYPES                         |
+-------------------------------------*/
static void PrintError( char* );

/********************************* header **********************************/
/**  Prints the headline
 */
static void header(void)
{
	printf("\n======================="
		   "\n===    LM63_TEST    ==="
		   "\n======================="
		   "\n(c)Copyright 2009 by MEN Mikro Elektronik GmbH\n%s\n\n", RCSid);
}

/********************************* usage ***********************************/
/**  Prints the program usage
 */
static void usage(void)
{
	printf(
		"\nUsage:     lm63_test <device> [<opts>] \n"
		"\nFunction:  LM63 test tool to check temperatures"
		"\nOptions: \n"
		"    device    device name e.g. lm63_1 \n"
		"    [-t=dec]  max. temperature - default: 60 degree Celsius \n"
		"    [-d=dec]  max. difference between DIE and REMOTE temperature \n"
		"              - default: 10 degree Celsius \n"
		"\nCalling examples:\n"
		"\n - test with default values: \n"
		"     lm63_test lm63_1 \n"
		"\n - test with user define values: \n"
		"     lm63_test lm63_1 -t=80 -d=5   \n"
		"\n(c)Copyright 2009 by MEN Mikro Elektronik GmbH\n%s\n\n", RCSid
		);
}

/***************************************************************************/
/** Program main function
 *
 *  \param argc       \IN  argument counter
 *  \param argv       \IN  argument vector
 *
 *  \return	          success (0) or error (1)
 */
int main(int argc, char *argv[])
{
	MDIS_PATH path;
	char      *deviceP=NULL;
	char      *optp=NULL, *errstr=NULL, ebuf[100];
	int32	  ch, nbrOfCh, ret, value, tempDie, tempRmt, tempDiff;
	int32     maxtemp, tempDelta;

	/*--------------------+
    |  check arguments    |
    +--------------------*/
    errstr = UTL_ILLIOPT( "?d=t=", ebuf );
	if( errstr ) {
		printf( "*** %s\n", errstr );
		usage();
		return(1);
	}
	if( UTL_TSTOPT("?") ) {
		usage();
		return(0);
	}

	/*--------------------+
	|  get arguments      |
	+--------------------*/
	deviceP = argv[1];

	if( !deviceP ) {
		printf( "\n***ERROR: missing LM63 device name!\n" );
		usage();
		return(1);
	}

	maxtemp   = ( (optp = UTL_TSTOPT("t=")) ? atoi(optp) : MAX_TEMP );		/* max temp */
	tempDelta = ( (optp = UTL_TSTOPT("d=")) ? atoi(optp) : DEFAULT_DIFF );	/* max diff */

	/*--------------------+
    |  open path          |
    +--------------------*/
	if ((path = M_open(deviceP)) < 0) {
		PrintError("open");
		return(1);
	}

	header();

	/* get number of channels */
	if ((M_getstat(path, M_LL_CH_NUMBER, &nbrOfCh)) < 0) {
		PrintError("setstat M_MK_IO_MODE");
		goto abort;
	}

	/*--------------------+
    |  config             |
    +--------------------*/
	/* repeat until keypress */
	do {
		for( ch=0; ch<nbrOfCh; ch++ ) {
			/* set channel number */
			if ((M_setstat(path, M_MK_CH_CURRENT, ch)) < 0) {
				PrintError("setstat M_MK_CH_CURRENT");
				goto abort;
			}

			/* read value from current channel (then increment channel)  */
			if ((M_read(path, &value )) < 0 ){
				PrintError("read");
				goto abort;
			}

			switch( ch ) {
				case LM63_CH_TEMP:
					tempDie = value;
					if( tempDie > maxtemp ) {
						printf( "\nLM63 die temperature   : %d degree celcius > max. %d degree celcius\n"
								"--> WARNING: TEMPERATURE TOO HIGH!!!\n"
								"\naborting...\n", (int)tempDie, (int)maxtemp );
						goto abort;
					}
					else {
						printf("\nLM63 die temperature : %d degree celcius --> OK\n", (int)tempDie);
					}
					break;

				case LM63_CH_RMTTEMP:
					tempRmt = value;
					if( tempRmt > maxtemp ) {
						printf( "Remote temperature   : %d degree celcius > max. %d degree celcius\n"
								"--> WARNING: TEMPERATURE TOO HIGH!!!\n"
								"\naborting...\n", (int)tempRmt, (int)maxtemp );
						goto abort;
					}
					else {
						printf("Remote temperature   : %d degree celcius --> OK\n", (int)tempRmt );
					}
					break;

				case LM63_CH_FANSPEED:
					break;
			}
		}

		UOS_Delay(500);

		/* check temperature difference */
		tempDiff = tempRmt-tempDie;
		if (tempDiff < 0) {
			tempDiff = tempDiff * (-1); /* Betrag, we only want positive values */
		}

		if( tempDiff > tempDelta ) {
			printf( "\nTemperature Delta    : %d degree celcius > %d degree celcius\n"
					"--> WARNING: TEMPERATURE DELTA TOO HIGH!!!\n"
					"\naborting...\n", (int)tempDiff, (int)tempDelta );
			goto abort;
		}
		else {
			printf( "\nTemperature Delta    : %d degree celcius "
					"--> OK \n", (int)tempDiff );
		}

	} while( UOS_KeyPressed() == -1 );

	/*--------------------+
    |  cleanup            |
    +--------------------*/
	abort:
	if (M_close(path) < 0) {
		PrintError("close");
		return(1);
	}

	return(0);
}

/********************************* PrintError ******************************/
/** Print MDIS error message
 *
 *  \param info       \IN  info string
*/
static void PrintError(char *info)
{
	printf("*** can't %s: %s\n", info, M_errstring(UOS_ErrnoGet()));
}
