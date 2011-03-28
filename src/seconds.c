/*
 * seconds.c
 *
 *  Created on: Dec 19, 2010
 *      Author: derrickstolee
 */

#include <stdio.h>

int main(void)
{
	double seconds;

	while ( scanf("%lf",&seconds) != EOF )
	{
		long long int seconds_int = seconds;

		int msecs, secs, mins, hrs, days, yrs;

		msecs = (seconds - (double)seconds_int) * 100;

		secs = seconds_int % 60;
		mins = (seconds_int % 3600)/60;
		hrs = (seconds_int % (3600 * 24)) / 3600;
		days = (seconds_int % (3600 * 24 * 365)) / (3600 * 24);
		yrs = seconds_int / (3600*24*365);

		printf("%2d & %3d & %2d & %2d  & %2d&%02d \\\\\n", yrs, days, hrs, mins, secs, msecs);
	}

	return 0;
}
