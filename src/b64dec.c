/*
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2017, Peter Haemmerlein (peterpawn@yourfritz.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, please look for the file LICENSE.
 */

#define B64DEC_C

#include "common.h"
#include "b64dec_usage.c"

static	char *				__commandNames[] = {
#include "b64dec_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static 	commandEntry_t 		__b64dec_command = { .names = &commandNames, .ep = &b64dec_entry, .usage = &b64dec_usage };
EXPORTED commandEntry_t *	b64dec_command = &__b64dec_command;

// statics

//// error messages ////
static	char *			errorInvalidValue = "Invalid data value encountered on STDIN.\n";
static	char *			errorInvalidDataSize = "Invalid data size encountered on STDIN.\n";
static	char *			errorUnexpectedError = "Unexpected error %d (%s) encountered.\n";
static	char *			errorWriteFailed = "Write to STDOUT failed.\n";
//// end ////

// 'b64dec' function - decode Base64 encoded data from STDIN to STDOUT

int		b64dec_output(char * base64, bool hexOutput, bool pad)
{
	char				binary[3];
	size_t				binarySize = base64ToBinary(base64, (size_t) -1, binary, sizeof(binary), pad);
	char				hex[6];
	char *				out;
	size_t				outSize;

	if (isAnyError()) /* usually invalid characters */
	{
		if (isError(INV_B64_DATA))
		{
			errorMessage(errorInvalidValue);
		}
		else if (isError(INV_B64_SIZE))
		{
			errorMessage(errorInvalidDataSize);
		}
		else
		{
			errorMessage(errorUnexpectedError, getError(), getErrorText(getError()));
		}
		return EXIT_FAILURE;
	}
	if (hexOutput)
	{
		outSize = binaryToHexadecimal(binary, binarySize, hex, sizeof(hex));
		out = hex;
	}
	else
	{
		outSize = binarySize;
		out = binary;
	}
	if (fwrite(out, outSize, 1, stdout) != 1)
	{
		setError(WRITE_FAILED);
		errorMessage(errorWriteFailed);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int		b64dec_entry(int argc, char** argv, int argo, commandEntry_t * entry, const char * name)
{
	char				buffer[80];
	char *				input;
	char				base64[5];
	int					convUsed = 0;
	bool				hexOutput = false;
	bool				padOutput = false;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
			{ "pad-output", no_argument, NULL, 'p' },
		};
		char *			options_short = "xp" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					entry->finalNewlineOnTTY = true;
					break;

				case 'p':
					padOutput = true;
					break;

				check_verbosity_options_short();
				help_option();
				getopt_message_displayed();
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((input = fgets(buffer, sizeof(buffer), stdin)) != NULL)
	{
		input--;
		while (*(++input))
		{
			if (isspace(*input))
				continue;
			base64[convUsed++] = *input;
			if (convUsed == 4)
			{
				int		result;

				base64[convUsed] = 0;
				if ((result = b64dec_output(base64, hexOutput, padOutput)))
					return result;
				convUsed = 0;
			}
		}
	}	

	if (convUsed > 0) /* remaining data exist */
	{
		base64[convUsed] = 0;
		return b64dec_output(base64, hexOutput, padOutput);
	}
	
	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}
