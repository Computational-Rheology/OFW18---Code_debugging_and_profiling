define printField
	set $fieldSize = $arg0.size_
	set $formatType = 0
	set $fieldType = $arg0.typeName._M_dataplus._M_p

	set max-value-size unlimited
	set print elements 0
	set print repeats 0
	set pagination off

	if $argc > 1
		set logging file $arg1
		set logging redirect on
		set logging enabled on
	end

	if ($_regex($fieldType, ".*[sS]calar.*"))
		print *$arg0.v_@$fieldSize
	else
		print ($arg0.v_).v_@$fieldSize
		set $formatType = 1
	end
		
	if $argc > 1
		set logging enabled off
		set max-value-size 65536
		set print elements 200
		set print repeats 10
		set pagination on

		if ($formatType == 0)
			shell sed -i -e '/$[0-9]*/s/}.*$//g' -e '/$[0-9]*/s/, /\n/g' -e 's/.*$[0-9]* = {//' $arg1
		else
			shell sed -i -e '/$[0-9]*/s/}}.*/)/' -e '/$[0-9]*/s/}, {/)\n(/g' -e 's/.*$[0-9]* = {{/(/' -e 's/,/ /g' -e 's/  */ /g' $arg1
		end
	end

	# Reset values of convenience variables
	set $fieldSize = -1
	set $formatType = -1
	set $fieldType = "notInUse"

end

document printField
	Print internalField of vol<type>Field
	Function can take up to 2 arguments: vf, fileName

	Examples:
		printField vf: prints internalField of vf to the screen
		printField vf store.txt: saves internalField of vf to a file named fileName
end

