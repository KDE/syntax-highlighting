// This is from Odin examples:
// https://github.com/odin-lang/examples/blob/master/by_example/dir_info/main.odin

package main

import "core:fmt"
import "core:os"
import "core:path/filepath"
import "core:strings"

/**
 * This is
 * a multiline
 * comment
 */

packedDirective :: struct #packed {
	x: u8,
	y: i32,
	z: u16,
	w: u8,
}

Foo :: enum {
	A,
	B,
	C,
	D,
}

@private
_SS_MAXSIZE   :: 128

@require_results
some_proc::proc "contextless"() -> i32{
    return 0
}

@(private = "file")
print_file_info :: proc(fi: os.File_Info) {
    context.user_index = 100
    flt := 12.33
    flt_e := 1e9
	bin_ok := 0b110001
	bin_wrong := 0b1122001010123123
	oct := 0o1237
	oct_wrong := 0o1239999
	hex := 0xFF123AC
	hex_wrong := 0xFF123ACHH

	// Split the path into directory and filename
	_, filename := filepath.split(fi.fullpath)

	SIZE_WIDTH :: 12
	buf: [SIZE_WIDTH]u8

	raw_str := `this_is raw ;';@@""\\\' string`
	// Print size to string backed by buf on stack, no need to free
	_size := "-" if fi.is_dir else fmt.bprintf(buf[:], "%v", fi.size)

	a: u8 = cast(u8)'"'
	b: u8 = '"'

	// Right-justify size for display, heap allocated
	size := strings.right_justify(_size, SIZE_WIDTH, " ")
	defer delete(size)

	val: i32 = 1
	switch (val) {
	case 1:
		val = 2
	case 2:
		val = 3
	case:
		val = 0
	}

	// This should not turn anything below into string colors
	triggerCharacter := "\""

	if fi.is_dir {
		fmt.printf ("%v [%v]\n word", size, filename)
	} else {
		fmt.printf("%v %v\n", size, filename)
	}
}

main :: proc() {
	cwd := os.get_current_directory()
	fmt.println("Current working directory:", cwd)

	f, err := os.open(cwd)
	defer os.close(f)

	if err != os.ERROR_NONE {
		// Print error to stderr and exit with errorcode
		fmt.eprintln("Could not open directory for reading", err)
		os.exit(1)
	}

	fis: []os.File_Info
	defer os.file_info_slice_delete(fis) // fis is a slice, we need to remember to free it

	fis, err = os.read_dir(f, -1) // -1 reads all file infos
	if err != os.ERROR_NONE {
		fmt.eprintln("Could not read directory", err)
		os.exit(2)
	}

	for fi in fis {
		print_file_info(fi)
	}

	enumTest := Foo.A
	#partial switch enumTest {
	case .A:
		fmt.println("A letter\n nextline \xfe \u200B \000 \U0001f4ae")
	case .D:
		fmt.println("D")
	}
}

