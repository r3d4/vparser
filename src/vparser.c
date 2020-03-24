/* 
 * This file is part of the vparser distribution (https://github.com/r3d4/vparser).
 * Copyright (c) 2020 Henrik Schondorff.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <byteswap.h>
#include <libgen.h>


#define INFO_START				0x8000

#define INFO_SIZE				0x8000
#define INFO_OFFSET_TOKEN		0x0000
#define INFO_OFFSET_YEAR		0x0002
#define INFO_OFFSET_MONTH		0x0004
#define INFO_OFFSET_DAY			0x0005
#define INFO_OFFSET_HOUR		0x0006
#define INFO_OFFSET_MINUTE      0x0007
#define INFO_OFFSET_SECOND      0x0008
#define INFO_OFFSET_FILE		0x0014
#define INFO_OFFSET_CHANNEL		0x0032
#define INFO_OFFSET_NAME		0x0432
#define INFO_OFFSET_DESCRIPTION	0x0832
#define INFO_OFFSET_TRANSPONDER	0x1032
#define INFO_OFFSET_LANGUAGE	0x2189


struct info_time 
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
};

void show_help(char *name)
{
	printf("%s - parse RECS.INF file from Vestel TV \n\n", name );
	printf("Usage:  %s <recs_info_filename> \n\n", name);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		show_help(basename(argv[0]));
		return 0;
	}

	char *filename = argv[1];
	FILE *fp = fopen(filename, "rb");
	if(fp == NULL)
	{
		printf("Error opening file: %s\n", filename);
		return 1;
	}

	//printf("using file %s ...\n", filename);

	fseek(fp, 0, SEEK_END);
	int file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int recodings = (file_size - INFO_START) / INFO_SIZE;

	if(recodings < 1)
	{
		printf("Error no recordings found\n");
		fclose(fp);
		return 1;
	}
	//printf("found %d recordings\n", recodings);

	uint8_t *buffer = (uint8_t *) malloc(INFO_SIZE);
	if( buffer == NULL)
	{
		fclose(fp);
		return 1;
	}

	printf("Date, File, Name, Channel, Description, Transponder, Language\n");

	fseek(fp, INFO_START, SEEK_SET);
	for(int i = 0; i < recodings; i++)
	{
		size_t size = fread(buffer, 1, INFO_SIZE, fp);
		if(size != INFO_SIZE)
		{
			printf("read size fails s = %ld\n", size);
			break;
		}
		if(buffer[INFO_OFFSET_TOKEN] != 'R')
		{
			printf("wrong record header: %d", i);
			continue;
		}
		
        struct info_time itime;
		memset(&itime, 0, sizeof(itime));
		memcpy(&itime, buffer + INFO_OFFSET_YEAR, sizeof(itime));
		itime.year = bswap_16(itime.year);

		printf("%04d-%02d-%02d %02d:%02d:%02d, %s, \"%s\", \"%s\", \"%s\", \"%s\", \"%.3s\"\n",
			itime.year, itime.month, itime.day,
			itime.hour, itime.minute, itime.second,
			buffer + INFO_OFFSET_FILE,
			buffer + INFO_OFFSET_NAME,
			buffer + INFO_OFFSET_CHANNEL,
			buffer + INFO_OFFSET_DESCRIPTION,
			buffer + INFO_OFFSET_TRANSPONDER,
			buffer + INFO_OFFSET_LANGUAGE
			);
	}

	free(buffer);
	fclose(fp);

	return 0;
}
