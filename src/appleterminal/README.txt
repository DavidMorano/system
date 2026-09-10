APPLETERMINAL

This program sends a command to the Apple-Terminal to set either
the Present-Working-Directory (PWD) or a current "document" text
string.

Synopsis:
$ appleterminal_pwd <current-path> <optional-hostname>
$ appleterminal_doc <document>

Arguments:
	<pwd>		present-working-directory
	<doc>		document string
	<hostname>	optional hostname string

Returns:
	EXIT_SUCCESS	success
	EXIT_FAILURE	failure

