IF echo "	"
	DO echo @SUCCESS
NIF echo "	"
	DO echo @FAILURE
IF test -z '	'
	DO echo @FAILURE
