SET %FAIL% LALALALA
SET %MEOW% @SUCCESS

IF true
	DO echo %MEOW%

IF false
	@FAILURE

IF test ! %FAIL% = LALALALA
	@FAILURE

DO echo %
DO echo %%

DO echo %ALALALlA%
