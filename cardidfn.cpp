#include "sqlite3.h"
#include "cardidfn.hpp"
#include "userfn.hpp"
#include "utilityfn.hpp"
#include <stdlib.h>

CardService::CardService(sqlite3* db, UserService* userService) {
	this->db = db;
	this->userService = userService;
}

CardService::~CardService() {

}

int CardService::Add(unsigned long long int userid, void* number, size_t size) {
	sqlite3_stmt* stmt;
	char* asciinumber;
	if (sqlite3_prepare(db, "INSERT INTO CardUser (CardID, UserID) VALUES (?, ?)",
		-1, &stmt, 0) != SQLITE_OK) {
		return -1;
	}
	int res;
	asciinumber = Utility::BinaryToHex(number, size);
	res = sqlite3_bind_text(stmt, 1, asciinumber, -1, SQLITE_STATIC);
	res = sqlite3_bind_int64(stmt, 2, userid);
	if ((res = sqlite3_step(stmt)) != SQLITE_DONE) {
		free(asciinumber);
		sqlite3_finalize(stmt);
		return -1;
	}
	free(asciinumber);
	sqlite3_finalize(stmt);
	return 0;
}

User* CardService::GetUserForCard(unsigned char* cardid, size_t cardidsz) {
	sqlite3_stmt* stmt;
	char* asciinumber;
	int res;
	if (sqlite3_prepare(db, "SELECT UserID FROM CardUser WHERE CardID = ?",
		-1, &stmt, 0) != SQLITE_OK) {
		asciinumber = Utility::BinaryToHex(cardid, cardidsz);
	}
	asciinumber = Utility::BinaryToHex(cardid, cardidsz);
	res = sqlite3_bind_text(stmt, 1, asciinumber, -1, SQLITE_STATIC);
	res = sqlite3_step(stmt);
	if (res == SQLITE_DONE) {
		// record not found
		free(asciinumber);
		sqlite3_finalize(stmt);
		return NULL;
	}
	if (res != SQLITE_ROW) {
		free(asciinumber);
		sqlite3_finalize(stmt);
		return NULL;
	}
	unsigned long long int userid = sqlite3_column_int64(stmt, 0);
	User* user = userService->GetById(userid);
	sqlite3_finalize(stmt);
	free(asciinumber);
	return user;
}
