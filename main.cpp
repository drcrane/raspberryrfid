#include "sqlite3.hpp"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
//#include <fnctl.h>

#include "imagefn.hpp"
#include "utilityfn.hpp"
#include "userfn.hpp"
#include "cardidfn.hpp"
#include "readcard.h"
#include "appconfig.h"

/**
 * Crappy way to make your images complient with this programme...
 * convert ~/toni.jpg -rotate 270 -resize 1024x2048! toni2.jpg
 * or sth like it ;-)
 */

int main(int argc, char* argv[]) {
	std::string s;
	if (argc == 1) {
		fprintf(stdout, "Usage: ");
		fprintf(stdout, "%s\nCREATE\n"
			"ADDUSER id friendlyname jpegfile\n"
			"DELUSER id\n"
			"ADDCARD userid cardinhex\n"
			"RUN\n", argv[0]);
	}
	if (argc == 2) {
		if (strcmp(argv[1], "CREATE") == 0) {
			int res;
			SQLite3 sql("authusers.db");
			res = sql.exe("CREATE TABLE User (ID INTEGER NOT NULL, FriendlyName TEXT, Picture BLOB, PRIMARY KEY (ID))");
			fprintf(stdout, "User: %s\n", res == SQLITE_OK ? "OK" : "ERR");
			res = sql.exe("CREATE TABLE CardUser (CardID TEXT NOT NULL, UserID INTEGER NOT NULL, FOREIGN KEY (UserID) REFERENCES User(ID))");
			fprintf(stdout, "CardUser: %s\n", res == SQLITE_OK ? "OK" : "ERR");
		}
		if (strcmp(argv[1], "RUN") == 0) {
			SQLite3 sql("authusers.db");
			UserService userService(sql.GetDbHandle());
			CardService cardService(sql.GetDbHandle(), &userService);
			while (1) {
				unsigned char cardid[8];
				size_t cardidsz = 8;
				memset(cardid, 0, sizeof(cardid));
				User* user = NULL;
				int res;
				if ((res = ReadCard(cardid, &cardidsz)) == 0) {
					user = cardService.GetUserForCard(cardid, cardidsz);
					if (user != NULL) {
						fprintf(stdout, "Name: %s\n", user->GetFriendlyName());
						system("./opendoor.sh");
						delete user;
					} else {
						char* asciinumber;
						asciinumber = Utility::BinaryToHex((void *)cardid, cardidsz);
						fprintf(stdout, "Unknown User: %s\n", asciinumber);
						free(asciinumber);
					}
				} else {
					usleep(READING_DELAY);
				}
			}
		}
	}
	if (argc == 5 || argc == 3 || argc == 4) {
		SQLite3 sql("authusers.db");
		sql.exe("PRAGMA FOREIGN_KEYS = ON");
		unsigned long long int userid;
		if (strlen(argv[2]) <= 8) {
			char * endptr;
			/* 8 or fewer hex digits */
			userid = strtoul(argv[2], &endptr, 16);
			fprintf(stdout, "32bit User ID: 0x%08llx\n", userid);
		}
		if (strlen(argv[2]) > 8) {
			char * endptr;
			userid = strtoull(argv[2], &endptr, 16);
			fprintf(stdout, "32bit+ User ID: 0x%08llx\n", userid);
		}
		if (argc == 3) {
			if (strcmp(argv[1], "DELUSER") == 0) {
				UserService userfn(sql.GetDbHandle());
				fprintf(stdout, "Deleting user 0x%llx\n", userid);
				userfn.DeleteById(userid);
			}
			if (strcmp(argv[1], "DUMPUSER") == 0) {
				/* Not Implemented */
			}
		}
		if (argc == 4 && strcmp(argv[1], "ADDCARD") == 0) {
			char* outary;
			int len;
			Utility::HexToBinary(argv[3], (void**)&outary, &len);
			CardService* cardService = new CardService(sql.GetDbHandle(), (UserService*)NULL);
			cardService->Add(userid, outary, len);
			delete cardService;
			free(outary);
		}
		if (argc == 5 && strcmp(argv[1], "ADDUSER") == 0) {
			if (userid == 0) {
				fprintf(stderr, "Cannot have a user with a UserID of 0, sorry!\n");
				return 1;
			}
			//int imgfile = open(argv[4], O_RDONLY);
			{
				unsigned long int width, height, res;
				res = get_jpeg_size(argv[4], &width, &height);
				if (res < 0) {
					fprintf(stderr, "Invalid File Specified\n");
					return 1;
				}
				fprintf(stdout, "Jpeg Size: %ldx%ld\n", width, height);
				if (!Utility::IsPowerOf2(width) || !Utility::IsPowerOf2(height)) {
					fprintf(stderr, "OpenGL requires all images to be sized to the power of 2\n");
					return 1;
				}
			}
			
			UserService userfn(sql.GetDbHandle());
			User* user;
			unsigned int res;
			user = userfn.GetById(userid);
			if (user != NULL) {
				fprintf(stdout, "Cannot Add User: ID Already Exists\n");
				fprintf(stdout, "Friendly Name: %s\n", user->GetFriendlyName());
				delete(user);
				return 1;
			}
			
			fprintf(stdout, "Adding %s to the database...\n", argv[3]);
			long int filesz;
			void * filebuf;
			filesz = Utility::LoadFileIntoBuffer(&filebuf, argv[4]);
			if (!filesz > 0) {
				fprintf(stderr, "Problem with file when reading it\n");
				return 1;
			}
			res = 0;
			user = new User(userid, argv[3], (unsigned char *)filebuf, filesz);
			res = userfn.Add(user);
			if (res == 0) {
				fprintf(stdout, "%s added!\n", argv[3]);
			}
		}
		
	}
}
