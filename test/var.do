SET %FAIL% @FAILURE
SET %MEOW% @SUCCESS

IF true
	DO echo %MEOW%

IF false
	DO echo %FAIL%

SET % @FAILURE
SET %% @FAILURE

DO echo %
DO echo %%
