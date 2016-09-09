-- kate: hl SQL (Oracle)
-- test case shipped with highlighting in bugzilla, LGPL

begin
	q'{adfasdf'sadfasdf j}'{' hkjhkhkjhkjh khlkhklj'fghdfghdfgh'hkjh'jkhkh'a'
	Q'(asldflahsl;'dkjfhklsdfh)'
	q'[asdasd;'asdasd'a]sd'asd'asasd]';11111111[1']; asdasdasdasd'errrrrrrrrrrrrrr-p

	q'agdfgsdfgfhfghjfgh'f'f'sdfg'sdfg'dfg#a' dafdfadasfasdf;

	Q'#gdfgsdfgsdfgsdfgsdfg#' afgasasdfasdfasfasdfasdfasdfasdfsdf

	if sldfjsdj then
		case
			when 1=1 then
				aslfjsldkfj;
			when 2=2 then
				asdfg;
			else
				null;
		end case;
		
		if sdfjh then
			for i in 1 .. 2
			LOOP
				dbms_output.put_line(q';ololo;');
			END Loop;
			
			while true
			loop
				dbms_output.put_line('1111');
			end loop;
		end if;
		ksjfklasjd;
		fklj;
	elsif
		sdklfjsdklfj;
	else
		sdfdfsdf;
	end if;
end;