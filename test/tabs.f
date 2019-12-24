IF echo "	"
	DO echo @SUCCESS
IF NOT echo "	"
	DO echo @FAILURE
IF test -z '	'
	DO echo @FAILURE
