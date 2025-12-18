/*
 * ReduceAOF
 * =========
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
	unsigned int ChunkFileId;
	unsigned int maxChunks;
	unsigned int numChunks;
} ChunkFileHdr;

typedef struct
{
	unsigned char chunkId[8];
	unsigned int fileOffset;
	unsigned int size;
} ChunkEntryHdr;

typedef struct
{
	ChunkFileHdr CFH;
	ChunkEntryHdr *CEH;
	char **DAT;
} ChunkInfoStr;

static int verbose;

static int bite(ChunkInfoStr *I, FILE *fp)
{
	size_t m = fread(I->CEH, sizeof(*I->CEH), I->CFH.maxChunks, fp);

	if (m == I->CFH.maxChunks)
	{
		int n;
		for (n=0; n < I->CFH.maxChunks; ++n)
		{
			char name[9];
			strncpy(name, (const char *) I->CEH[n].chunkId, 8);
			name[8]='\0';
			I->DAT[n] = NULL;

			if ( I->CEH[n].fileOffset != 0 )
			{
				I->DAT[n] = malloc(I->CEH[n].size);
				if (I->DAT[n] == NULL)
				{
					fprintf(stderr, "malloc() failed\n");
					abort();
				}

				fseek(fp, I->CEH[n].fileOffset, SEEK_SET);
				if (fread((I->DAT[n]), 1, I->CEH[n].size, fp) != I->CEH[n].size)
				{
					free(I->DAT[n]); I->DAT[n] = NULL; m--;
				}
			}
		}
	}

	return m == I->CFH.maxChunks;
}

static void strip(ChunkInfoStr *I)
{
	unsigned int n;
	for (n=0; n < I->CFH.maxChunks; ++n)
	{
		char name[9];
		strncpy(name, (const char *) I->CEH[n].chunkId, 8);
		name[8]='\0';

		if ( I->DAT[n] != NULL && !strcmp(name, "OBJ_IDFN") )
		{
			free(I->DAT[n]); I->DAT[n] = NULL;
		}
	}
}

static unsigned int brush(ChunkInfoStr *I)
{
	unsigned int n, to = 0;

	for (n=0; n < I->CFH.maxChunks; ++n)
	{
		if (I->DAT[n] != NULL && to != n)
		{
			memcpy(&I->CEH[to], &I->CEH[n],
			sizeof(*I->CEH)); I->DAT[to] = I->DAT[n];
			I->DAT[n] = NULL;
		}

		if (I->DAT[to] != NULL)
			to++;
		else
		{
			memcpy( I->CEH[n].chunkId, "SpareNow", 8);
			I->CEH[n].fileOffset = 0;
			I->CEH[n].size = 0;
		}
	} return to;
}

static void reorganise(ChunkInfoStr *I, unsigned int got)
{
	unsigned int n, place;
	I->CFH.maxChunks = I->CFH.numChunks = got;
	place = sizeof(I->CFH) + sizeof(*I->CEH) * I->CFH.maxChunks;

	for (n=0; n < I->CFH.numChunks; ++n)
	{
		I->CEH[n].fileOffset = place;
		place += (I->CEH[n].size + 3) & ~3;
	}
}

static int get_CFH(ChunkInfoStr *I, FILE *fp)
{
	if (fread(&I->CFH, sizeof(I->CFH), 1, fp) != 1)
		return 0;

	return I->CFH.ChunkFileId == 0xC3CBC6C5;
}

static void operate(const char *fname)
{
	FILE *fp;

	if (verbose)
		fprintf(stderr, "ReduceAOF %s \n", fname );

	if ((fp = fopen(fname, "rb")) != NULL)
	{
		ChunkInfoStr CIS;

		if (get_CFH(&CIS, fp) != 0 && CIS.CFH.numChunks > 0)
		{
			CIS.CEH = malloc(sizeof(*CIS.CEH) * CIS.CFH.maxChunks);
			CIS.DAT = malloc(sizeof(char*) * CIS.CFH.maxChunks);

			if (!CIS.CEH || !CIS.DAT)
			{
				fprintf(stderr, "malloc() failed\n"); abort();
			}

			if (bite(&CIS, fp) != 0)
			{
				int n;
				fclose(fp);
				fp = NULL;
				strip(&CIS);
				reorganise(&CIS, brush(&CIS));

				fp = fopen(fname, "wb");
				if (fp)
				{
					fwrite(&CIS.CFH, sizeof(CIS.CFH), 1, fp);
					fwrite(CIS.CEH, sizeof(*CIS.CEH), CIS.CFH.maxChunks, fp);

					for (n=0; n < CIS.CFH.maxChunks; ++n)
					{
						fseek(fp, CIS.CEH[n].fileOffset, SEEK_SET);
						fwrite((CIS.DAT[n]), sizeof(char), CIS.CEH[n].size, fp);
					}
				}
				else
				{
					fprintf(stderr, "Could not write %s\n", fname);
					exit(EXIT_FAILURE);
				}

				for (n=0; n < CIS.CFH.maxChunks; ++n)
				{
					free(CIS.DAT[n]);
				}
			}
			else
			{
				fprintf(stderr, "Error reading %s\n", fname);
				exit(EXIT_FAILURE);
			}

			free(CIS.CEH);
		}

		if (fp)
		{
			if (fclose(fp) != 0)
			{
				fprintf(stderr, "Error closing %s\n", fname);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			fprintf(stderr, "Wrong format %s\n", fname);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		fprintf(stderr, "Could not read %s\n", fname);
		exit(EXIT_FAILURE);
	}
}

static void plenty(const char *n)
{
	FILE *fp;

	if ((fp = fopen(n, "r")) != NULL)
	{
		char s[256];
		while (!feof(fp))
		{
			if (fgets(s, sizeof(s), fp) != NULL)
			{
				char *t = strchr(s, '\n');
				if (t != NULL) *t = '\0'; operate(s);
			}
		}
		fclose(fp);
	}
	else
	{
		fprintf(stderr, "Error reading %s\n", n);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	int n, go = argc > 1;

	for (n = 1; n < argc; ++n)
	{
		if (!strcmp(argv[n], "-?") || !strcmp(argv[n], "-h") || !strcmp(argv[n], "-help"))
			go = 0;

		if (!strcmp(argv[n], "-via"))
		{
			if (argv[++n] == NULL) go = 0;
		}

		if ( !strcmp( argv[n], "-v" ) )
			verbose = 1;
	}

	if (go)
	{
		for (n = 1; n < argc; ++n)
		{
			if (!strcmp(argv[n], "-via"))
			{
				plenty(argv[++n]);
				continue;
			}
			if ( *argv[n] != '-' )
				operate(argv[n]);
		}
	}
	else
	{
		fprintf(stderr, "ReduceAOF "
				__DATE__
				". "
				"Copyright © Richard Sargeant, 1999.\n"
				"ReduceAOF comes with ABSOLUTELY NO WARRANTY.\n"
			);
	}

	return n == argc ? EXIT_SUCCESS : EXIT_FAILURE;
} /* Just written for OSLib :-) mailto:oslib@arcade.demon.co.uk */

