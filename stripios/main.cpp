/*	
	IOS ELF stripper, converts traditional ELF files into the format IOS wants.
	Copyright (C) 2008 neimod.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA	02110-1301	USA
*/

#include <stdio.h>
#include <stdint.h>


#define ELF_NIDENT 16

typedef struct 
{
	uint32_t		ident0;
	uint32_t		ident1;
	uint32_t		ident2;
	uint32_t		ident3;
	uint32_t		machinetype;
	uint32_t		version;
	uint32_t		entry;
	uint32_t		phoff;
	uint32_t		shoff;
	uint32_t		flags;
	uint16_t		ehsize;
	uint16_t		phentsize;
	uint16_t		phnum;
	uint16_t		shentsize;
	uint16_t		shnum;
	uint16_t		shtrndx;
}__attribute__((packed)) elfheader;

typedef struct 
{
	uint32_t		type;
	uint32_t		offset;
	uint32_t		vaddr;
	uint32_t		paddr;
	uint32_t		filesz;
	uint32_t		memsz;
	uint32_t		flags;
	uint32_t		align;
}__attribute__((packed)) elfphentry;

typedef struct
{
	uint32_t offset;
	uint32_t size;
}__attribute__((packed)) offset_size_pair;

uint16_t getbe16(void* pvoid)
{
	unsigned char* p = (unsigned char*)pvoid;

	return (p[0] << 8) | (p[1] << 0);
}

uint32_t getbe32(void* pvoid)
{
	unsigned char* p = (unsigned char*)pvoid;

	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3] << 0);
}

void putbe16(void* pvoid, uint16_t val)
{
	unsigned char* p = (unsigned char*)pvoid;

	p[0] = val >> 8;
	p[1] = val >> 0;
}

void putbe32(void* pvoid, uint32_t val)
{
	unsigned char* p = (unsigned char*)pvoid;

	p[0] = val >> 24;
	p[1] = val >> 16;
	p[2] = val >> 8;
	p[3] = val >> 0;
}

