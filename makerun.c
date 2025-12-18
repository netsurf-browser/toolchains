#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#ifdef __riscos
  #include <unixlib/local.h>
  #include <oslib/osfile.h>
#endif

struct header {
	uint32_t decompress;
	uint32_t selfreloc;
	uint32_t zeroinit;
	uint32_t entry;
	uint32_t exit;
	uint32_t rosize;
	uint32_t rwsize;
	uint32_t dbsize;
	uint32_t zisize;
	uint32_t dbtype;
	uint32_t base;
	uint32_t wkspace;
	uint32_t flags;
	uint32_t database;
	uint32_t r1;
	uint32_t r2;
	uint32_t zicode[16];
};

struct sqz_header {
	uint32_t decsize;
	uint32_t encsize;
	uint32_t tabsize;
	uint32_t nshorts;
	uint32_t nlongs;
	uint32_t bytestomove;
};

int main(int argc, char **argv)
{
	FILE *f, *g;
	struct header header;
	long flen;
	size_t file_size, reloc_size = 0, expected_size = 0;
	char buf[256];

	if (argc != 4) {
		fprintf(stderr, "Usage: %s <runimage> <input> <output>\n", 
				argv[0]);
		return 1;
	}

	f = fopen(argv[1], "rb");
	if (f == NULL) {
		fprintf(stderr, "Failed opening %s\n", argv[1]);
		return 1;
	}

	if (fseek(f, 0, SEEK_END) < 0 || (flen = ftell(f)) < 0 ||
			fseek(f, 0, SEEK_SET) < 0) {
		fprintf(stderr, "Failed reading file size\n");
		fclose(f);
		return 1;
	}
	file_size = (size_t) flen;

	if (file_size < sizeof(struct header) ||
			fread(&header, sizeof(struct header), 1, f) != 1) {
		fprintf(stderr, "Failed reading image header");
		fclose(f);
		return 1;
	}
	expected_size = header.rosize + header.rwsize + header.dbsize;

	if ((header.decompress >> 24) == 0xEB) {
		/* Image is compressed */
		struct sqz_header sqzhdr;
		uint8_t buf[1024], *lastb;
		/* Compute offset of squeeze data (if any) */
		size_t off = ((header.decompress & 0xFFFFFF) << 2) + 8 - (6*4);
		/* Read in at most 1024 trailing bytes */
		if (file_size - off > sizeof(buf)) {
			fprintf(stderr, "Decompression buffer too small\n");
			fclose(f);
			return 1;
		}
		if (fseek(f, (long) off, SEEK_SET) < 0 ||
				fread(buf, file_size - off, 1, f) != 1) {
			fprintf(stderr, "Failed reading decompression code\n");
			fclose(f);
			return 1;
		}
		/* Look for squeeze signature (file size must be a multiple
		 * of 15 and must end "rcc X.YZ\n" padded out with spaces */
		lastb = buf + (file_size - off);
		while (lastb > buf && *(lastb-1) == ' ')
			lastb--;
		if ((file_size & 0xf) != 0xf || (lastb - 9 <= buf) ||
				*(lastb-9) != 'r' || *(lastb-8) != 'c' ||
				*(lastb-7) != 'c' || *(lastb-6) != ' ' ||
				*(lastb-4) != '.' || *(lastb-1) != '\n') {
			fprintf(stderr, "Unknown compression scheme\n");
			fclose(f);
			return 1;
		}

		/* We have been squeezed: sanity check compressed size */
		memcpy(&sqzhdr, buf, sizeof(sqzhdr));
		if (sizeof(header) + 4 + sqzhdr.encsize + sqzhdr.tabsize !=
				off) {
			fprintf(stderr, "Malformed squeeze data\n");
			fclose(f);
			return 1;
		}

		/* Squeeze always processes pairs of words at a time,
		 * so the declared decompressed size will always be
		 * rounded up to an 8-byte boundary. We will need to
		 * adjust expected_size to account for this but, if
		 * the image is also relocateable, the file size will
		 * be modified instead as a result of discarding the
		 * relocation code and tables from the end of the
		 * image. Therefore, we'll defer the expected_size
		 * adjustment until later.
		 */
		file_size = sizeof(header) + (size_t) sqzhdr.decsize;
	}

	fclose(f);

	if (header.decompress == 0x464c457f /* FLE\x7f */) {
		/* ELF binary */
		file_size += 0x8000; /* Add 32k of scratch space */
	} else {
		if ((header.entry >> 24) != 0xEB) {
			fprintf(stderr, "%s not binary\n", argv[1]);
			return 1;
		}

		if ((header.selfreloc >> 24) == 0xEB) {
			/* Image is relocateable */
			size_t off = 4 +
				((header.selfreloc & 0xFFFFFF) << 2) + 8;

			if (off >= file_size) {
				fprintf(stderr, "Relocation code missing\n");
				return 1;
			}

			/* Deduct relocation code and tables from file size */
			reloc_size = file_size - off;
			file_size -= reloc_size;
			/* Add relocation workspace on to reloc size */
			reloc_size += header.wkspace;
		} else if ((header.decompress >> 24) == 0xEB) {
			/* Image is compressed, but not relocateable */
			/* Round expected size up to an even number of words
			 * (see above for details) */
			expected_size = (expected_size + 7) & ~7;
		}

		if (file_size != expected_size) {
			fprintf(stderr, "Mismatched field sizes\n");
			return 1;
		}

#define max(a, b) ((a) > (b) ? (a) : (b))
		file_size += max(header.zisize, reloc_size) +
				0x8000 /* 32k of scratch space */;
#undef max
	}

	/* Round slotsize up to the next 4k boundary */
	file_size = (file_size + 0xfff) & ~0xfff;

	f = fopen(argv[2], "r");
	if (f == NULL) {
		fprintf(stderr, "Failed opening %s\n", argv[2]);
		return 1;
	}

	g = fopen(argv[3], "w");
	if (g == NULL) {
		fclose(f);
		fprintf(stderr, "Failed opening %s\n", argv[3]);
		return 1;
	}

	while (fgets(buf, sizeof(buf), f) != NULL) {
		if (strncmp(buf, "WIMPSLOT\n", 9) == 0) {
			fprintf(g, "WimpSlot -min %zuk -max %zuk\n",
					(file_size / 1024), (file_size / 1024));
		} else {
			fputs(buf, g);
		}
	}

	fclose(g);
	fclose(f);

#ifdef __riscos
	if (__riscosify(argv[3], 0, __RISCOSIFY_STRICT_UNIX_SPECS | 
			__RISCOSIFY_NO_SUFFIX | __RISCOSIFY_FILETYPE_NOT_SET, 
			buf, sizeof(buf), NULL) == NULL) {
		fprintf(stderr, "Riscosify failed\n");
		return 1;
	}

	osfile_set_type(buf, osfile_TYPE_OBEY);
#endif

	return 0;
}

