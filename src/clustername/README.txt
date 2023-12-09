CLUSTERNAME

This is a Korn Shell (KSH) built-in command.  It prints out the name
of the primary cluster associated with the specified node.  If no node
is specified, then the name of the primary cluster for the current node
is printed.

Synopsis:
$ clustername [ { [<node(s)>] [-l] } | -a ] [-V]

Arguments:
<node(s)>	node to print primary cluster name for
-l		print all clusters for given node (not just primary)
-a		list all cluster names (specified nodes are ignored)
-V		print command version to standard-error and then exit

