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

// display usage help

void 	userpw_usage(const char * name, const bool help, UNUSED const bool version)
{
	FILE *	out = (help || version ? stdout : stderr);

	showUsageHeader(out, name, help, version);

	if (version)
	{
		fprintf(out, "\n");
		return;
	}

	showPurposeHeader(out, name);
	fprintf(out,
		"This program generates a key, based on the specified password. The key may be used for the first\n"
		"stage of decryption for an export file or for decryption of a single value (with another program\n"
		"named %s).\n",
		showBold(DECODER_CONFIG_DECRYPT_SINGLE_VALUES_NAME)
	);

	showFormatHeader(out, name);
	addSpace();
	addOption("options");
	addSpace();
	endOptions();
	addSpace();
	addArgument("password");
	showFormatEnd(out, name);

	showOptionsHeader("options");
	addOptionsEntry("-x, --hex-output", "output data as a hexadecimal string");
	addOptionsEntryVerbose();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out, name);

	fprintf(out,
		"\nAn export file has to be created with a user-specified password or the decryption will fail, 'cause\n"
		"the device's secret key - computed from only two device properties - was used to encrypt the key\n"
		"for the second stage.\n\n"
		"The generated key will be written to STDOUT; if the option '--hex-output' (or '-x') was specified,\n"
		"output data will be converted to a hexadecimal string.\n\n"
		"If %s starts with a hyphen, you have to insert '--' in front of it.\n",
		showUndl("password")
	);

	showUsageFinalize(out, name, help, version);
}