int main(int argc, char* argv[])
{
	int result = -1;
	elfheader header;
	uint32_t elfmagicword = 0;
	uint32_t phoff = 0;
	uint16_t phnum = 0;
	uint32_t memsz = 0, filesz = 0;
	uint32_t vaddr = 0, paddr = 0;	
	elfphentry* origentries = NULL;
	elfphentry* iosphentry = NULL;
	elfphentry* entries = NULL;
	offset_size_pair* offsetsizes = NULL;
	elfphentry* q = NULL;

	fprintf(stdout, "stripios - IOS ELF stripper - by neimod\n");
	if (argc != 3)
	{
		fprintf(stderr,"Usage: %s <in.elf> <out.elf>\n", argv[0]);

		return -1;
	}

	FILE* fin = fopen(argv[1], "rb");
	FILE* fout = fopen(argv[2], "wb");

	if (fin == 0 || fout == 0)
	{
		if (fin == 0)
			fprintf(stderr,"ERROR opening file %s\n", argv[1]);
		if (fout == 0)
			fprintf(stderr,"ERROR opening file %s\n", argv[2]);
		goto cleanup;
	}



	if (fread(&header, sizeof(elfheader), 1, fin) != 1)
	{
		fprintf(stderr,"ERROR reading ELF header\n");
		goto cleanup;
	}

	elfmagicword = getbe32(&header.ident0);

	if (elfmagicword != 0x7F454C46)
	{
		fprintf(stderr,"ERROR not a valid ELF\n");
		goto cleanup;
	}

	phoff = getbe32(&header.phoff);
	phnum = getbe16(&header.phnum);
	memsz = 0; filesz = 0;
	vaddr = 0; paddr = 0;

	putbe32(&header.ident1, 0x01020161);
	putbe32(&header.ident2, 0x01000000);
	putbe32(&header.ident3, 0);
	putbe32(&header.machinetype, 0x20028);
	putbe32(&header.version, 1);
	putbe32(&header.flags, 0x606);
	putbe16(&header.ehsize, 0x34);
	putbe16(&header.phentsize, 0x20);
	putbe16(&header.shentsize, 0);
	putbe16(&header.shnum, 0);
	putbe16(&header.shtrndx, 0);
	putbe32(&header.phoff, 0x34);
	putbe32(&header.shoff, 0);

	putbe16(&header.phnum, phnum + 2);

	origentries = new elfphentry[phnum];

	fseek(fin, phoff, SEEK_SET);
	if (fread(origentries, sizeof(elfphentry), phnum, fin) != phnum)
	{
		fprintf(stderr,"ERROR reading program header\n");
		goto cleanup;
	}


	iosphentry = 0;

	// Find zero-address phentry
	for(int i=0; i<phnum; i++)
	{
		elfphentry* phentry = &origentries[i];
		if (getbe32(&phentry->paddr) == 0)
		{
			iosphentry = phentry;
		}
	}

	if (0 == iosphentry)
	{
		fprintf(stderr,"ERROR IOS table not found in program header\n");
		goto cleanup;
	}
	

	entries = new elfphentry[phnum+2];
	offsetsizes = new offset_size_pair[phnum];

	q = entries;
	phoff = 0x34;

	for(int i=0; i<phnum; i++)
	{
		elfphentry phentry;
		elfphentry* p = &origentries[i];

		offsetsizes[i].offset = getbe32(&p->offset);
		offsetsizes[i].size = getbe32(&p->filesz);

		if (p == iosphentry)
		{
			uint32_t startoffset = phoff;
			uint32_t startvaddr = vaddr;
			uint32_t startpaddr = paddr;
			uint32_t totalsize = 0;

			filesz = memsz = (phnum+2) * 0x20;

			// PHDR 
			putbe32(&phentry.type, 6);
			putbe32(&phentry.offset, phoff);
			putbe32(&phentry.vaddr, paddr);
			putbe32(&phentry.paddr, vaddr);
			putbe32(&phentry.filesz, filesz);
			putbe32(&phentry.memsz, memsz);
			putbe32(&phentry.flags, 0x00F00000);
			putbe32(&phentry.align, 0x4);

			*q++ = phentry;	

			phoff += memsz;
			paddr += memsz;
			vaddr += memsz;
			totalsize += memsz;

			filesz = memsz = getbe32(&p->memsz);

			// NOTE 
			putbe32(&phentry.type, 4);
			putbe32(&phentry.offset, phoff);
			putbe32(&phentry.vaddr, vaddr);
			putbe32(&phentry.paddr, paddr);
			putbe32(&phentry.filesz, filesz);
			putbe32(&phentry.memsz, memsz);
			putbe32(&phentry.flags, 0x00F00000);
			putbe32(&phentry.align, 0x4);

			*q++ = phentry;	

			phoff += memsz;
			paddr += memsz;
			vaddr += memsz;
			totalsize += memsz;

			filesz = memsz = totalsize;

			// LOAD
			putbe32(&phentry.type, 1);
			putbe32(&phentry.offset, startoffset);
			putbe32(&phentry.vaddr, startvaddr);
			putbe32(&phentry.paddr, startpaddr);
			putbe32(&phentry.filesz, totalsize);
			putbe32(&phentry.memsz, totalsize);
			putbe32(&phentry.flags, 0x00F00000);
			putbe32(&phentry.align, 0x4000);

			*q++ = phentry;
		}
		else
		{
			filesz = getbe32(&p->filesz);
			memsz = getbe32(&p->memsz);

			putbe32(&phentry.type, getbe32(&p->type));
			putbe32(&phentry.offset, phoff);
			putbe32(&phentry.vaddr, getbe32(&p->vaddr));
			putbe32(&phentry.paddr, getbe32(&p->paddr));
			putbe32(&phentry.filesz, filesz);
			putbe32(&phentry.memsz, memsz);
			putbe32(&phentry.flags, getbe32(&p->flags));
			//putbe32(&phentry.align, getbe32(&p->align));
			putbe32(&phentry.align, 0x4);

			*q++ = phentry;

			phoff += filesz;
		}
	}
	
	if (fwrite(&header, sizeof(elfheader), 1, fout) != 1)
	{
		fprintf(stderr,"ERROR writing ELF header\n");
		goto cleanup;
	}

	if (fwrite(entries, sizeof(elfphentry), phnum+2, fout) != (phnum+2))
	{
		fprintf(stderr,"ERROR writing ELF program header\n");
		goto cleanup;
	}

	for(int i=0; i<phnum; i++)
	{
		elfphentry *p = &origentries[i];

		uint32_t offset = getbe32(&p->offset);
		uint32_t filesz = getbe32(&p->filesz);

		if (filesz)
		{
			fseek(fin, offset, SEEK_SET);

			fprintf(stdout,"Writing segment 0x%08X to 0x%08X (%d bytes)\n", getbe32(&p->vaddr), (unsigned int)ftell(fout), filesz);

			unsigned char* data = new unsigned char[filesz];

			
			if (fread(data, filesz, 1, fin) != 1 || fwrite(data, filesz, 1, fout) != 1)
			{
				fprintf(stderr,"ERROR writing segment\n");
				delete[] data;
				goto cleanup;
			}

			delete[] data;
		}
		else
		{
			fprintf(stdout,"Skipping segment 0x%08X\n", getbe32(&p->vaddr));
		}
	}
	
	result = 0;
	printf("Done!\n");

cleanup:
	if (offsetsizes)
		delete[] offsetsizes;
	if (entries)
		delete[] entries;
	if (origentries)
		delete[] origentries;
	if (fout)
		fclose(fout);

	if (fin)
		fclose(fin);

	return result;
}

