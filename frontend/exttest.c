/* Example program using extended ASCII */

/* Note: Entering international characters from the command prompt does not always
   produce the required output. Zint assumes that the text is extended ASCII already
   whereas the default system (on OpenSUSE 11.0 at least) is UTF-8. This example
   shows a basic example of how you can produce barcodes with sensible
   extended ASCII content. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zint.h>

int main()
{
	struct zint_symbol *my_symbol;
	char test_string[8];
	
	my_symbol = ZBarcode_Create();
	
	my_symbol->height = 50;
	my_symbol->symbology = BARCODE_CODE128;
	strcpy(my_symbol->outfile, "barext.png");
	test_string[0] = 'g';
	test_string[1] = 'r';
	test_string[2] = 246; /* ö */
	test_string[3] = 223; /* ß */
	test_string[4] = 'e';
	test_string[5] = 'r';
	test_string[6] = '\0';
	
	if(ZBarcode_Encode_and_Print(my_symbol, test_string) != 0) {
		printf("%s\n", my_symbol->errtxt);
		return 1;
	}
	
	ZBarcode_Delete(my_symbol);
	
	return 0;
}
