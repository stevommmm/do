IF echo "	"
	DO echo @SUCCESS
IF NOT echo "	"
	@FAILURE
IF test -z '	'
	@FAILURE
