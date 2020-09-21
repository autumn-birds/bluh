#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Used references:
// - mostly just https://www.cplusplus.com/reference
// (for how to API calls)

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Please give one argument: the raw / assembled binary\n");
		exit(1);
	}

	// This program means to make a valid boot sector out of some assembled code
	// (assuming the programmer wanted to know how many bytes it took up, or just...
	// whatever), by adding the required extra bits and perhaps filling with zeros.
	// It's probably unnecessary.  Oh well.

	char *fn = argv[1];
	FILE *bin_in = fopen(fn, "rb");
	if (bin_in == NULL) {
    		fprintf(stderr, "Couldn't open %s\n", fn);
    		exit(1);
	}

	// Find out the file size.

 	fseek(bin_in, 0, SEEK_END);
	int in_len = ftell(bin_in);

	if (in_len < 1) {
    		fprintf(stderr, "File size of %d doesn't make sense.\n");
	}

	// Ensure that the provided binary is, in fact, small enough to fit in a floppy
	// boot sector.  That is, 512 bytes minus the two special bytes (at the end),
	// which must be some sort of magic number.

	if (in_len > 510) {
    		fprintf(stderr, "%d (the size of %s) is too big to fit in a floppy boot sector.\n",
    		        in_len,
    		        fn);
    		exit(1);
	}

	// Read in the whole file.  Just to make things simpler we set up the complete buffer of
	// the sector beforehand and read directly into that.

	char sector[512];
	memset(sector, 0, 512);

	rewind(bin_in);
	int res = fread(sector, 1, in_len, bin_in);
	fclose(bin_in);

	// Just for simplicity's sake and because we're reading so little, we'll try
	// failing at the slightest provocation.
	
	if (res != in_len) {
    		fprintf(stderr, "Could only read %d bytes.  Something's fishy...\n");
    		exit(1);
	}

	// This is a magic number appearing at the end of the boot sector, which tells the
	// BIOS that it's bootable.  I can't figure out whether it's stored on disk as little
	// endian or not, or what the actual value should be in that case.

	// Remember that the array is zero-indexed.  It sounds basic but I actually made this
	// mistake.

	sector[511] = 0xAA;
	sector[510] = 0x55;

	// Now everything should be:
    	// - the read in file
    	// - zeros (if read() does what I think it does) (and only if in_len < 510)
    	// - 0xAA55 (or is it 0x55AA? I'm so confused.)
    	//
    	// We just dump out the image on stdout at this point.  Make sure you know how to
    	// reset your terminal I guess, just in case.

	// Also we'll warn the programmer about how larger her boot sector assembled down
	// to.

	fprintf(stderr, "note: Your input file was %d bytes long (of up to 510)\n", in_len);
	fprintf(stderr, "note: This is about %d%% space usage\n", (int)(((float)in_len/510.0)*100));

    	res = fwrite((void *)sector, 1, 512, stdout);
    	if (res != 512) {
        	fprintf(stderr, "note: Something fishy happened: fwrite(...) = %d\n", res);
    	}

	return 0;
}
